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

#ifndef PREDICTIONCLIENT_H_
#define PREDICTIONCLIENT_H_

#include <parsedopts.h>

#include <dataprovider/dataprovider.h>
#include <modelbase.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <vector>

#include <comm/connection.h> // Must come before boost/serialization headers.
#include <comm/protocol.h>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

namespace prediction
{

namespace server
{

/// Downloads stock quote information from a server.
class PredictionServer
{
public:
    /// Constructor starts the asynchronous connect operation.
    PredictionServer(boost::asio::io_service& io_service,
            const ParsedOptions& opts);

    virtual ~PredictionServer()
    {
        std::cout << "~PredictionClient()" << std::endl;
    }



    /// Handle completion of a accept operation.
    void handle_accept(const boost::system::error_code& e,
            comm::connection_ptr conn);

    /// Handle completion of a read operation.
    void handle_read(const boost::system::error_code& e,
            comm::connection_ptr conn);

    void handle_write(const boost::system::error_code& e,
            comm::connection_ptr conn);

private:
    void interpretInputMessage(const comm::protocol::Message& msg);
    double getPrediction(size_t offset, size_t length, size_t horizon,
            size_t progress);
    void createPredictionModel(const std::string& algorithm);

private:
    boost::asio::ip::tcp::acceptor _acceptor;
    comm::protocol::Message _inBuffer;
    comm::protocol::Message _outBuffer;
    std::string _algorithm;
    bool _stopFlag;
    bool _predictionStarted;
    boost::shared_ptr<models::dataprovider::DataProvider> _dataProvider;
    boost::shared_ptr<models::AbstractModel> _predictionModel;
    const ParsedOptions& _opts;

    /// The data received from the server.
    //  std::vector<stock> stocks_;
};

}
}

#endif /* PREDICTIONCLIENT_H_ */
