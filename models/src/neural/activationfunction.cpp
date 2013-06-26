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

#include <neural/activationfunction.h>

#include <util.h>

#include <cmath>
#include <iostream>

namespace models
{
namespace neural
{

using namespace debug;

ActivationFunction * ActivationFunction::getActivationFunctionByCode(
        ActivationFunctionType type)
{
    ActivationFunction *af = 0;
    switch (type)
    {
    case Linear:
        af = new LinearAF();
        break;
    case Tanh:
        af = new TanhAF();
        break;
    case Exponential:
        af = new ExponentialAF();
        break;
    case Sigmoidal:
        af = new SigmoidalAF();
        break;
    case Square:
        af = new SquareAF();
        break;
    case ExponentialSimple:
        af = new ExponentialSimpleAF();
        break;
    }

    return af;
}

double neural::LinearAF::operator ()(double value) const
{
    return value;
}

double neural::LinearAF::derivative(double value) const
{
    return 1.0;
}

const double TanhAF::BETA = 1.0;
double neural::TanhAF::operator ()(double value) const
{
    return std::tanh(TanhAF::BETA * value);
}

double neural::TanhAF::derivative(double value) const
{
    double tanh = (*this)(value);
    return 1 - tanh*tanh;
}

const double param_a = 7.0;
const double param_b = 1.0;
double neural::ExponentialAF::operator ()(double value) const
{
    return std::exp(-(value - param_a) / param_b);
}

double neural::ExponentialAF::derivative(double value) const
{
    return std::exp(-(value - param_a) / param_b) / (param_b * param_b);
}

const double SigmoidalAF::BETA = 1.0;
double neural::SigmoidalAF::operator ()(double value) const
{
    return 1.0 / (1.0 + std::exp(-SigmoidalAF::BETA * value));
}

double neural::SigmoidalAF::derivative(double value) const
{
    double buff = (*this)(value);
    return buff * (1 - buff);
}

double neural::SquareAF::operator ()(double value) const
{
    return value * value;
}

double neural::SquareAF::derivative(double value) const
{
    return 2 * value;
}

double neural::ExponentialSimpleAF::operator ()(double value) const
{
    return exp(value);
}

double neural::ExponentialSimpleAF::derivative(double value) const
{
    return exp(value);
}

}
}
