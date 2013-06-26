/* * Copyright (c) 2010 Dariusz Gadomski <dgadomski@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef NEURON_H_
#define NEURON_H_

#include <neural/activationfunction.h>

#include <util.h>

#include <cstring>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>

namespace models
{
namespace neural
{

class Neuron
{
    friend class NetSerializer;

public:
    explicit Neuron(std::string parentLayerName) :
        _parentLayerName(parentLayerName), _numInputs(0), _buffer(0.0), _errorValue(0.0)    {};

    void setNumberInputs(size_t numInputs);
    size_t getNumberInputs() const;

    void setErrorValue(double errVal);
    double getErrorValue() const;

    double getOutput() const;

    double& operator[](size_t idx);

    void insertInput(const std::vector<double>& input, double bias, const ActivationFunction& af);

    void updateWeights(double learningFactor, const ActivationFunction& af);

protected:
    void fillWeights();

private:
    std::string _parentLayerName;
    size_t _numInputs;
    std::vector<double> _weights;
    std::vector<double> _lastWeightDeltas;
    std::vector<double> _inputValues;
    double _buffer;
    double _errorValue;
};

inline
void Neuron::setNumberInputs(size_t numInputs)
{
    _numInputs = numInputs;
    if (_weights.size() != _numInputs)
    {
//      debug::dbg() << "WEIGHT NUM DIFFERS: (numInputs: " << numInputs << ")" << std::endl;
//      debug::printSeq("WEIGHTS: ", _weights);
        fillWeights();
    }
}

inline size_t Neuron::getNumberInputs() const
{
    return _numInputs;
}

inline
double Neuron::getOutput() const
{
    return _buffer;
}

inline
void Neuron::setErrorValue(double errVal)
{
    _errorValue = errVal;
}

inline
double Neuron::getErrorValue() const
{
    return _errorValue;
}

}
}

#endif /* NEURON_H_ */
