#ifndef ETHERNET_FRAME_H
#define ETHERNET_FRAME_H

#include "MacAddress.h"
#include "../../Data.h"
#include "../../Transport/Arp/ArpPacket.h"
#include "../../Internet/IPv4/IPv4Packet.h"

namespace NetworkStack
{
    class EthernetFrame : public Data
    {
        public:
            static const uint16_t PROTOCOL_IP = 0x0800;
            static const uint16_t PROTOCOL_ARP = 0x0806;
            static const unsigned int HeaderSize = 14;
            static const unsigned int PAYLOAD_SIZE = 1536;

            EthernetFrame(const EthernetFrame&);
            EthernetFrame(const uint8_t* buffer, unsigned int size);
            EthernetFrame(MacAddress source, MacAddress destination);

            EthernetFrame& operator=(const EthernetFrame&);

            virtual std::vector<uint8_t> build();
            virtual std::unique_ptr<Data> clone() const;

            // Getters Setters
            void setDestination(MacAddress destination);
            MacAddress getDestination() const;
            void setSource(MacAddress source);
            MacAddress getSource() const;
            void setProtocol(uint16_t protocol);
            uint16_t getProtocol() const;
            void setPayload(Data& payload);
            Data* getPayload() const;

        private:
            MacAddress mDestination;
            MacAddress mSource;
            uint16_t mProtocol;

            std::unique_ptr<Data> mPayload;
    };
}

#endif