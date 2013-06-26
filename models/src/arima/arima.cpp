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

#include <arima/arima.h>

#include <util.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

#include <boost/thread.hpp>

namespace models
{

namespace arima
{

void runR(char * filename, std::string *output);
void writeFileForR(const std::vector<double>& input,
        const std::vector<int>& order, char *filename, unsigned horizon);

using namespace debug;
using namespace std;

const char* Arima::FILE_TEMPLATE = "/tmp/arima.XXXXXX";

Arima::Arima() :
    _horizon(0)
{
    dbg(debug::Informational) << "Arima()" << std::endl;
    fillOrder();
    if (!mkfifo(Arima::FILE_TEMPLATE, 0666))
    {
        dbg(debug::High) << "Error creating named pipe" << std::endl;
    }
    strcpy(_tempFilename, FILE_TEMPLATE);
    dbg(debug::Informational) << "Arima() - END" << std::endl;
}

Arima::~Arima()
{
    unlink(_tempFilename);
}

void Arima::fillOrder()
{
    double order[] =
    { 1, 1, 1 };
    unsigned numElem = sizeof(order) / sizeof(order[0]);

    _order.resize(numElem);
    copy(order, order + numElem, _order.begin());
}

int counter = 0;

double Arima::getPrediction(unsigned horizon)
{
    if (_outputBuff.size() < horizon)
    {
        dbg(debug::Informational) << "_outbuf.size(): " << _outputBuff.size()
                << std::endl;
        dbg(debug::Informational) << "COUNTER Prediciton: " << ++counter
                << std::endl;
        preparePrediction(_tempFilename, horizon);
        parseOutputForPrediction(_buff);
    }

    return _outputBuff[horizon - 1];
}

std::vector<int> Arima::getOrder() const
{
    return _order;
}

void Arima::setOrder(const std::vector<int> & order)
{
    if (order != _order)
    {
        _order = order;
        _outputBuff.clear();
    }
}

void Arima::preparePrediction(char * filename, unsigned horizon)
{
    dbg(debug::Informational) << "preparePrediction" << std::endl;
    boost::thread_group tg;
    tg.add_thread(new boost::thread(runR, filename, &_buff));
    tg.add_thread(new boost::thread(writeFileForR, _input, _order, filename,
            _horizon));

    tg.join_all();
    dbg(debug::Informational) << "preparePrediction - END" << std::endl;
}

void Arima::provideInput(const std::vector<double>& input, unsigned horizon)
{
    dbg(debug::Informational) << "provideInput: " << "(, horizon: " << horizon
            << ")" << std::endl;
    _input = input;
    _outputBuff.clear();
    _outputBuff.reserve(horizon);
    _horizon = horizon;
}

void Arima::parseOutputForPrediction(const std::string & output)
{
    dbg(debug::Informational) << "parseOutputForPrediction" << std::endl;
    //  dbg() << "R output: " << output << std::endl;
    std::istringstream input(output);

    std::string line;
    while (!std::getline(input, line).eof())
    {
//      dbg(debug::Informational) << "line: " << lineCount++ << std::endl;
//      dbg(debug::Informational) << "line: '" << line << "'" << std::endl;
        size_t pos = line.find(" ", std::log10(_horizon));
        if (pos != std::string::npos)
        {
            std::string tmp(line.substr(pos));
//          std::remove(tmp.begin(), tmp.end(), ' ');
            std::istringstream iss(tmp);
            while (!iss.eof())
            {
                double result = 0.0;
                iss >> result;
                _outputBuff.push_back(result);
//              dbg(debug::Informational) << "result: " << result << std::endl;
            }
        }
    }
    dbg(debug::Informational) << "_outputBuff.size(): " << _outputBuff.size() << std::endl;
    dbg(debug::Informational) << "parseOutputForPrediction - END" << std::endl;
}

void runR(char * filename, std::string *buff)
{
    dbg(debug::Informational) << "runR" << std::endl;
    std::string command = "R --slave -q --vanilla < ";
    command += filename;
    FILE * f = popen(command.c_str(), "r");

    std::ostringstream externOutput;

    int character = 0;
    while ((character = fgetc(f)) != EOF)
    {
        externOutput << (char) character;
    }

    buff->clear();
    buff->append(externOutput.str());
    dbg(debug::Informational) << "runR - END" << std::endl;
}

void writeFileForR(const std::vector<double>& input,
        const std::vector<int>& order, char *filename, unsigned horizon)
{
    dbg(debug::Informational) << "writeFileForR" << std::endl;
    std::ostringstream outstream;

    // set the time series
    outstream << "x <- c(";
    for (unsigned i = 0; i < input.size(); ++i)
    {
        outstream << input[i];
        if (i < input.size() - 1)
        {
            outstream << ", ";
        }
    }
    outstream << ")" << std::endl;
    // set order of ARIMA - p, d, q
    outstream << "o <- c(";
    for (unsigned i = 0; i < order.size(); ++i)
    {
        outstream << order[i];
        if (i < order.size() - 1)
        {
            outstream << ", ";
        }
    }
    outstream << ")" << std::endl;

    // run arima0
    outstream << "fit <- arima( x, o )" << std::endl;

    // run prediction
    outstream << "pre = predict(fit, n.ahead=" << horizon << ",se.fit=FALSE)"
            << std::endl;
    //  outstream << "tail(pre,1)" << std::endl;
    outstream << "pre[1:" << horizon << "]" << std::endl;

    ofstream outFile(filename);

    outFile << outstream.str();

    outFile.close();
    dbg(debug::Informational) << "writeFileForR - END" << std::endl;
}

}
}

