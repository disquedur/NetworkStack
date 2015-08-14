#ifndef ETHERNET_PROTOCOL_H
#define ETHERNET_PROTOCOL_H

#include "EthernetFrame.h"
#include "MacAddress.h"
#include "../../AbstractProtocol.h"
#include "../../Internet/IPv4/IPv4Packet.h"
#include "../../Internet/IPv4/IPv4Address.h"
#include "../../Transport/Arp/ArpProtocol.h"

namespace NetworkStack
{
    class EthernetProtocol : AbstractProtocol
    {
        using AbstractProtocol::AbstractProtocol;

        public:
            void send(IPv4Packet ipv4Packet);
    };
}

#endif