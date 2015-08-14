#ifndef HARDWARE_ADDRESS_H
#define HARDWARE_ADDRESS_H

#include <string>
#include <vector>
#include <memory>

namespace NetworkStack
{
    class HardwareAddress
    {
        public:
            virtual ~HardwareAddress() {}

            virtual int getSize() const = 0;
            virtual std::string toString() const = 0;
            virtual std::vector<uint8_t> toVector() const = 0;
            virtual std::unique_ptr<HardwareAddress> clone() const = 0;
    };
}

#endif