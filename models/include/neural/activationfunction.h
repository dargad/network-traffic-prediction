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

#ifndef ACTIVATIONFUNCTION_H_
#define ACTIVATIONFUNCTION_H_

namespace models
{
namespace neural
{

enum ActivationFunctionType
{
    Linear = 10,
    Tanh = 20,
    Exponential = 30,
    Sigmoidal = 40,
    Square = 50,
    ExponentialSimple = 60
};

const double ALPHA = 0.9;

class ActivationFunction
{
public:
    ActivationFunction(ActivationFunctionType type): _type(type) {};
    virtual ~ActivationFunction() {};

    double virtual operator()(double value) const = 0;
    double virtual derivative(double value) const = 0;

    int getActivationFunctionType() { return _type; };

    static ActivationFunction *getActivationFunctionByCode(ActivationFunctionType type);

protected:
    ActivationFunctionType _type;
};

class LinearAF: public ActivationFunction
{
public:
    LinearAF(): ActivationFunction(Linear) {};

    double operator()(double value) const;
    double derivative(double value) const;
};

class TanhAF: public ActivationFunction
{
public:
    TanhAF(): ActivationFunction(Tanh) {};

    const static double BETA;

    double operator()(double value) const;
    double derivative(double value) const;
};

class ExponentialAF: public ActivationFunction
{
public:
    ExponentialAF(): ActivationFunction(Exponential) {};

    double operator()(double value) const;
    double derivative(double value) const;
};

class ExponentialSimpleAF: public ActivationFunction
{
public:
    ExponentialSimpleAF(): ActivationFunction(ExponentialSimple) {};

    double operator()(double value) const;
    double derivative(double value) const;
};

class SigmoidalAF: public ActivationFunction
{
public:
    SigmoidalAF(): ActivationFunction(Sigmoidal) {};

    const static double BETA;

    double operator()(double value) const;
    double derivative(double value) const;
};

class SquareAF: public ActivationFunction
{
public:
    SquareAF(): ActivationFunction(Square) {};

    double operator()(double value) const;
    double derivative(double value) const;
};

}
}


#endif /* ACTIVATIONFUNCTION_H_ */
