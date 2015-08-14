#include "EthernetProtocol.h"
#include <iostream>

namespace NetworkStack
{
    void EthernetProtocol::send(IPv4Packet ipv4Packet)
    {
        ArpProtocol arp(mDevice);

        MacAddress destinationMac;
        IPv4Address destinationIPv4 = ipv4Packet.getDestinationAddress();

        if (destinationIPv4.isInRange(mDevice->getNetworkAddress(), mDevice->getMask())) {
            destinationMac = arp.resolveMac(destinationIPv4);
        } else {
            destinationMac = arp.resolveMac(mDevice->getGatway());
        }

        EthernetFrame ethernetFrame(mDevice->getMac(), destinationMac);
        ethernetFrame.setPayload(ipv4Packet);

        mDevice->sendEthernet(ethernetFrame);
    }
}