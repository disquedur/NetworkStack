#ifndef ICMP_TYPE_H
#define ICMP_TYPE_H

#include <stdint.h>
#include <netinet/in.h>
#include <vector>
#include <memory>

namespace NetworkStack
{
    class IcmpType
    {
        public:
            virtual ~IcmpType() {}

            virtual uint8_t getType() const = 0;
            virtual uint8_t getCode() const { return 0; }
            virtual std::vector<uint8_t> getRestOfHeader() const = 0;
            virtual std::unique_ptr<IcmpType> clone() const = 0;
    };
}

#endif