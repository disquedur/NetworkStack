#ifndef ARP_PROTOCOL_H
#define ARP_PROTOCOL_H

#include "ArpPacket.h"
#include "../../AbstractProtocol.h"
#include "../../Link/Ethernet/EthernetFrame.h"
#include "../../Link/Ethernet/MacAddress.h"
#include "../../Internet/IPv4/IPv4Address.h"
#include "../../../Device/AbstractDevice.h"

#include <memory>
#include <map>

namespace NetworkStack
{
    class ArpProtocol : AbstractProtocol
    {
        using AbstractProtocol::AbstractProtocol;

        public:
            MacAddress resolveMac(IPv4Address ip);

        private:
            std::map<std::string, MacAddress> mTable;

            static MacAddress waitResponse(IPv4Address ip, std::shared_ptr<AbstractDevice> device);
    };
}

#endif