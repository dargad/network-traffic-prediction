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

#ifndef NEURALPROXY_H_
#define NEURALPROXY_H_

#include <neural/neuralnet.h>
#include <dataprovider/dataprovider.h>
#include <parsedopts.h>
#include <outputwriter.h>

#include <boost/thread.hpp>

namespace prediction
{

namespace client
{

class NeuralProxy
{
public:
    NeuralProxy(const ParsedOptions& opts);
    virtual ~NeuralProxy();

    void insertInput(double input, int modelIdx);

    void operator()();

    unsigned getTestDataSize();

    void join();

private:
    void initBuffer();
    bool hasInput() const;
    void loadNet();
    std::vector<double> getInput();
    void createResultLog();
    void compareResults(double& prediction, double expected,
            const std::vector<double>& inputs);

private:
    std::string _inputFilePath;
    models::neural::NeuralNet::ModeOfOperation _mode;
    std::vector<std::list<double> > _buffer;
    std::list<double> _outputBuffer;
    bool _running;
    boost::condition_variable _inputAvailCond;
    boost::mutex _inputBufferMutex;
    boost::thread _thread;
    models::neural::NeuralNet* _neuralNet;
    const ParsedOptions& _opts;
    boost::shared_ptr<models::dataprovider::DataProvider> _dataProvider;
    boost::shared_ptr<OutputWriter> _outputWriter;
    bool _writeOutput;

};

inline
unsigned NeuralProxy::getTestDataSize()
{
    if( _dataProvider != boost::shared_ptr<models::dataprovider::DataProvider>())
    {
        return _dataProvider->getDataSize();
    }
    return -1;
}

}

}

#endif /* NEURALPROXY_H_ */
