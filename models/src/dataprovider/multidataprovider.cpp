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

#include <dataprovider/multidataprovider.h>

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

MultiDataProvider::MultiDataProvider(const std::string& filename) :
    _filename(filename)
{
    loadFromFile(_filename);
    dbg() << "MultiDataProvider(): " << _filename << std::endl;
}

MultiDataProvider::~MultiDataProvider()
{
    dbg() << "~MultiDataProvider()" << std::endl;
}

std::vector<double> MultiDataProvider::getData(int idx) const
{
    return _items[idx];
}

std::vector<std::vector<double> > MultiDataProvider::getDataVector(int idx,
        size_t size) const
{
    std::vector<std::vector<double> > tmpVector;

    for (size_t i = 0; i < size; ++i)
    {
        tmpVector.push_back(_items[idx + i]);
    }

    return tmpVector;
}

size_t MultiDataProvider::getDataSize() const
{
    return _items.size();
}

std::vector<double> MultiDataProvider::getDataSubvector(int idx, size_t start,
        size_t size) const
{
    std::vector<double> tmp;

    std::copy(_items[idx].begin() + start,
            _items[idx].begin() + start + size, back_inserter(tmp));

    return tmp;
}

bool MultiDataProvider::loadFromFile(const std::string & filename)
{
    ifstream inData;
    inData.open(filename.c_str());
    if (!inData)
        return false;

    size_t size = 0;
    string line;
    while (!inData.eof())
    {
        std::vector<double> lineValues;
        getline(inData, line);
        std::istringstream iss(line);
        while( !iss.eof() )
        {
            double tmp = 0.0;
            iss >> tmp;
            lineValues.push_back(tmp);
        }

        _items.push_back(lineValues);

        size++;
    }

    return true;
}

double MultiDataProvider::getMaxValue() const
{
    double max = 0.0;

    for(unsigned i=0; i < _items.size(); ++i)
    {
        double el = *(std::max_element(_items[i].begin(), _items[i].end()));
        max = std::max(max, el);
    }

    dbg() << "[MultiDataProvider::getMaxValue()] MAX: " << max << std::endl;
    return max;
}

double MultiDataProvider::getValue(int idx, size_t pos) const
{
    return _items[idx][pos];
}

}
}
