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

#ifndef MODELPROXY_H_
#define MODELPROXY_H_

#include <parsedopts.h>

#include <comm/connection.h>
#include <comm/protocol.h>

#include <boost/function.hpp>
#include <boost/thread.hpp>

namespace prediction
{

namespace client
{

struct ResultInfo
{
    ResultInfo(double result, unsigned modelId):
        Result(result), ModelId(modelId) {}

    double Result;
    unsigned ModelId;
};

class ModelProxy
{
public:
    ModelProxy(boost::asio::io_service& io_service, boost::asio::ip::tcp::resolver::iterator endpoint_iterator,
            const ParsedOptions& opts,
            ServerData sd, boost::function<void (const ResultInfo)> waitCallback);
    virtual ~ModelProxy();

    void operator()();
    void stop();
    void start();

    void handle_write(const boost::system::error_code& e);

    void handle_read(const boost::system::error_code& e);

    void handle_connect(const boost::system::error_code & e,
            boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

    void addResult(double result);
    double getResult();

    static unsigned getModelIndex(const std::string& algName);

private:
    void sendInitPacket();
    void initConnection();

private:
    static const char* MODELS[];

    bool _running;
    comm::connection _connection;
    boost::thread *_thread;

    comm::protocol::Message _inBuffer;
    comm::protocol::Message _outBuffer;

    const ParsedOptions& _opts;

    boost::function<void (const ResultInfo)> _resultCallback;

    std::list<double> _resultBuffer;

    boost::mutex _bufferMutex;

    unsigned _modelIndex;
    unsigned _progress;
    unsigned _dataLength;

    boost::asio::io_service& _io_service;

    ServerData _serverData;

    boost::asio::ip::tcp::resolver::iterator _endpoint_iterator;

};

}

}

#endif /* MODELPROXY_H_ */
