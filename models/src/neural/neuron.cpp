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

#include <neural/neuron.h>

#include <neural/layer.h>
#include <util.h>

#include <cstdlib>
#include <ctime>

namespace models
{
namespace neural
{
using namespace debug;

double& Neuron::operator [](size_t idx)
{
    if (idx >= _weights.size())
    {
        _weights.resize(idx + 1);
        _numInputs = _weights.size();
    }
    return _weights[idx];
}

void Neuron::insertInput(const std::vector<double> & input, double bias,
        const ActivationFunction& af)
{
    _inputValues = input;

    dbg() << "[" + _parentLayerName + "] ";

    double bufValue = 0.0;
    for (size_t i = 0; i < _weights.size(); ++i)
    {
        double val = input[i];
        // sum all products of input values and weights
        bufValue += val * _weights[i];
    }

    dbg() << bufValue << " --> ";
    // consider the activation function
    bufValue -= bias;
    dbg() << bufValue << " --> ";
    bufValue = af(bufValue);
    dbg() << bufValue << std::endl;
    _buffer = bufValue;
}

void Neuron::updateWeights(double learningFactor, const ActivationFunction& af)
{
    //  if (_parentLayerName == std::string("OUTPUT"))
    //  {
    //      dbg(debug::Error) << "OUPUT _inputValues.size(): "
    //              << _inputValues.size() << std::endl;
    //  }
    for (size_t i = 0; i < _weights.size(); ++i)
    {
        double e = _inputValues[i];
        double delta = learningFactor * getErrorValue() * e * af.derivative(e)
                + ALPHA * _lastWeightDeltas[i];
        dbg() << "[ " << _parentLayerName << " ] weight no. " << i << std::endl;
        dbg() << "learningFactor: " << learningFactor << std::endl;
        dbg() << "getErrorValue: " << getErrorValue() << std::endl;
        dbg() << "e: " << e << std::endl;
        dbg() << "af.derivative(e): " << af.derivative(e) << std::endl;
        dbg() << "ALPHA * _lastWeightDeltas[i]: " << ALPHA
                * _lastWeightDeltas[i] << std::endl;
        dbg() << "Old weight: " << _weights[i] << std::endl;
        dbg() << "Delta in neuron: " << delta << std::endl;
        _weights[i] += delta;
        dbg() << "New weight: " << _weights[i] << std::endl;
        dbg() << std::endl;
        _lastWeightDeltas[i] = delta;
    }

    //  printSeq("[" + _parentLayer->getName() + "] " + "Weights in neuron: ", _weights);
}

void Neuron::fillWeights()
{
    _weights.clear();
    std::srand(std::time(0) * reinterpret_cast<unsigned long> (this));
    for (size_t i = 0; i < getNumberInputs(); ++i)
    {
        double r =
                ((double) std::rand() / ((double) (RAND_MAX) + (double) (1))) * 0.3;
        _weights.push_back(r);
        _lastWeightDeltas.push_back(0.0);
    }

    printSeq("[" + _parentLayerName + "] " + "Weights in neuron: ", _weights);
}

}
}
