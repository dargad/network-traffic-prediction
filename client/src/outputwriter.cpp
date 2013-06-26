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
 * outputwriter.cpp
 *
 *  Created on: 2010-05-11
 *      Author: darek
 */

#include <outputwriter.h>

#include <algorithm>
#include <iterator>

namespace prediction
{

namespace client
{

OutputWriter::OutputWriter(const std::string& outputFile) :
	_output(outputFile.c_str()), _outputLines(0)
{

}

OutputWriter::~OutputWriter()
{
	if (_output.is_open())
	{
		_output.close();
	}
}

void OutputWriter::write(const std::vector<double> & inputValues, double result, double expected)
{
	std::copy(inputValues.begin(), inputValues.end(), std::ostream_iterator<
			double>(_output, "\t"));
	_output << result << '\t';
	_output << expected << '\n';
	++_outputLines;

	// flush file every 100 lines
	//	if( !(_outputLines % 30) )
	//		_output.flush();
}

void OutputWriter::writeTime(double time)
{
	_output << "Time: " << time << std::endl;
}

void OutputWriter::close()
{
	_output.close();
}

}
}
