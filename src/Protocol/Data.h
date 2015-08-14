#ifndef DATA_H
#define DATA_H

#include <stdint.h>
#include <vector>
#include <memory>

namespace NetworkStack
{
    class Data
    {
        public:
            virtual ~Data() {}

            virtual std::vector<uint8_t> build() = 0;
            virtual std::unique_ptr<Data> clone() const = 0;
    };
}

#endif