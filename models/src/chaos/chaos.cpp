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

#include <chaos/chaos.h>

#include <util.h>

#include <cmath>
#include <limits>
#include <sstream>

#include <boost/foreach.hpp>

namespace models
{

namespace chaos
{

using namespace debug;

void printArray(const std::vector<std::vector<double> >& array)
{
    for (unsigned i = 0; i < array.size(); ++i)
    {
        std::ostringstream oss;
        oss << i << ": ";
        printSeq(oss.str(), array[i]);
    }
}

Chaos::Chaos(unsigned d, unsigned t) :
    _d(d), _t(t), _originalPhasePoints(0)
{
    dbg() << "ChaosModel()" << std::endl;
}

Chaos::~Chaos()
{
    dbg() << "~ChaosModel()" << std::endl;
}

double Chaos::getPrediction(unsigned horizon)
{
    return _outputBuffer[horizon-1];
}

void Chaos::provideInput(const std::vector<double> & inputValues,
        unsigned horizon)
{
    _inputBuffer = inputValues;
    _outputBuffer.clear();
    _outputBuffer.reserve(horizon);

    for (unsigned h = 1; h <= horizon; ++h)
    {
        performPrediction();
        _inputBuffer.push_back(_outputBuffer.back());
    }
}

void Chaos::performPrediction()
{
    createPhaseSpace();

    _originalPhasePoints = _inputBuffer.size()-(_d-1)*_t;

    double dist = 0.0;
    unsigned idx = -1;

    findNearestNeighbourToTheLastRow(dist, idx);

    double tmp = 0.0;

    for(unsigned k=0; k<_d-2; ++k)
    {
        double sq = _inputBuffer[_inputBuffer.size() - (_d-1)*_t + k*_t] - _inputBuffer[idx+!+k*_t];
        sq *= sq;
        tmp += sq;
    }

    dbg() << "dist: " << dist << std::endl;
    dbg() << "tmp: " << tmp << std::endl;

    tmp = std::sqrt(dist*dist - tmp);

    double pred = _inputBuffer[idx+1 - (_d-1)*_t] - tmp;

    _outputBuffer.push_back(pred);
}

void Chaos::findNearestNeighbourToTheLastRow(double& dist, unsigned& idx)
{
    double minDist = std::numeric_limits<double>::max();
    int index = 0;
    double k = _originalPhasePoints - 1;

    for (unsigned i = 0; i < k; ++i)
    {
        double dst = getDistance(k, i, 0, _d);
        if (dst < minDist)
        {
            minDist = dst;
            index = i;
        }
    }

    dist = minDist;
    idx = index;
}

void Chaos::checkFalseNearestNeighbours()
{
//  unsigned k = _y.size() - 2;
//  unsigned knn = _y.size() - 1;
//
//  for (unsigned d = 1; d < _d - 1; ++d)
//  {
//      double dist = getDistance(k, knn, 0, d);
//
//      double rtNumerator = std::abs(_y[k][d + 1] - _y[knn][d + 1]);
//  }
}

double Chaos::getSumSquares(unsigned k, unsigned knn, unsigned dstart,
        unsigned dend)
{
    double dist = 0.0;

    for (unsigned i = dstart; i < dend; ++i)
    {
        //      dbg() << "i: " << i << std::endl;
        //      dbg() << "_y[k].size(): " << _y[k].size() << std::endl;
        //      dbg() << "_y[knn].size(): " << _y[knn].size() << std::endl;
        //      dbg() << "k: " << k << std::endl;
        //      dbg() << "knn: " << knn << std::endl;
        double tmp = _inputBuffer[k+i*_t] - _inputBuffer[knn+i*_t];
//      double tmp = _y[k][i] - _y[knn][i];
        tmp *= tmp;
        dist += tmp;
    }

    return dist;
}

double Chaos::getDistance(unsigned k, unsigned knn, unsigned dstart,
        unsigned dend)
{
    return std::sqrt(getSumSquares(k, knn, dstart, dend));
}

void Chaos::createPhaseSpace()
{
//  _y.clear();
//
//  unsigned n = _inputBuffer.size();
//
//  unsigned N = n - (_d - 1) * _t;
//  dbg() << "N: " << N << std::endl;
//
//  for (unsigned i = 0; i < N; ++i)
//  {
//      _y.push_back(std::vector<double>());
//      for (unsigned j = 1; j <= _d; ++j)
//      {
//          unsigned idx = i + (j - 1) * _t;
//          _y[i].push_back(_inputBuffer[idx]);
//      }
//  }
}

}

}
