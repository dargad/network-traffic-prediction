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

#include <predictionclient.h>
#include <parsedopts.h>

#include <comm/protocol.h>
#include <util.h>

#include <boost/foreach.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>

namespace prediction
{

namespace client
{

using namespace comm;
using namespace debug;

PredictionClient::PredictionClient(boost::asio::io_service& io_service,
        const ParsedOptions& opts) :
    //  _acceptor(io_service, boost::asio::ip::tcp::endpoint(
            //          boost::asio::ip::tcp::v4(), opts.SourcePort)),
            _opts(opts), _modelsCount(0), _neuralProxy(new NeuralProxy(opts)),
            _activeServerCount(0)
{
//  _inBuffers.resize(_opts.ModelServers.size());
//  _outBuffers.resize(_opts.ModelServers.size());
    _inBuffers = new comm::protocol::Message[_opts.ModelServers.size()];
    _outBuffers = new comm::protocol::Message[_opts.ModelServers.size()];

    unsigned buffnum = 0;
BOOST_FOREACH(ServerData sd, _opts.ModelServers)
{
    boost::asio::ip::tcp::resolver resolver(io_service);
    dbg() << "Resolving " << sd.ServerName << ":" << sd.Port << std::endl;
    boost::asio::ip::tcp::resolver::query query(sd.ServerName, sd.Port);
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator =
    resolver.resolve(query);
    boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;

    connection_ptr conn(new connection(io_service));

    conn->socket().async_connect(endpoint,
            boost::bind(&PredictionClient::handle_connect, this,
                    boost::asio::placeholders::error, ++endpoint_iterator, conn, buffnum));
    ++buffnum;
}

}

void PredictionClient::sendInitPacket(connection_ptr conn, unsigned buffnum)
{
dbg() << "Sending init packet: " << std::endl;
//  _outBuffer = comm::protocol::Message();
_outBuffers[buffnum].DataOffset = _opts.DataOffset;
_outBuffers[buffnum].DataLength = _opts.DataLength;
_outBuffers[buffnum].Horizon = _opts.Horizon;

conn->async_write(_outBuffers[buffnum], boost::bind(&PredictionClient::handle_write,
                this, boost::asio::placeholders::error, conn, buffnum));

dbg() << "After async write" << std::endl;
}

/// Handle completion of a connect operation.
void PredictionClient::handle_connect(const boost::system::error_code& e,
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator, connection_ptr conn,
    unsigned buffnum)
{
if (!e)
{
    dbg(debug::Informational) << "PredictionClient::handle_connect() [" << buffnum << "]" << std::endl;
    // Successfully established connection. Start operation to read the list
    // of stocks. The connection::async_read() function will automatically
    // decode the data that is read from the underlying socket.
    //  conn->async_read(stocks_,
    //          boost::bind(&PredictionClient::handle_read, this,
    //                  boost::asio::placeholders::error));
    sendInitPacket(conn, buffnum);
    incrementActiveServerCount();
}
else if (endpoint_iterator != boost::asio::ip::tcp::resolver::iterator())
{
    // Try the next endpoint.
    conn->socket().close();
    boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
    conn->socket().async_connect(endpoint,
            boost::bind(&PredictionClient::handle_connect, this,
                    boost::asio::placeholders::error, ++endpoint_iterator, conn, buffnum));
}
else
{
    // An error occurred. Log it and return. Since we are not starting a new
    // operation the io_service will run out of work to do and the client will
    // exit.
    dbg(debug::High) << "PredictionClient::handle_connect(): " << e.message() << std::endl;
}
}

/// Handle completion of a read operation.
void PredictionClient::handle_read(const boost::system::error_code& e, connection_ptr conn,
    unsigned buffnum)
{
if (!e)
{
    size_t dataSize = _neuralProxy->getTestDataSize();

    dbg(debug::Informational) << "Received " << buffnum << ": " <<  conn->socket().remote_endpoint().address() << ":"
    << conn->socket().remote_endpoint().port() << std::endl;
    dbg(debug::Informational) << _inBuffers[buffnum] << std::endl;

    _neuralProxy->insertInput(_inBuffers[buffnum].Result,
            ModelProxy::getModelIndex(_inBuffers[buffnum].Algorithm));

    if( _outBuffers[buffnum].DataOffset + _opts.PredictionStep + _opts.Horizon + _opts.DataLength < dataSize)
    {
        dbg(debug::Informational) << "Sending prediction request: " << buffnum << std::endl;
        dbg(debug::Informational) << _outBuffers[buffnum] << std::endl;

        _outBuffers[buffnum].DataOffset += _opts.PredictionStep;
        conn->async_write(_outBuffers[buffnum], boost::bind(&PredictionClient::handle_write,
                        this, boost::asio::placeholders::error, conn, buffnum));
    }
    else
    {
        decrementActiveServerCount();

        if( getActiveServerCount() == 0 )
        {
            dbg(debug::Informational) << "Waiting for neural proxy..." << std::endl;
            _neuralProxy->join();
            dbg(debug::Informational) << "Joined with neural proxy." << std::endl;
        }
    }

}
else
{
    // An error occurred.
    dbg(debug::High) << "PredictionClient::handle_read(): " << e.message() << std::endl;
}

// Since we are not starting a new operation the io_service will run out of
// work to do and the client will exit.
}

void PredictionClient::handle_write(const boost::system::error_code & e, connection_ptr conn,
    unsigned buffnum)
{
if (!e)
{
    dbg() << "Sent " << buffnum << ": " << conn->socket().remote_endpoint().address() << ":"
    << conn->socket().remote_endpoint().port() << std::endl;
    dbg() << _outBuffers[buffnum] << std::endl;
    conn->async_read(_inBuffers[buffnum], boost::bind(
                    &PredictionClient::handle_read, this,
                    boost::asio::placeholders::error, conn, buffnum));
}
else
{
    // An error occurred.
    dbg(debug::High) << "PredictionClient::handle_write(): " << e.message() << std::endl;
}
}

PredictionClient::~PredictionClient()
{
}

void PredictionClient::resultObtained(const ResultInfo resultInfo)
{
dbg() << "Received result " << resultInfo.Result << " from model "
<< resultInfo.ModelId << std::endl;
_neuralProxy->insertInput(resultInfo.Result, resultInfo.ModelId);
}

void PredictionClient::incrementActiveServerCount()
{
    boost::unique_lock<boost::mutex> lock(_activeServerGuard);
    ++_activeServerCount;
}

void PredictionClient::decrementActiveServerCount()
{
    boost::unique_lock<boost::mutex> lock(_activeServerGuard);
    --_activeServerCount;
}

int PredictionClient::getActiveServerCount()
{
    boost::unique_lock<boost::mutex> lock(_activeServerGuard);
    return _activeServerCount;
}

}
}
