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

#include <modelproxy.h>

#include <util.h>

#include <algorithm>
#include <boost/date_time.hpp>

namespace prediction
{

namespace client
{

using namespace comm;
using namespace debug;

const unsigned SLEEP_TIME = 10;

const char* ModelProxy::MODELS[] =
{ "chaos", "grey", "neural" };

ModelProxy::ModelProxy(boost::asio::io_service& io_service,
        boost::asio::ip::tcp::resolver::iterator endpoint_iterator,
        const ParsedOptions& opts, ServerData sd,
        boost::function<void(const ResultInfo)> resultCallback) :
    _running(true), _connection(io_service), _opts(opts), _resultCallback(
            resultCallback), _progress(0), _dataLength(_opts.DataLength),
            _io_service(io_service), _serverData(sd), _endpoint_iterator(endpoint_iterator)
{
    dbg() << "ModelProxy()" << std::endl;
}

ModelProxy::~ModelProxy()
{
    _connection.socket().close();
    dbg() << "~ModelProxy()" << std::endl;
}

void ModelProxy::start()
{
    dbg() << "ModelProxy::start()" << std::endl;
    _thread = new boost::thread(boost::ref(*this));
}

void ModelProxy::stop()
{
    _running = false;
}

void ModelProxy::initConnection()
{
    try
    {
    dbg() << "ModelProxy::initConnection()" << std::endl;
    // Resolve the host name into an IP address.

    boost::asio::ip::tcp::endpoint endpoint = *_endpoint_iterator;

    dbg() << "ModelProxy::initConnection() -- 2" << std::endl;

    dbg() << "Async connect to " << _serverData.ServerName << ":" << _serverData.Port << std::endl;
    _connection.socket().async_connect(endpoint, boost::bind(
            &ModelProxy::handle_connect, this,
            boost::asio::placeholders::error, ++_endpoint_iterator));
    std::cout << "ModelProxy::initConnection() -- END" << std::endl;
    }
    catch( std::exception& e )
    {
        dbg(debug::High) << e.what() << std::endl;
    }
}

void ModelProxy::operator ()()
{
    initConnection();

    dbg() << "Before running..." << std::endl;
    while (_running)
    {
        dbg() << "Thread working..." << std::endl;
        boost::this_thread::sleep(boost::posix_time::seconds(SLEEP_TIME));
    }
    dbg() << "After running..." << std::endl;
}

void ModelProxy::handle_connect(const boost::system::error_code & e,
        boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
{
    dbg() << "PredictionClient::handle_connect()" << std::endl;
    if (!e)
    {
        sendInitPacket();
    }
    else if (endpoint_iterator != boost::asio::ip::tcp::resolver::iterator())
    {
        dbg() << "PredictionClient::handle_connect() - else if" << std::endl;
        // Try the next endpoint.
        _connection.socket().close();
        boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
        _connection.socket().async_connect(endpoint, boost::bind(
                &ModelProxy::handle_connect, this,
                boost::asio::placeholders::error, ++endpoint_iterator));
    }
    else
    {
        dbg(debug::High) << "PredictionClient::handle_connect(): "
                << e.message() << std::endl;
    }
}

void ModelProxy::handle_write(const boost::system::error_code & e)
{
    if (!e)
    {
        dbg() << "Sent: " << _connection.socket().remote_endpoint().address() << ":"
                << _connection.socket().remote_endpoint().port() << std::endl;
        dbg() << _outBuffer << std::endl;
        _connection.async_read(_inBuffer, boost::bind(
                &ModelProxy::handle_read, this,
                boost::asio::placeholders::error));
    }
    else
    {
        // An error occurred.
        dbg(debug::High) << e.message() << std::endl;
    }
}

void ModelProxy::handle_read(const boost::system::error_code& e)
{
    if (!e)
    {
        dbg() << "Received From: "
                << _connection.socket().remote_endpoint().address() << ":"
                << _connection.socket().remote_endpoint().port() << std::endl;
        dbg() << _inBuffer << std::endl;

        _modelIndex = getModelIndex(_inBuffer.Algorithm);

        addResult(_inBuffer.Result);

        _connection.async_write(_outBuffer, boost::bind(
                &ModelProxy::handle_write, this,
                boost::asio::placeholders::error));
    }
    else
    {
        // An error occurred.
        dbg(debug::High) << e.message() << std::endl;
    }
}

void ModelProxy::addResult(double result)
{
    boost::unique_lock<boost::mutex> lock(_bufferMutex);
    _resultBuffer.push_back(result);
}

double ModelProxy::getResult()
{
    boost::unique_lock<boost::mutex> lock(_bufferMutex);
    double result = _resultBuffer.front();
    _resultBuffer.pop_front();
    return result;
}

unsigned ModelProxy::getModelIndex(const std::string& algName)
{
    size_t elemCount = 3;
    for (size_t i = 0; i < elemCount; ++i)
    {
        if (algName == MODELS[i])
        {
            return i;
        }
    }
    return -1;
}

void ModelProxy::sendInitPacket()
{
    dbg() << "Sending init packet: " << std::endl;

    dbg() << "Opts:" << std::endl;
    dbg() << _opts << std::endl;

    //  _outBuffer = comm::protocol::Message();
    _outBuffer.DataOffset = _opts.DataOffset;
    _outBuffer.DataLength = _opts.DataLength;
    _outBuffer.Horizon = _opts.Horizon;

    dbg() << _outBuffer << std::endl;

    _connection.async_write(_outBuffer, boost::bind(&ModelProxy::handle_write,
            this, boost::asio::placeholders::error));

    dbg() << "After async write" << std::endl;
}

}

}
