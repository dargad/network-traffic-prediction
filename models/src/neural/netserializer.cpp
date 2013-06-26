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

#include <neural/netserializer.h>

#include <neural/layer.h>
#include <neural/inputlayer.h>
#include <neural/outputlayer.h>
#include <util.h>

#include <fstream>
#include <sstream>

namespace models
{

namespace neural
{

using namespace std;
using namespace debug;

NetSerializer::NetSerializer()
{
}

NetSerializer::~NetSerializer()
{
}

void NetSerializer::saveToFile(const boost::shared_ptr<NeuralNet> & net,
        const string & filename)
{
    ostringstream oss;

    oss << "Learning factor: " << net->getLearningFactor() << endl;

    for (size_t i = 0; i < net->_layers.size(); ++i)
    {
        boost::shared_ptr<Layer> layer = net->_layers[i];
        oss << "Layer" << endl;
        oss << "Type: " << layer->getType() << endl;
        oss << "Name: " << layer->getName() << endl;
        oss << "Bias: " << layer->bias() << endl;
        //      dbg() << "ActivationFunction::SAVE: "
        //              << layer->activationFunction()->getActivationFunctionType()
        //              << endl;
        oss << "ActivationFunction: "
                << layer->activationFunction()->getActivationFunctionType()
                << endl;

        oss << "Neurons: " << layer->neuronCount() << endl;
        for (size_t n = 0; n < layer->neuronCount(); ++n)
        {
            boost::shared_ptr<Neuron> neuron = (*layer)[n];

            for (size_t j = 0; j < neuron->getNumberInputs(); ++j)
            {
                oss << (*neuron)[j] << "\t";
            }
            oss << endl;
            for (size_t j = 0; j < neuron->getNumberInputs(); ++j)
            {
                oss << neuron->_lastWeightDeltas[j] << "\t";
            }
            oss << endl;
        }
    }

    ofstream outfile(filename.c_str());
    string buff(oss.str());
    outfile << buff;

    //  dbg() << "Output: " << endl;
    //  dbg() << buff << endl;
    outfile.close();
}

NeuralNet * NetSerializer::loadFromFile(const string& filename)
{
    ifstream infile(filename.c_str());

    string line;

    NeuralNet *net = new NeuralNet;
    vector<boost::shared_ptr<Layer> > layers;

    boost::shared_ptr<Layer> tmpLayer;

    double bias;
    string layerName;
    size_t numNeurons;
    Layer::LayerType layerType(Layer::Regular);
    ActivationFunction *af;

    while (!getline(infile, line).eof())
    {
        if (line.find("Learning factor") != string::npos)
        {
            double val = 0.0;
            stringstream inbuf(getFieldValue(line));
            inbuf >> val;
            net->setLearningFactor(val);
        }

        if (line == "Layer")
        {
            if (tmpLayer != boost::shared_ptr<Layer>())
            {
                layers.push_back(tmpLayer);
            }
        }

        if (line.find("Type") != string::npos)
        {
            int val = 0;
            istringstream inbuf(getFieldValue(line));
            inbuf >> val;
            layerType = static_cast<Layer::LayerType> (val);
        }

        if (line.find("Name") != string::npos)
        {
            layerName = getFieldValue(line);
            //          dbg() << "'" << layerName << "'" << endl;
        }

        if (line.find("Bias") != string::npos)
        {
            istringstream inbuf(getFieldValue(line));
            inbuf >> bias;
        }

        if (line.find("ActivationFunction") != string::npos)
        {
            int val = 0;
            istringstream inbuf(getFieldValue(line));
            inbuf >> val;
            ActivationFunctionType afType =
                    static_cast<ActivationFunctionType> (val);
            //          dbg(debug::Error) << "line: " << line << "\t" << "afType: " << val << std::endl;

            af = ActivationFunction::getActivationFunctionByCode(afType);
        }

        if (line.find("Neurons") != string::npos)
        {
            //          std::dbg() << "num neurons found (line): " << line << std::endl;
            istringstream inbuf(getFieldValue(line));
            inbuf >> numNeurons;

            //          std::dbg() << "num neurons found: " << numNeurons << std::endl;

            tmpLayer = createNewLayer(layerType, numNeurons, layerName);
            tmpLayer->setBias(bias);
            tmpLayer->setActivationFunction(af);

            for (unsigned i = 0; i < numNeurons; ++i)
            {
                // read _weights
                getline(infile, line);
                // remove the ending tab before creating istringstream
                istringstream weights(line.substr(0, line.size() - 1));
                int weightNum = 0;
                while (weights.good())
                {
                    double val;
                    weights >> val;
                    (*(*tmpLayer)[i])[weightNum] = val;
                    //                  std::dbg() << "Weight: " << val << std::endl;
                    ++weightNum;
                }

                (*tmpLayer)[i]->setNumberInputs(weightNum);

                // read _lastWeightsDeltas
                getline(infile, line);
                // remove the ending tab before creating istringstream
                istringstream deltas(line.substr(0, line.size() - 1));
                int deltaNum = 0;
                std::vector<double> delta_vec;
                while (deltas.good())
                {
                    double val;
                    deltas >> val;
                    delta_vec.push_back(val);
                    ++deltaNum;
                }
                (*tmpLayer)[i]->_lastWeightDeltas.resize(delta_vec.size());
                copy(delta_vec.begin(), delta_vec.end(),
                        (*tmpLayer)[i]->_lastWeightDeltas.begin());
            }
        }

    }

    layers.push_back(tmpLayer);

    net->setLayers(layers);

    return net;
}

string NetSerializer::getFieldValue(const string & line)
{
    size_t pos = line.find_first_of(':');
    if (pos != string::npos)
    {
        return line.substr(pos + 2);
    }
    return string();
}

boost::shared_ptr<Layer> NetSerializer::createNewLayer(
        Layer::LayerType layerType, size_t numNeurons,
        const std::string& layerName)
{
    Layer *layer = 0;

    if (layerType == Layer::Regular)
    {
        layer = new Layer(numNeurons, layerName);
    }
    else if (layerType == Layer::Input)
    {
        layer = new InputLayer(numNeurons);
    }
    else if (layerType == Layer::Output)
    {
        layer = new OutputLayer;
    }

    return boost::shared_ptr<Layer>(layer);
}

}

}
