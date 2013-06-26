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

#ifndef DATAPROVIDER_H_
#define DATAPROVIDER_H_

#include <string>
#include <vector>

namespace models
{

namespace dataprovider
{

class DataProvider
{
public:
    DataProvider(const std::string& filename);
    ~DataProvider();

    size_t getDataSize() const;

    double getData(int idx) const;
    std::vector<double> getDataVector(int idx, size_t size) const;

    double getAverage(int idx, size_t size) const;
    double getMaxValue() const;

    static double getMeanSquareError(const std::vector<double>& expected,
            const std::vector<double>& actual);

    static double getNormalizedMeanSquareError(const std::vector<double>& expected,
                const std::vector<double>& actual);

    static double getSignalToErrorRatio(const std::vector<double>& expected,
                const std::vector<double>& actual);

    static double getVariance(const std::vector<double>& expected,
            const std::vector<double>& actual);



private:
    bool loadFromFile(const std::string& filename);

private:
    std::string _filename;
    std::vector<double> _items;
};

}
}

#endif /* DATAPROVIDER_H_ */
