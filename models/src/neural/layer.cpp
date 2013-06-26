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
 * layer.cpp
 *
 *  Created on: 2010-04-15
 *      Author: darek
 */

#include <neural/layer.h>

#include <neural/neuron.h>
#include <util.h>

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iterator>


namespace models
{
namespace neural
{
using namespace debug;

Layer::Layer(size_t num, std::string name) :
	_num(num), _name(name), _activationFunction(new SigmoidalAF), _bias(1.0)
//	_num(num), _name(name), _activationFunction(new ExponentialAF), _bias(1.0), _net(0)
//	_num(num), _name(name), _activationFunction(&linear), _activationFunctionDerivative(&der_linear), _bias(1.0), _net(0)
//	_num(num), _name(name), _activationFunction(&square), _activationFunctionDerivative(&der_square), _bias(100.0), _net(0)
{
	for(size_t i=0; i<_num; ++i)
	{
		boost::shared_ptr<Neuron> ptr(new Neuron(getName()));
		_neurons.push_back(ptr);
	}
}

Layer::~Layer()
{
//	dbg() << "~Layer() " << getName() << std::endl;
}

void Layer::insertInput(const std::vector<double>& input)
{
	_buffer.clear();

	_numInputs = input.size();

	for(std::vector<boost::shared_ptr<Neuron> >::iterator it = _neurons.begin();
			it != _neurons.end(); ++it)
	{
		boost::shared_ptr<Neuron> neuron = *it;
		if( neuron->getNumberInputs() == 0)
		{
			neuron->setNumberInputs(input.size());
		}
		neuron->insertInput(input, _bias, *_activationFunction);
		_buffer.push_back(neuron->getOutput());
	}

	printSeq("[" + _name + "] Buffers: ", _buffer);

	stepDone();
}

void Layer::stepDone()
{
	_callback(this);
}

std::vector<double> Layer::getOutput() const
{
	return _buffer;
}

boost::shared_ptr<Neuron> Layer::operator[](size_t idx) const
{
	if (idx < _num)
	{
		return _neurons[idx];
	}
	return boost::shared_ptr<Neuron>();
}

void Layer::setErrorValues(std::vector<double> errorValues)
{
	for(size_t i=0; i<errorValues.size(); ++i)
	{
		_neurons[i]->setErrorValue(errorValues[i]);
	}
}

std::vector<double> Layer::errorValues() const
{
	return _errors;
}

void Layer::setActivationFunction(boost::shared_ptr<ActivationFunction> af)
{
	_activationFunction = af;
}

void Layer::setActivationFunction(ActivationFunction *af)
{
	_activationFunction = boost::shared_ptr<ActivationFunction>(af);
}

boost::shared_ptr<ActivationFunction> Layer::activationFunction() const
{
	return _activationFunction;
}

void Layer::setCallback(boost::function<void (Layer*)> f)
{
	_callback = f;
}

}
}
