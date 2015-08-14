#include "IcmpProtocol.h"
#include <iostream>
#include "../../../Device/PCAP/PCAPDevice.h"
#include <memory>

namespace NetworkStack
{
    uint16_t IcmpProtocol::sequenceNumber = 1;

    void IcmpProtocol::sendEchoRequest(IPv4Address ip)
    {
        EthernetProtocol ethernet(mDevice);

        IcmpTypeEchoRequest echoRequest;
        echoRequest.setData("InterPaul Packet Data !");
        echoRequest.setIdentifier(sequenceNumber);
        echoRequest.setSequenceNumber(sequenceNumber);

        IcmpPacket icmpPacket;
        icmpPacket.setType(echoRequest);

        IPv4Packet ipv4Packet(mDevice->getIP(), ip);
        ipv4Packet.setProtocol(IPv4Protocol::ICMP);
        ipv4Packet.setFlags(IPv4Flag::DF, true);
        ipv4Packet.setData(icmpPacket);

        ethernet.send(ipv4Packet);
    }

    void IcmpProtocol::sendEchoReply(IPv4Address ip, IcmpTypeEchoRequest request)
    {
        EthernetProtocol ethernet(mDevice);

        IcmpTypeEchoReply echoReply(request);

        IcmpPacket icmpPacket;
        icmpPacket.setType(echoReply);

        IPv4Packet ipv4Packet(mDevice->getIP(), ip);
        ipv4Packet.setProtocol(IPv4Protocol::ICMP);
        ipv4Packet.setFlags(IPv4Flag::DF, true);
        ipv4Packet.setData(icmpPacket);

        ethernet.send(ipv4Packet);
    }

    IcmpTypeEchoRequest& IcmpProtocol::getEchoRequest(const IPv4Address& ip)
    {
        while (true) {
            EthernetFrame ethernetFrame = mDevice->getNextEthernet(
                "icmp and ether dst " + mDevice->getMac().toString()
                + "and src " + ip.toString()
            );

            IPv4Packet* ipv4Packet = dynamic_cast<IPv4Packet*> (ethernetFrame.getPayload());
            IcmpPacket* icmpPacket = dynamic_cast<IcmpPacket*> (ipv4Packet->getData());

            IcmpTypeEchoRequest* icmpTypeEchoRequest = dynamic_cast<IcmpTypeEchoRequest*> (icmpPacket->getType());
            if (icmpTypeEchoRequest) {
                return *icmpTypeEchoRequest;
            }
        }
    }

    void IcmpProtocol::autoEchoReply()
    {
        while (true) {
            EthernetFrame ethernetFrame = mDevice->getNextEthernet("icmp and ether dst " + mDevice->getMac().toString());
            IPv4Packet* ipv4Packet = dynamic_cast<IPv4Packet*> (ethernetFrame.getPayload());
            IcmpPacket* icmpPacket = dynamic_cast<IcmpPacket*> (ipv4Packet->getData());

            IcmpTypeEchoRequest* icmpTypeEchoRequest = dynamic_cast<IcmpTypeEchoRequest*> (icmpPacket->getType());
            if (icmpTypeEchoRequest) {
                sendEchoReply(ipv4Packet->getSourceAddress(), *icmpTypeEchoRequest);
            }

        }
    }
}