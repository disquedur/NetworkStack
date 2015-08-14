#ifndef ICMP_TYPE_ECHO_REQUEST_H
#define ICMP_TYPE_ECHO_REQUEST_H

#include <stdint.h>
#include "IcmpType.h"

namespace NetworkStack
{
    class IcmpTypeEchoRequest : public IcmpType
    {
        public:
            static const uint8_t TYPE = 8;

            IcmpTypeEchoRequest();
            IcmpTypeEchoRequest(const uint8_t* buffer, unsigned int size);

            virtual uint8_t getType() const;
            virtual std::vector<uint8_t> getRestOfHeader() const;
            virtual std::unique_ptr<IcmpType> clone() const;

            void setIdentifier(uint16_t identifier);
            uint16_t getIdentifier() const;
            void setSequenceNumber(uint16_t sequenceNumber);
            uint16_t getSequenceNumber() const;
            void setData(std::vector<uint8_t> data);
            void setData(std::string data);
            std::string getData() const;

        private:
            uint16_t mIdentifier;
            uint16_t mSequenceNumber;
            std::vector<uint8_t> mData;
    };
}

#endif