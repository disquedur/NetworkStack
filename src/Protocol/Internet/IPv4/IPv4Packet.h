#ifndef IPV4_PACKET_H
#define IPV4_PACKET_H

#include <stdexcept>

#include "IPv4Address.h"
#include "../../Data.h"
#include "../../Transport/Tcp/TcpSegment.h"
#include "../../Internet/Icmp/IcmpPacket.h"

namespace NetworkStack
{
    namespace IPv4Protocol
    {
        const uint8_t ICMP = 0x01;
        const uint8_t TCP = 0x06;
    }

    namespace IPv4Flag
    {
        const uint16_t DF = 0x4000; // Don't fragment
        const uint16_t MF = 0x2000; // More fragments
    }

    class IPv4Packet : public Data
    {
        public:
            static const unsigned int HeaderSize = 20;

            IPv4Packet(const IPv4Packet&);
            IPv4Packet(IPv4Address source, IPv4Address destination);
            IPv4Packet(const uint8_t* packet, unsigned int size);

            IPv4Packet& operator=(const IPv4Packet &ipv4Packet);
            void calculateChecksum();

            std::vector<uint8_t> buildHeader();
            virtual std::vector<uint8_t> build();
            virtual std::unique_ptr<Data> clone() const;

            // Getters Setters
            void setVersion(uint8_t version);
            uint8_t getVersion() const;
            void setHeaderLength(uint8_t headerLength);
            uint8_t getHeaderLength() const;
            void setDifferentiatedServices(uint8_t differentiatedServices);
            uint8_t getDifferentiatedServices() const;
            void setTotalLength(uint16_t totalLength);
            uint16_t getTotalLength() const;
            void setIdentification(uint16_t identification);
            uint16_t getIdentification() const;
            void setFlags(uint16_t flag, bool value);
            bool getFlags(uint16_t flag) const;
            void setFragmentOffset(uint16_t fragmentOffset);
            uint16_t getFragmentOffset() const;
            void setTimeToLive(uint8_t timeToLive);
            uint8_t getTimeToLive() const;
            void setProtocol(uint8_t protocol);
            uint8_t getProtocol() const;
            void setChecksum(uint16_t checksum);
            uint16_t getChecksum() const;
            void setSourceAddress(const IPv4Address &ip);
            IPv4Address getSourceAddress() const;
            void setDestinationAddress(const IPv4Address &ip);
            IPv4Address getDestinationAddress() const;
            void setData(const Data& data);
            Data* getData() const;

        private:
            uint8_t mVersionAndHeaderLength;
            uint8_t mDifferentiatedServices;
            uint16_t mTotalLength;
            uint16_t mIdentification;
            uint16_t mFlagsAndFragmentOffset;
            uint8_t mTimeToLive;
            uint8_t mProtocol;
            uint16_t mChecksum;

            IPv4Address mSource, mDestination;
            std::unique_ptr<Data> mData;
    };
}
#endif