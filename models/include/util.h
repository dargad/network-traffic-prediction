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

#ifndef UTIL_H_
#define UTIL_H_

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <sstream>
#include <vector>

#include <sys/time.h>

namespace debug
{

enum DebugLevel
{
    Debug=0, Informational, Normal, High, Highest
};

extern DebugLevel _verbosity_;

inline
void setVerbosity(debug::DebugLevel level)
{
    _verbosity_ = level;
}

inline DebugLevel getVerbosity()
{
    return _verbosity_;
}

std::ostream& dbg(debug::DebugLevel level = debug::Debug);

template<typename T>
void printSeq(const std::string& comment, const std::vector<T>& seq,
        DebugLevel debugLevel = Debug, const std::string& separator = std::string(", "))
{
    dbg(debugLevel) << comment;
    std::copy(seq.begin(), seq.end(), std::ostream_iterator<T>(dbg(debugLevel),
            separator.c_str()));
    dbg(debugLevel) << std::endl;
}

template<typename T>
void printSeq(std::ostream& out, const std::string& comment, const std::vector<T>& seq,
        DebugLevel debugLevel = Debug, const std::string& separator = std::string(", "))
{
    out << comment;
    std::copy(seq.begin(), seq.end(), std::ostream_iterator<T>(out,
            separator.c_str()));
    out << std::endl;
}

int
timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y);

int
timeval_add(struct timeval *result, struct timeval *x, struct timeval *y);

}
#endif /* UTIL_H_ */
