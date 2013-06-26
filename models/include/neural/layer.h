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

#ifndef LAYER_H_
#define LAYER_H_

#include <neural/neuron.h>

#include <neural/activationfunction.h>

#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/function.hpp>
#include <boost/foreach.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>

namespace models
{
namespace neural
{

class NetSerializer;

class Layer
{
    friend class NetSerializer;
public:
    enum LayerType
    {
        Regular, Input, Output
    };

    Layer(size_t numNeurons, std::string name);
    virtual ~Layer();

    double bias() const;
    void setBias(double bias);

    void setNumberInputs(size_t numInputs);
    size_t getNumberInputs() const;

    void setErrorValues(std::vector<double> errorValues);
    std::vector<double> errorValues() const;

    std::string getName() const;

    double getLearningFactor() const;

    virtual void
    setActivationFunction(boost::shared_ptr<ActivationFunction> af);
    virtual void setActivationFunction(ActivationFunction *af);
    virtual boost::shared_ptr<ActivationFunction> activationFunction() const;

    virtual void setCallback(boost::function<void(Layer *)> f);

    virtual void insertInput(const std::vector<double>& input);

    virtual std::vector<double> getOutput() const;

    virtual size_t neuronCount() const;

    virtual boost::shared_ptr<Neuron> operator[](size_t idx) const;

    virtual LayerType getType() const
    {
        return Regular;
    }

protected:
    virtual void stepDone();

protected:
    size_t _num;
    std::string _name;

    std::vector<boost::shared_ptr<Neuron> > _neurons;

    // buffer for the output values
    std::vector<double> _buffer;

    // values of errors for each neuron in this layer calculated
    // at last training step
    std::vector<double> _errors;

    // pointer to the activation function and its derivative
    boost::shared_ptr<ActivationFunction> _activationFunction;

    double _bias;
    boost::function<void(Layer *)> _callback;
    size_t _numInputs;
};

inline
double Layer::bias() const
{
    return _bias;
}

inline
void Layer::setBias(double bias)
{
    _bias = bias;
}

inline size_t Layer::neuronCount() const
{
    return _num;
}

inline std::string Layer::getName() const
{
    return _name;
}

inline
double Layer::getLearningFactor() const
{
    //  if (_net != 0)
    //  {
    //      return _net->getLearningFactor();
    //  }
    return 0.0;
}

inline
void Layer::setNumberInputs(size_t numInputs)
{
    _numInputs = numInputs;
    BOOST_FOREACH( boost::shared_ptr<Neuron> neuron, _neurons)
{   neuron->setNumberInputs(numInputs);
}
}

inline
size_t Layer::getNumberInputs() const
{
return _numInputs;
}

}
}

#endif /* LAYER_H_ */
