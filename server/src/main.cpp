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
#include <parsedopts.h>

#include <util.h>

#include <algorithm>
#include <iostream>
#include <boost/program_options.hpp>

using namespace std;
using namespace prediction::server;

namespace po = boost::program_options;

const unsigned DEFAULT_SERVER_PORT = 4421;
const std::string DEFAULT_SERVER_ADDRESS = "localhost";

const char* ALLOWED_ALGORITHMS[] =
{ "arima", "chaos", "grey", "neural" };

ParsedOptions parseOptions(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	try
	{
		ParsedOptions opts(parseOptions(argc, argv));
		debug::dbg(debug::Informational) << "Starting server with opts:"
				<< opts;
		boost::asio::io_service io_service;
		PredictionServer ps(io_service, opts);
		io_service.run();
	} catch (exception& e)
	{
		dbg(debug::Highest) << e.what() << std::endl;
	}
}

ParsedOptions parseOptions(int argc, char *argv[])
{
	po::options_description desc("Client options");
	desc.add_options()("help,h", "produce help message")

	("listen-port,l",
			po::value<unsigned>()->default_value(DEFAULT_SERVER_PORT),
			"set server port for connection "
				"(or service name e.g. http)")

	("algorithm,a", po::value<std::string>(),
			"set prediction algorithm: arima, grey, neural")

	("input-file,i", po::value<std::string>(), "set path to the input file")

	("debug-level,d",
			po::value<unsigned>()->default_value(debug::Informational),
			"set debug level (0-4)");

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		cout << desc << endl;
		exit(1);
	}

	ParsedOptions opts;
	if (vm.count("listen-port"))
	{
		opts.ListenPort = vm["listen-port"].as<unsigned> ();
	}

	if (vm.count("debug-level"))
	{
		unsigned val = vm["debug-level"].as<unsigned> ();
		if (val >= debug::Debug && val <= debug::Highest)
		{
			debug::setVerbosity(static_cast<debug::DebugLevel> (val));
		}
	}

	if (vm.count("input-file"))
	{
		opts.InputFile = vm["input-file"].as<std::string> ();
	}
	else
	{
		dbg(debug::Highest) << "Input file not provided. Exiting." << endl;
		exit(1);
	}

	if (!vm.count("algorithm"))
	{
		dbg(debug::Highest) << "Prediction algorithm not provided. Exiting."
				<< endl;
		exit(1);
	}
	else
	{
		vector<std::string> allowedAlgorithms;
		size_t numAlgs = sizeof(ALLOWED_ALGORITHMS)
				/ sizeof(ALLOWED_ALGORITHMS[0]);
		allowedAlgorithms.resize(numAlgs);
		copy(ALLOWED_ALGORITHMS, ALLOWED_ALGORITHMS + numAlgs,
				allowedAlgorithms.begin());

		std::string algName = vm["algorithm"].as<std::string> ();

		vector<std::string>::iterator it = std::find(allowedAlgorithms.begin(),
				allowedAlgorithms.end(), algName);

		if (it != allowedAlgorithms.end())
		{
			opts.Algorithm = algName;
		}
		else
		{
			dbg(debug::Highest)
					<< "Incorrect prediction algorithm provided. Exiting."
					<< endl;
			exit(1);
		}
	}

	return opts;
}
