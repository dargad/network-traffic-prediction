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

#ifndef CHAOSMODEL_H_
#define CHAOSMODEL_H_

#include <modelbase.h>

#include <vector>

namespace models
{

namespace chaos
{

class Chaos: public AbstractModel
{
public:
    Chaos(unsigned d, unsigned t);
    virtual ~Chaos();

    void provideInput(const std::vector<double>& inputValues, unsigned horizon);

    double getPrediction(unsigned horizon);

private:
    void createPhaseSpace();
    double getDistance(unsigned k, unsigned knn, unsigned dstart, unsigned dend);
    double getSumSquares(unsigned k, unsigned knn, unsigned dstart, unsigned dend);
    void checkFalseNearestNeighbours();
    void findNearestNeighbourToTheLastRow(double& dist, unsigned& idx);
    void performPrediction();

private:
    std::vector<double> _inputBuffer;

//  std::vector<std::vector<double> > _y;

    // embedding dimension
    unsigned _d;
    // delay
    unsigned _t;

    unsigned _originalPhasePoints;

    std::vector<double> _outputBuffer;
};

}

}

#endif /* CHAOSMODEL_H_ */
