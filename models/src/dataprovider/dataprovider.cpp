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
 * dataprovider.cpp
 *
 *  Created on: 2010-05-01
 *      Author: darek
 */

#include <dataprovider/dataprovider.h>

#include <util.h>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>

using namespace std;
namespace models
{

namespace dataprovider
{

using namespace debug;

DataProvider::DataProvider(const std::string& filename) :
	_filename(filename)
{
	loadFromFile(_filename);
	dbg() << "DataProvider(): " << _filename << std::endl;
}

DataProvider::~DataProvider()
{
	dbg() << "~DataProvider()" << std::endl;
}

double DataProvider::getData(int idx) const
{
	return _items[idx];
}

std::vector<double> DataProvider::getDataVector(int idx, size_t size) const
{
	std::vector<double> tmpVector;

	for (size_t i = 0; i < size; ++i)
	{
		tmpVector.push_back(_items[idx + i]);
	}

	return tmpVector;
}

size_t DataProvider::getDataSize() const
{
	return _items.size();
}

double DataProvider::getAverage(int idx, size_t size) const
{
	double sum = 0.0;

	for (size_t i = idx; i < idx + size; ++i)
	{
		sum += _items[i];
	}

	return sum / size;
}

double DataProvider::getMeanSquareError(const std::vector<double> & expected,
		const std::vector<double> & actual)
{
	double sumError = 0.0;

	for (size_t i = 0; i < actual.size(); ++i)
	{
		double error = expected[i] - actual[i];
		error *= error;
		sumError += error;
	}

	return sumError / expected.size();
}

double DataProvider::getMaxValue() const
{
	double max = *std::max_element(_items.begin(), _items.end());
	return max;
}

double DataProvider::getVariance(const std::vector<double> & expected,
		const std::vector<double> & actual)
{
	// calculate average
	double sum = 0.0;

	for (size_t i = 0; i < actual.size(); ++i)
	{
		double val = actual[i];
		sum += val;
	}

	double avg = sum / actual.size();

	// calculate variance
	sum = 0.0;
	for (size_t i = 0; i < actual.size(); ++i)
	{
		double sq = actual[i] - avg;
		sq *= sq;
		sum += sq;
	}

	return sum / actual.size();
}

double DataProvider::getNormalizedMeanSquareError(
		const std::vector<double> & expected,
		const std::vector<double> & actual)
{
	double meanSqErr = getMeanSquareError(expected, actual);
	double variance = getVariance(expected, actual);

	return meanSqErr / variance;
}

double DataProvider::getSignalToErrorRatio(
		const std::vector<double> & expected,
		const std::vector<double> & actual)
{
	double rmsData = 0.0;
	double rmsError = 0.0;

	for (size_t i = 0; i < actual.size(); ++i)
	{
		double valData = actual[i];
		valData *= valData;
		double valError = actual[i] - expected[i];
		valError *= valError;

		rmsData += valData;
		rmsError += valError;
	}

	unsigned n = actual.size();
	rmsData /= n;
	rmsError /= n;

	rmsData = std::sqrt(rmsData);
	rmsError = std::sqrt(rmsError);

	return 10 * std::log10(rmsData / rmsError);
}

bool DataProvider::loadFromFile(const std::string & filename)
{
	ifstream inData;
	list<double> tmpList;
	inData.open(filename.c_str());
	if (!inData)
		return false;

	size_t size = 0;
	string line;
	while (!inData.eof())
	{
		getline(inData, line);
		double tmp = 0.0;
		std::istringstream iss(line);
		if (iss >> tmp)
		{
			tmpList.push_back(tmp);
		}
		else
		{
		}

		size++;
	}

	_items.reserve(size);
	copy(tmpList.begin(), tmpList.end(), back_inserter(_items));

	return true;
}

}
}
