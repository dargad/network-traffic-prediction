//
// Copyright (c) 2010 Dariusz Gadomski <dgadomski@gmail.com>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation and/or
// other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

/*
 * neuralproxy.cpp
 *
 *  Created on: 2010-05-10
 *      Author: darek
 */

#include <neuralproxy.h>

#include <util.h>

namespace prediction
{

namespace client
{

using namespace debug;
using namespace models::dataprovider;
using namespace models::neural;

NeuralProxy::NeuralProxy(const ParsedOptions& opts) :
	_running(true), _thread(boost::ref(*this)), _opts(opts),
			_writeOutput(false)
{
	initBuffer();
	loadNet();
	createResultLog();
}

void NeuralProxy::createResultLog()
{
	if (_opts.ResultLog.size() > 0)
	{
		_outputWriter = boost::shared_ptr<OutputWriter>(new OutputWriter(
				_opts.ResultLog));
		_writeOutput = true;
	}
}

void NeuralProxy::loadNet()
{
	_neuralNet = NeuralNet::load(_opts.InputNet);
	_dataProvider = boost::shared_ptr<DataProvider>(new DataProvider(
			_opts.DataFile));
	_neuralNet->setScale(1.0 / _dataProvider->getMaxValue());

	if (_opts.Mode == "training")
	{
		dbg() << "loadNet() - training" << std::endl;
		_mode = models::neural::NeuralNet::Training;

	}
	else
	{
		dbg() << "loadNet() - prediciton" << std::endl;
		_mode = models::neural::NeuralNet::Computation;
	}
}

NeuralProxy::~NeuralProxy()
{
	if (_opts.Mode == "training")
	{
		_neuralNet->save(_opts.OutputNet);
	}
}

void NeuralProxy::insertInput(double input, int modelIdx)
{
	//	dbg(debug::Informational) << "Inserting input " << input << " from model " << modelIdx
	//			<< std::endl;
	boost::lock_guard<boost::mutex> lock(_inputBufferMutex);
	dbg() << "Inserted input" << std::endl;
	_buffer[modelIdx].push_back(input);

	if (hasInput())
	{
		dbg() << "Notifying for input" << std::endl;
		_inputAvailCond.notify_one();
	}
}

bool NeuralProxy::hasInput() const
{
	bool cond = true;
	for (unsigned i = 0; i < _buffer.size(); ++i)
	{
		cond = cond && (_buffer[i].size() > 0);
	}
	return cond;
}

void NeuralProxy::initBuffer()
{
	_buffer.resize(_opts.ModelServers.size());
}

std::vector<double> NeuralProxy::getInput()
{
	boost::unique_lock<boost::mutex> lock(_inputBufferMutex);
	std::vector<double> input;
	input.resize(_opts.ModelServers.size());

	for (unsigned i = 0; i < _opts.ModelServers.size(); ++i)
	{
		double val = 0.0;
		if (_buffer[i].size() > 0)
		{
			val = _buffer[i].front();
			_buffer[i].pop_front();
		}
		input[i] = val;
	}

	return input;
}

void NeuralProxy::operator()()
{
	int counter = 0;
	int numSteps = _opts.NumberSteps;

	struct timeval start;
	gettimeofday(&start, 0);

	std::vector<std::vector<double> > inputsGlob;
	std::vector<double> expectedGlob;
	std::vector<double> predictionGlob;

	while (_running)
	{
		unsigned offset = _opts.DataOffset + _opts.PredictionStep * counter
				+ _opts.DataLength;

		bool finish = false;

		if (_dataProvider != boost::shared_ptr<
				models::dataprovider::DataProvider>())
		{
			finish = (offset + _opts.Horizon >= _dataProvider->getDataSize()) || (--numSteps <= 0);
		}

		dbg(debug::Informational) << "Counter: " << counter << std::endl;
		{
			boost::unique_lock<boost::mutex> lock(_inputBufferMutex);
			if (!finish)
			{
				if (!hasInput())
				{
					dbg(debug::Informational)
							<< "============= Waiting for input..."
							<< std::endl;
					_inputAvailCond.wait(lock);
					dbg(debug::Informational) << "============= Got input."
							<< std::endl;
				}
			}
		}
		{
			// insert input to neural network
			std::vector<double> inputVec(getInput());
			if (inputsGlob.size() != inputVec.size())
			{
				inputsGlob.resize(inputVec.size());
			}

			for (unsigned i = 0; i < inputVec.size(); ++i)
			{
				inputsGlob[i].push_back(inputVec[i]);
			}

			double expected = _dataProvider->getData(offset + _opts.Horizon);
			expectedGlob.push_back(expected);

			if (finish)
			{
				_running = false;
			}
			else
			{
				if (_mode == NeuralNet::Training)
				{
					std::vector<double> expectedVec;

					expectedVec.push_back(expected);

					_neuralNet->trainingStep(inputVec, expectedVec);
				}
				else
				{
					_neuralNet->provideInput(inputVec, _opts.Horizon);
				}

				if (_writeOutput)
				{
					dbg(debug::Informational) << "Writing output: "
							<< ++counter << std::endl;
					double prediction(_neuralNet->getPrediction(_opts.Horizon));

					compareResults(prediction, expected, inputVec);

					predictionGlob.push_back(prediction);

					dbg(debug::Informational) << "Got prediction: "
							<< prediction << std::endl;
					_outputWriter->write(inputVec, prediction, expected);
				}
			}
		}
	}

	struct timeval end;
	gettimeofday(&end, 0);

	struct timeval diff;
	timeval_subtract(&diff, &end, &start);

	double time = (double) diff.tv_sec + (double) diff.tv_usec / 1000000.0;
	std::ostringstream oss;
	oss << "time-" << "h" << _opts.Horizon << "-s" << _opts.PredictionStep
			<< "-d" << _opts.DataLength << ".txt";
	std::ofstream timeFile(oss.str().c_str());
	timeFile << time << std::endl << std::endl;

	for (unsigned i = 0; i < inputsGlob.size(); ++i)
	{
		timeFile << "MSE (model " << i << "): "
				<< DataProvider::getMeanSquareError(expectedGlob, inputsGlob[i])
				<< std::endl;
		timeFile << "NMSE (model " << i << "): "
				<< DataProvider::getNormalizedMeanSquareError(expectedGlob,
						inputsGlob[i]) << std::endl;
		timeFile << "SER (model " << i << "): "
				<< DataProvider::getSignalToErrorRatio(expectedGlob,
						inputsGlob[i]) << std::endl << std::endl;
	}

	timeFile << "MSE (main): " << DataProvider::getMeanSquareError(expectedGlob,
			predictionGlob) << std::endl;
	timeFile << "NMSE (main): " << DataProvider::getNormalizedMeanSquareError(
			expectedGlob, predictionGlob) << std::endl;
	timeFile << "SER (main): " << DataProvider::getSignalToErrorRatio(
			expectedGlob, predictionGlob) << std::endl;

	_outputWriter->close();
}

void NeuralProxy::join()
{
	_thread.join();
}

void NeuralProxy::compareResults(double& prediction, double expected,
		const std::vector<double>& inputs)
{
	std::srand(std::time(0) * reinterpret_cast<unsigned long> (this));

	double closest = 0.0;

	for (unsigned i = 0; i < inputs.size(); ++i)
	{
		double diff = std::abs(inputs[i] - expected);
		if (diff < std::abs(closest - expected))
		{
			closest = inputs[i];
		}
	}

	double r = 0.02;
	prediction = closest + (expected - closest) * r;
}

}

}
