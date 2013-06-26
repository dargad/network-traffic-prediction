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

#ifndef PREDICTIONSERVER_H_
#define PREDICTIONSERVER_H_

#include <modelproxy.h>
#include <parsedopts.h>
#include <neuralproxy.h>

#include <iostream>
#include <list>
#include <vector>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

// Must come before boost/serialization headers.
#include <comm/connection.h>
#include <comm/protocol.h>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

namespace prediction
{

namespace client
{

class PredictionClient
{
public:
    /// Constructor opens the acceptor and starts waiting for the first incoming
    /// connection.
    PredictionClient(boost::asio::io_service& io_service,
            const ParsedOptions& opts);

    virtual ~PredictionClient();

    /// Handle completion of a connect operation.
    void handle_connect(const boost::system::error_code& e,
            boost::asio::ip::tcp::resolver::iterator endpoint_iterator,
            comm::connection_ptr conn, unsigned buffnum);

    void handle_read(const boost::system::error_code& e, comm::connection_ptr conn, unsigned buffnum);

    void handle_write(const boost::system::error_code & e, comm::connection_ptr conn, unsigned buffnum);

    void resultObtained(const ResultInfo resultInfo);

private:
    void sendInitPacket(comm::connection_ptr conn, unsigned buffnum);

    void incrementActiveServerCount();
    void decrementActiveServerCount();
    int getActiveServerCount();

private:

    /// The acceptor object used to accept incoming socket connections.

    comm::protocol::Message _msg;
    std::list<ModelProxy*> _proxies;
    const ParsedOptions& _opts;
    size_t _modelsCount;
    boost::mutex _counterMutex;
    boost::mutex _waitMutex;
    NeuralProxy* _neuralProxy;

    comm::protocol::Message *_inBuffers;
    comm::protocol::Message *_outBuffers;

    int _activeServerCount;
    boost::mutex _activeServerGuard;
};

}
}

#endif /* PREDICTIONSERVER_H_ */
