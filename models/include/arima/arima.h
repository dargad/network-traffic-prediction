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

#ifndef ARIMA_H_
#define ARIMA_H_

#include <modelbase.h>

#include <string>
#include <vector>

namespace models
{

namespace arima
{

class Arima: public AbstractModel
{
public:
    Arima();
    virtual ~Arima();

    void provideInput(const std::vector<double>& input, unsigned horizon);

    double getPrediction(unsigned horizon);

    void setOrder(const std::vector<int>& order);
    std::vector<int> getOrder() const;

private:
    void fillOrder();
    void preparePrediction(char * filename, unsigned horizon);
    void parseOutputForPrediction(const std::string& output);

private:
    static const char *FILE_TEMPLATE;
    std::vector<double> _input;
    std::vector<int> _order;
    char _tempFilename[255];
    std::string _buff;
    std::vector<int> _outputBuff;
    unsigned _horizon;
};

}
}

#endif /* ARIMA_H_ */
