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

/*
 * main.cpp
 *
 *  Created on: 2010-05-04
 *      Author: darek
 */

#include <predictionclient.h>

#include <parsedopts.h>
#include <util.h>

#include <iostream>
#include <boost/program_options.hpp>

using namespace std;
using namespace prediction::client;
namespace po = boost::program_options;

const std::string DEFAULT_SERVER_PORT = "4421";
const unsigned DEFAULT_NUM_MODELS = 4;

ParsedOptions parseOptions(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	try
	{
		ParsedOptions opts(parseOptions(argc, argv));
		debug::dbg(debug::Informational) << "Starting server with opts:"
				<< opts;
		boost::asio::io_service io_service;
		PredictionClient ps(io_service, opts);
		io_service.run();
	} catch (exception& e)
	{
		dbg(debug::Highest) << e.what() << std::endl;
	}
}

ParsedOptions parseOptions(int argc, char *argv[])
{
	po::options_description desc("Server options");
	desc.add_options()("help,h", "produce help message")

	("servers,s", po::value<std::vector<std::string> >(),
			"instance of server info in form of <servername>:<port>")

	("mode,m", po::value<std::string>()->default_value("prediction"),
			"set server mode: training, prediction")

	("input-net,i", po::value<std::string>(),
			"set input file to read the neural net from")

	(
			"output-net,o",
			po::value<std::string>(),
			"set output file to save the neural net after training (only valid if mode=training)")

	("result-file,r", po::value<std::string>(),
			"set output file to log results")

	("data-file,f", po::value<std::string>(),
			"set file name of input historical data file")

	("data-offset", po::value<unsigned>()->default_value(0),
			"set offset of historical data in the input file")

	("data-length", po::value<unsigned>()->default_value(100),
			"set length of historical data in the input file")

	("prediction-step", po::value<unsigned>()->default_value(500),
			"set size of the prediction step (used to increment the data offset) after each prediciton")

	("num-steps,n", po::value<unsigned>()->default_value(14),
			"set number of steps to perform register prediction results")

	("horizon,H", po::value<unsigned>()->default_value(1),
			"set the forecast horizon")

	("debug-level,d",
			po::value<unsigned>()->default_value(debug::Informational),
			"set debug level (0-4)");

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		cout << desc << "\n";
		exit(1);
	}

	ParsedOptions opts;

	if (vm.count("servers"))
	{
		std::vector<ServerData> servers;
		std::vector<std::string> serversInpput = vm["servers"].as<std::vector<
				std::string> > ();

		BOOST_FOREACH (std::string server, serversInpput)
		{
			size_t pos =server.find(':');
			std::string serverName = "localhost";
			std::string port = DEFAULT_SERVER_PORT;

			if( pos != string::npos )
			{
				serverName =server.substr(0, pos);
				port = server.substr(pos+1);

			}
			else
			{
				serverName = server;
			}

			ServerData sd;
			sd.ServerName = serverName;
			sd.Port = port;
			servers.push_back(sd);
		}

		opts.ModelServers = servers;
	}

	if (vm.count("input-net"))
	{
		opts.InputNet = vm["input-net"].as<std::string> ();
	}

	if (vm.count("output-net"))
	{
		opts.OutputNet = vm["output-net"].as<std::string> ();
	}

	if (vm.count("result-file"))
	{
		opts.ResultLog = vm["result-file"].as<std::string> ();
	}

	if (vm.count("data-file"))
	{
		opts.DataFile = vm["data-file"].as<std::string> ();
	}
	else
	{
		dbg(debug::High) << "Data file not provided. Exiting." << std::endl;
		exit(1);
	}

	if (vm.count("data-offset"))
	{
		opts.DataOffset = vm["data-offset"].as<unsigned> ();
	}

	if (vm.count("data-length"))
	{
		opts.DataLength = vm["data-length"].as<unsigned> ();
	}

	if (vm.count("horizon"))
	{
		opts.Horizon = vm["horizon"].as<unsigned> ();
	}

	if( vm.count("prediction-step") )
	{
		opts.PredictionStep = vm["prediction-step"].as<unsigned>();
	}

	if( vm.count("num-steps") )
	{
		opts.NumberSteps = vm["num-steps"].as<unsigned>();
	}

	if (vm.count("mode"))
	{
		opts.Mode = vm["mode"].as<std::string> ();
	}

	if (vm.count("debug-level"))
	{
		unsigned val = vm["debug-level"].as<unsigned> ();
		if (val >= debug::Debug && val <= debug::Highest)
		{
			debug::setVerbosity(static_cast<debug::DebugLevel> (val));
		}
	}

	return opts;
}
