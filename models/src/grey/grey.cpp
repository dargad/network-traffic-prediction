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
 * grey.cpp
 *
 *  Created on: 2010-05-02
 *      Author: darek
 */

#include <grey/grey.h>

#include <util.h>

#include <cmath>

namespace models
{
namespace grey
{
using namespace debug;

Grey::Grey() :
	_ag(0.0), _ug(0.0)
{

}

void Grey::provideInput(const std::vector<double>& input, unsigned horizon)
{
	_x0 = input;

	printSeq("[GREY] input: ", _x0);

	_x1 = performAGO(_x0);

	printSeq("[GREY] AGO: ", _x1);

	_z1 = applyMean(_x1);

	printSeq("[GREY] MEAN: ", _z1);

	matrixOperations();
}

const std::vector<double> Grey::performAGO(const std::vector<double>& input)
{
	std::vector<double> vecAGO;

	for (size_t i = 0; i < input.size(); ++i)
	{
		double sum = 0;
		for (size_t j = 0; j <= i; ++j)
		{
			sum += input[j];
		}
		vecAGO.push_back(sum);
	}

	return vecAGO;
}

const std::vector<double> Grey::applyMean(const std::vector<double> & input)
{
	std::vector<double> vecMEAN;

	for (size_t i = 1; i < input.size(); ++i)
	{
		vecMEAN.push_back(0.5 * (input[i] + input[i - 1]));
	}

	return vecMEAN;
}

double Grey::getPrediction(unsigned horizon)
{
	double p1 = (_x0[0] - _ug / _ag);
	double p2 = std::exp(-_ag * (_x0.size() + horizon - 1));
	double p3 = (1 - std::exp(_ag));
	double result = p1 * p2 * p3;
	return result;
}

void Grey::matrixOperations()
{
	printSeq("[GREY] _z1: ", _z1);

	double C = 0.0;
	for (unsigned i = 0; i < _z1.size(); ++i)
	{
		C += _z1[i];
	}

	double D = 0.0;
	for (unsigned i = 1; i < _x0.size(); ++i)
	{
		D += _x0[i];
	}

	double E = 0.0;
	for (unsigned i = 1; i < _x0.size(); ++i)
	{
		E += _z1[i - 1] * _x0[i];
	}

	double F = 0.0;
	for (unsigned i = 0; i < _z1.size(); ++i)
	{
		F += _z1[i] * _z1[i];
	}

	double n = _x0.size();
	_ag = (C * D - (n - 1) * E) / ((n - 1) * F - C * C);
	_ug = (D * F - C * E) / (n * F - 1 - C * C);
}

}
}
