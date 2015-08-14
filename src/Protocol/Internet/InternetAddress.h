#ifndef INTERNET_ADDRESS_H
#define INTERNET_ADDRESS_H

#include <string>
#include <vector>
#include <memory>

namespace NetworkStack
{
    class InternetAddress
    {
        public:
            virtual ~InternetAddress() {}

            virtual int getSize() const = 0;
            virtual std::string toString() const = 0;
            virtual std::vector<uint8_t> toVector() const = 0;
            virtual std::unique_ptr<InternetAddress> clone() const = 0;
    };
}

#endif