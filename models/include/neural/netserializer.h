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

#ifndef NETSERIALIZER_H_
#define NETSERIALIZER_H_

#include <neural/neuralnet.h>

#include <neural/layer.h>

#include <string>

#include <boost/shared_ptr.hpp>

namespace models
{

namespace neural
{

class NetSerializer
{
public:
    NetSerializer();
    virtual ~NetSerializer();

    void saveToFile(const boost::shared_ptr<NeuralNet>& net, const std::string& filename);
    NeuralNet * loadFromFile(const std::string& filename);

private:
    std::string getFieldValue(const std::string & line);
    boost::shared_ptr<Layer> createNewLayer(Layer::LayerType layerType, size_t numNeurons, const std::string& layerName);
};

}

}

#endif /* NETSERIALIZER_H_ */
