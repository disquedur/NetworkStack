#ifndef IPV4_ADDRESS_H
#define IPV4_ADDRESS_H

#include "../InternetAddress.h"
#include <stdint.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <sstream>

namespace NetworkStack
{
    class IPv4Address : public InternetAddress
    {
        public:
            static const unsigned int SIZE = 4;

            IPv4Address();
            IPv4Address(const uint32_t address);
            IPv4Address(const uint8_t* address, int size);
            IPv4Address(const std::vector<uint8_t> address);
            IPv4Address(const std::string address);

            bool isInRange(IPv4Address network, IPv4Address mask);
            uint32_t toInt() const;

            virtual int getSize() const;
            virtual std::string toString() const;
            virtual std::vector<uint8_t> toVector() const;
            virtual std::unique_ptr<InternetAddress> clone() const;

        private:
            uint32_t mAddress = 0;
    };
}

#endif