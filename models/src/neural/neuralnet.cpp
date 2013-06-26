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
 * neuralnet.cpp
 *
 *  Created on: 2010-04-15
 *      Author: darek
 */

#include <neural/neuralnet.h>

#include <neural/layer.h>
#include <neural/neuron.h>
#include <neural/netserializer.h>
#include <util.h>

#include <iostream>

namespace models
{
namespace neural
{

using namespace debug;

NeuralNet::NeuralNet() :
	_mode(Off), _numLearningSteps(0), _learningFactor(BASE_LEARNING_FACTOR),
			_scale(1.0)
{
	updateLearningFactor();
	dbg() << "NeuralNet()" << std::endl;
}

NeuralNet::~NeuralNet()
{
	dbg() << "~NeuralNet()" << std::endl;
}

void NeuralNet::setLayers(const std::vector<boost::shared_ptr<Layer> >& layers)
{
	_layers = layers;

	boost::shared_ptr<Layer> prevLayer;
	for (std::vector<boost::shared_ptr<Layer> >::iterator it = _layers.begin(); it
			!= _layers.end(); ++it)
	{
		boost::shared_ptr<Layer> layer = *it;
		dbg() << "Inserting " << layer->getName() << std::endl;
		layer->setCallback(std::bind1st(std::mem_fun(
				&NeuralNet::stepDoneCallback), this));

		if (prevLayer != boost::shared_ptr<Layer>())
		{
			dbg() << "!!! Assigning " << prevLayer->getName() << " before "
					<< layer->getName() << std::endl;
			_nextMap[prevLayer->getName()] = layer;
			_prevMap[layer->getName()] = prevLayer;
		}
		prevLayer = layer;
	}
}

void NeuralNet::provideInput(const std::vector<double>& inputValues,
		unsigned horizon)
{
	_mode = Computation;
	_resultBuffer.clear();
	_inputBuffer = scaleVector(inputValues);

	_layers[0]->insertInput(_inputBuffer);
}

void NeuralNet::trainingStep(const std::vector<double>& inputValues,
		std::vector<double>& expectedOutput)
{
	_mode = Training;
	_expectedOutput = scaleVector(expectedOutput);
	_inputBuffer = scaleVector(inputValues);
	_resultBuffer.clear();
	_layers[0]->insertInput(_inputBuffer);

}

void NeuralNet::stepDoneCallback(Layer *layer)
{
	if (_nextMap.find(layer->getName()) != _nextMap.end())
	{
		boost::shared_ptr<Layer> nextLayer = _nextMap[layer->getName()];
		nextLayer->insertInput(layer->getOutput());
	}
	else
	{
		if (_mode == Computation)
		{
			_resultBuffer.push_back(layer->getOutput()[0]);
		}
		else if (_mode == Training)
		{
			backPropagationTraining(layer);
			_resultBuffer.push_back(layer->getOutput()[0]);
		}

	}

}

void NeuralNet::calculateErrorValues(Layer *outputLayer)
{
	std::vector<double> output = outputLayer->getOutput();
	std::vector<double> outputErrors;
	for (size_t i = 0; i < outputLayer->neuronCount(); ++i)
	{
		outputErrors.push_back(_expectedOutput[i] - output[i]);
		dbg() << "===========================" << std::endl;
		dbg() << "output: " << output[i] << std::endl;
		dbg() << "expected: " << _expectedOutput[i] << std::endl;
		dbg() << "output errors: " << _expectedOutput[i] - output[i]
				<< std::endl;
		dbg() << "===========================" << std::endl;
	}
	outputLayer->setErrorValues(outputErrors);
	for (std::vector<boost::shared_ptr<Layer> >::reverse_iterator it =
			_layers.rbegin(); it != _layers.rend(); it++)
	{
		boost::shared_ptr<Layer> layer = *it;
		boost::shared_ptr<Layer> prevLayer;
		if (_prevMap.find(layer->getName()) != _prevMap.end())
		{
			prevLayer = _prevMap[layer->getName()];
			for (size_t i = 0; i < prevLayer->neuronCount(); ++i)
			{
				boost::shared_ptr<Neuron> ni = (*prevLayer)[i];
				double error = 0.0;
				for (size_t j = 0; j < layer->neuronCount(); ++j)
				{
					boost::shared_ptr<Neuron> nj = (*layer)[j];
					error += nj->getErrorValue() * (*nj)[i];
				}
				ni->setErrorValue(error);
			}

		}

	}

}

void NeuralNet::backPropagationTraining(Layer *outputLayer)
{
	calculateErrorValues(outputLayer);
	updateWeightValues();
}

void NeuralNet::updateWeightValues()
{
	for (std::vector<boost::shared_ptr<Layer> >::iterator it = _layers.begin();
			it != _layers.end(); ++it)
	{
		boost::shared_ptr<Layer> layer = *it;
		for (size_t i = 0; i < layer->neuronCount(); ++i)
		{
			(*layer)[i]->updateWeights(_learningFactor,
					*(layer->activationFunction()));
		}
	}

	_numLearningSteps++;
	updateLearningFactor();
}

void NeuralNet::updateLearningFactor()
{
	_learningFactor = BASE_LEARNING_FACTOR / (1 + _numLearningSteps / R);
}

void NeuralNet::save(const std::string & filename)
{
	NetSerializer ns;
	const boost::shared_ptr<NeuralNet> netPtr(this);
	ns.saveToFile(netPtr, filename);
}

NeuralNet *NeuralNet::load(const std::string & filename)
{
	NetSerializer ns;
	return ns.loadFromFile(filename);
}

double NeuralNet::getPrediction(unsigned horizon)
{
	while (_resultBuffer.size() < horizon && !_resultBuffer.empty())
	{
		printSeq("rotating INPUT BUFFER (before): ", _inputBuffer, debug::Debug);
		std::rotate(_inputBuffer.begin(), _inputBuffer.begin() + 1,
				_inputBuffer.end());
		printSeq("rotating INPUT BUFFER (after): ", _inputBuffer, debug::Debug);
		_inputBuffer.back() = _resultBuffer.front();
		_layers[0]->insertInput(_inputBuffer);
	}
	double predVal = 0.0;
	if (_resultBuffer.size() >= horizon)
	{
		predVal = _resultBuffer[horizon - 1];
	}

	return predVal / _scale;
}

std::vector<double> NeuralNet::scaleVector(const std::vector<double> & vec) const
{
	std::vector<double> tmpVec;
	tmpVec.reserve(vec.size());

	BOOST_FOREACH( double val, vec )
	{
		tmpVec.push_back(val*_scale);
	}

	return tmpVec;
}

}
}
