#ifndef ICMP_PACKET_H
#define ICMP_PACKET_H

#include <stdint.h>
#include <stdexcept>
#include <memory>
#include <netinet/in.h>

#include "IcmpType.h"
#include "IcmpTypeEchoRequest.h"
#include "IcmpTypeEchoReply.h"
#include "../../Data.h"

namespace NetworkStack
{
    class IcmpPacket : public Data
    {
        public:
            IcmpPacket();
            IcmpPacket(const IcmpPacket&);
            IcmpPacket(const uint8_t* buffer, unsigned int size);

            IcmpPacket& operator=(const IcmpPacket&);

            void calculateChecksum();
            std::vector<uint8_t> buildWithoutCalculateChecksum();

            virtual std::vector<uint8_t> build();
            virtual std::unique_ptr<Data> clone() const;

            // Getters Setters
            void setChecksum(uint16_t checksum);
            uint16_t getChecksum() const;
            void setType(const IcmpType& type);
            IcmpType* getType() const;

        private:
            uint16_t mChecksum;
            std::unique_ptr<IcmpType> mType;
    };
}

#endif