#ifndef TCP_SEGMENT_H
#define TCP_SEGMENT_H

#include <stdint.h>
#include <stdexcept>

#include "../../Data.h"
#include "../../Internet/IPv4/IPv4Packet.h"

namespace NetworkStack
{
    namespace TcpFlag
    {
        const uint8_t CWR = 0x80;
        const uint8_t ECE = 0x40;
        const uint8_t URG = 0x20;
        const uint8_t ACK = 0x10;
        const uint8_t PSH = 0x08;
        const uint8_t RST = 0x04;
        const uint8_t SYN = 0x02;
        const uint8_t FIN = 0x01;
    }

    class IPv4Packet;

    class TcpSegment : public Data
    {
        public:
            static const unsigned int HeaderSize = 20;

            TcpSegment();
            TcpSegment(std::string data);
            TcpSegment(const uint8_t* packet, unsigned int size);

            void calculateChecksum(const IPv4Packet* ipv4Packet);

            virtual std::vector<uint8_t> build();
            virtual std::unique_ptr<Data> clone() const;

            // getters/setters
            void setSourcePort(uint16_t port);
            uint16_t getSourcePort() const;
            void setDestinationPort(uint16_t port);
            uint16_t getDestinationPort() const;
            void setSequenceNumber(uint32_t sequenceNumber);
            uint32_t getSequenceNumber() const;
            void setAckNumber(uint32_t ackNumber);
            uint32_t getAckNumber() const;
            void setDataOffset(uint8_t dataOffset);
            uint8_t getDataOffset() const;
            void setFlags(uint8_t flags, bool value);
            bool getFlags(uint8_t flags) const;
            void setWindowSize(uint16_t windowSize);
            uint16_t getWindowSize() const;
            void setChecksum(uint16_t checksum);
            uint16_t getChecksum() const;
            void setUrgentPointer(uint16_t urgentPointer);
            uint16_t getUrgentPointer() const;
            void setData(std::string data);
            std::string getData() const;

        private:
            uint16_t mSourcePort;
            uint16_t mDestinationPort;
            uint32_t mSequenceNumber;
            uint32_t mAckNumber;
            uint8_t mDataOffset;
            uint8_t mFlags;
            uint16_t mWindowSize;
            uint16_t mChecksum;
            uint16_t mUrgentPointer;

            std::vector<uint8_t> mData;
    };
}
#endif