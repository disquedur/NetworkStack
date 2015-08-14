#ifndef ICMP_PROTOCOL_H
#define ICMP_PROTOCOL_H

#include "../../AbstractProtocol.h"
#include "../../Link/Ethernet/EthernetProtocol.h"
#include "../../Internet/IPv4/IPv4Address.h"
#include "../../Internet/IPv4/IPv4Packet.h"
#include "../../Internet/Icmp/IcmpPacket.h"
#include "../../Internet/Icmp/IcmpTypeEchoRequest.h"

namespace NetworkStack
{
    class IcmpProtocol : public AbstractProtocol
    {
        using AbstractProtocol::AbstractProtocol;

        public:
            void sendEchoRequest(IPv4Address ip);
            void sendEchoReply(IPv4Address ip, IcmpTypeEchoRequest request);
            IcmpTypeEchoRequest& getEchoRequest(const IPv4Address& ip);
            void autoEchoReply();

        private:
            static uint16_t sequenceNumber;
    };
}

#endif