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

#ifndef MULTIDATAPROVIDER_H_
#define MULTIDATAPROVIDER_H_

#include <string>
#include <vector>

namespace models
{

namespace dataprovider
{

class MultiDataProvider
{
public:
    MultiDataProvider(const std::string& filename);
    ~MultiDataProvider();

    size_t getDataSize() const;

    double getValue(int idx, size_t pos) const;

    std::vector<double> getData(int idx) const;

    std::vector<double> getDataSubvector(int idx, size_t start, size_t size) const;

    std::vector<std::vector<double> > getDataVector(int idx, size_t size) const;

    double getMaxValue() const;

private:
    bool loadFromFile(const std::string& filename);

private:
    std::string _filename;
    std::vector<std::vector<double> > _items;
};

}
}

#endif /* MULTIDATAPROVIDER_H_ */