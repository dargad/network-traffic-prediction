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

#ifndef NEURALNET_H_
#define NEURALNET_H_

#include <neural/layer.h>
#include <modelbase.h>

#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

namespace models
{
namespace neural
{

class NetSerializer;

class NeuralNet: public AbstractModel
{
    friend class NetSerializer;

    static const double BASE_LEARNING_FACTOR = 0.5;
    static const double R = 5000.0;


public:
    enum ModeOfOperation
    {
        Off, Training, Computation
    };

    NeuralNet();
    ~NeuralNet();

    void setLayers(const std::vector<boost::shared_ptr<Layer> >& layers);

    void provideInput(const std::vector<double>& inputValues, unsigned horizon);

    void trainingStep(const std::vector<double>& inputValues,
            std::vector<double>& expectedOutput);

    void stepDoneCallback(Layer *layer);

    double getPrediction(unsigned horizon);

    double getLearningFactor() const;
    void setLearningFactor(double learningFactor);

    void save(const std::string& filename);
    static NeuralNet * load(const std::string& filename);

    void setScale(double scale);
    double getScale() const;

private:
    void backPropagationTraining(Layer *outputLayer);
    void calculateErrorValues(Layer *outputLayer);
    void updateWeightValues();
    void updateLearningFactor();
    std::vector<double> scaleVector(const std::vector<double>& vec) const;

private:
    std::vector<boost::shared_ptr<Layer> > _layers;
    std::map<std::string, boost::shared_ptr<Layer> > _nextMap;
    std::map<std::string, boost::shared_ptr<Layer> > _prevMap;

    // expected output values (used for training)
    std::vector<double> _expectedOutput;
    ModeOfOperation _mode;

    long _numLearningSteps;
    double _learningFactor;
    std::vector<double> _resultBuffer;
    std::vector<double> _inputBuffer;
    double _scale;
};

inline
double NeuralNet::getLearningFactor() const
{
    return _learningFactor;
}

inline
void NeuralNet::setLearningFactor(double learningFactor)
{
    _learningFactor = learningFactor;
}

inline
double NeuralNet::getScale() const
{
    return _scale;
}

inline
void NeuralNet::setScale(double scale)
{
    _scale = scale;
}

}
}

#endif /* NEURALNET_H_ */
