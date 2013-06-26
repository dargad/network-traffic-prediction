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

#ifndef PARSEDOPTS_H_
#define PARSEDOPTS_H_

#include <util.h>

#include <ostream>
#include <string>
#include <vector>

#include <boost/foreach.hpp>

namespace prediction
{

namespace client
{

struct ServerData
{
    std::string ServerName;
    std::string Port;
};

struct ParsedOptions
{
    std::vector<ServerData> ModelServers;
    std::string Mode;
    std::string InputNet;
    std::string OutputNet;
    std::string DataFile;
    std::string ResultLog;
    unsigned DataOffset;
    unsigned DataLength;
    unsigned PredictionStep;
    unsigned NumberSteps;
    unsigned Horizon;
};

//namespace std
//{
inline std::ostream &operator<<(std::ostream &out, const prediction::client::ParsedOptions& opts)
{
//  out << "ModelServers: ";
//  BOOST_FOREACH(ServerData sd, opts.ModelServers)
//  {
//      out << sd.ServerName << ":" << sd.Port << ", ";
//  }
    out << std::endl;
    out << "Mode: " << opts.Mode << std::endl;
    out << "InputNet: " << opts.InputNet << std::endl;
    out << "OutputNet: " << opts.OutputNet << std::endl;
    out << "DataFile: " << opts.DataFile << std::endl;
    out << "ResultLog: " << opts.ResultLog << std::endl;
    out << "DataOffset: " << opts.DataOffset << std::endl;
    out << "DataLength: " << opts.DataLength << std::endl;
    out << "PredictionStep: " << opts.PredictionStep << std::endl;
    out << "NumberSteps: " << opts.NumberSteps << std::endl;
    out << "Horizon: " << opts.Horizon << std::endl;
    out << std::endl;
    return out;
}
//}

}
}

#endif /* PARSEDOPTS_H_ */
