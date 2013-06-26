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

#include <predictionserver.h>

#include <comm/protocol.h>
#include <arima/arima.h>
#include <chaos/chaos.h>
#include <grey/grey.h>
#include <neural/neuralnet.h>
#include <util.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

// Must come before boost/serialization headers.
#include <comm/connection.h>
#include <comm/protocol.h>

namespace prediction
{

namespace server
{

using namespace models::dataprovider;
using namespace comm;
using namespace debug;

PredictionServer::PredictionServer(boost::asio::io_service & io_service,
        const ParsedOptions& opts) :
    _acceptor(io_service, boost::asio::ip::tcp::endpoint(
            boost::asio::ip::tcp::v4(), opts.ListenPort)), _algorithm(
            opts.Algorithm), _dataProvider(new DataProvider(opts.InputFile)),
            _opts(opts)
//  _connection(io_service), _algorithm(opts.Algorithm), _stopFlag(false),
//          _predictionStarted(false), _opts(opts)
{
    connection_ptr new_conn(new connection(_acceptor.get_io_service()));
    _acceptor.async_accept(new_conn->socket(), boost::bind(
            &PredictionServer::handle_accept, this,
            boost::asio::placeholders::error, new_conn));

    createPredictionModel(_algorithm);

}

void PredictionServer::handle_read(const boost::system::error_code& e,
        connection_ptr conn)
{
    if (!e)
    {
        dbg(debug::Informational) << "Handle read: " << std::endl;
        dbg(debug::Informational) << _inBuffer << std::endl;

        unsigned dataStart = _inBuffer.DataOffset;
        unsigned dataLength = _inBuffer.DataLength;
        unsigned horizon = _inBuffer.Horizon;

        std::vector<double> inputBuffer(_dataProvider->getDataVector(dataStart,
                dataLength));

        _predictionModel->provideInput(inputBuffer, horizon);

        double prediction = _predictionModel->getPrediction(horizon);

//      if( _algorithm == std::string("neural") )
//      {
//          printSeq("!!! Input: ", inputBuffer, debug::High);
//          dbg(debug::High) << "!!! Prediction: " << prediction << std::endl;
//      }

        _outBuffer = _inBuffer;
        _outBuffer.Result = prediction;
        _outBuffer.Algorithm = _algorithm;
        _outBuffer.DataOffset = _inBuffer.DataOffset;
        _outBuffer.DataLength = _inBuffer.DataLength;

        conn->async_write(_outBuffer, boost::bind(
                &PredictionServer::handle_write, this,
                boost::asio::placeholders::error, conn));
    }
    else
    {
        dbg(debug::High) << e.message() << std::endl;
    }
}

void PredictionServer::handle_write(const boost::system::error_code& e,
        connection_ptr conn)
{
    if (!e)
    {
        dbg(debug::Informational) << "Handle write: " << std::endl;
        dbg(debug::Informational) << _outBuffer << std::endl;

        conn->async_read(_inBuffer, boost::bind(&PredictionServer::handle_read,
                        this, boost::asio::placeholders::error, conn));
    }
    else
    {
        dbg(debug::High) << e.message() << std::endl;
    }
}

void PredictionServer::handle_accept(const boost::system::error_code& e,
        connection_ptr conn)
{
    if (!e)
    {
        dbg() << "Accepted connection!" << std::endl;

        conn->async_read(_inBuffer, boost::bind(&PredictionServer::handle_read,
                this, boost::asio::placeholders::error, conn));

        connection_ptr new_conn(new connection(_acceptor.get_io_service()));
        _acceptor.async_accept(new_conn->socket(), boost::bind(
                &PredictionServer::handle_accept, this,
                boost::asio::placeholders::error, new_conn));
    }
    else
    {
        // An error occurred. Log it and return. Since we are not starting a new
        // accept operation the io_service will run out of work to do and the
        // server will exit.
        dbg(debug::High) << e.message() << std::endl;
    }
}

void PredictionServer::createPredictionModel(const std::string& algorithm)
{
    models::AbstractModel *model = 0;

    if (algorithm == "arima")
    {
        models::arima::Arima *arima = new models::arima::Arima();
        std::vector<int> order;
        order.push_back(1);
        order.push_back(2);
        order.push_back(1);
        arima->setOrder(order);
        model = arima;
    }
    else if (algorithm == "grey")
    {
        model = new models::grey::Grey();
    }
    else if (algorithm == "chaos")
    {
        model = new models::chaos::Chaos(3, 1);
    }
    else if (algorithm == "neural")
    {
        models::neural::NeuralNet *net = models::neural::NeuralNet::load("learning.net");
        net->setScale(1.0 / _dataProvider->getMaxValue());
        model = net;
    }

    if (model)
    {
        _predictionModel = boost::shared_ptr<models::AbstractModel>(model);
    }
}

double PredictionServer::getPrediction(size_t offset, size_t length,
        size_t horizon, size_t progress)
{
    std::vector<double> inputVec = _dataProvider->getDataVector(offset, length);

    if (progress == 0)
    {
        _predictionModel->provideInput(inputVec, horizon);
    }

    // get prediction - progress=0 -> prediction for t+1 etc.
    return _predictionModel->getPrediction(progress + 1);
}

}
}
