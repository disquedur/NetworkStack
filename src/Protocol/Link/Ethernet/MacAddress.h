#ifndef MAC_ADDRESS_H
#define MAC_ADDRESS_H

#include "../HardwareAddress.h"
#include <stdint.h>
#include <stdexcept>
#include <sstream>
#include <iomanip>

namespace NetworkStack
{
    class MacAddress : public HardwareAddress
    {
        public:
            static const int SIZE = 6;

            MacAddress();
            MacAddress(const std::vector<uint8_t> address);
            MacAddress(const uint8_t* address, int size);
            MacAddress(const std::string& address);

            virtual int getSize() const;
            virtual std::string toString() const;
            virtual std::vector<uint8_t> toVector() const;
            virtual std::unique_ptr<HardwareAddress> clone() const;

        private:
            std::vector<uint8_t> mAddress;
    };
}

#endif