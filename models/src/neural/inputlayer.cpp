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

#include <neural/inputlayer.h>

#include <neural/neuron.h>
#include <util.h>

#include <algorithm>
#include <iostream>
#include <iterator>

#include <boost/shared_ptr.hpp>

namespace models
{
namespace neural
{

using namespace debug;

InputLayer::InputLayer(int num) :
    Layer(num, "INPUT")
{
    setActivationFunction(new LinearAF);
}

void InputLayer::insertInput(const std::vector<double> & input)
{
    std::vector<double> inputBuffer;

    if (input.size() > neuronCount())
    {
        std::copy(input.begin() + input.size() - neuronCount(), input.end(),
                std::back_inserter(inputBuffer));
    }
    else
    {
        std::copy(input.begin(), input.end(), std::back_inserter(inputBuffer));
    }

    _buffer.clear();

//  for (std::vector<boost::shared_ptr<Neuron> >::iterator it = _neurons.begin(); it
//          != _neurons.end(); ++it)
    for(unsigned i=0; i<neuronCount(); ++i)
    {
        boost::shared_ptr<Neuron> neuron = _neurons[i];

        std::vector<double> tmpInput;
        tmpInput.push_back(inputBuffer[i]);

        neuron->insertInput(tmpInput, _bias, *_activationFunction);
        _buffer.push_back(neuron->getOutput());
    }

    printSeq("Buffers in " + _name + ": ", _buffer);

    stepDone();
}

//void InputLayer::insertInput(const std::vector<double> & input)
//{
//  _buffer.clear();
//
//  if (input.size() > neuronCount())
//  {
//      std::copy(input.begin()+input.size()-neuronCount(), input.end(), std::back_inserter(_buffer));
//  }
//  else
//  {
//      std::copy(input.begin(), input.end(), _buffer.begin());
//  }
//
//  stepDone();
//}

}
}
