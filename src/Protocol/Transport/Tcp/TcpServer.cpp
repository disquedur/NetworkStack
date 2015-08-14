#include "TcpServer.h"
#include <iostream>

namespace NetworkStack
{
    TcpMessage TcpServer::listen(int port)
    {
        EthernetProtocol ethernet(mDevice);
        mPort = port;
        std::string filter = "tcp dst port " + std::to_string(port);

        while (true) {
            EthernetFrame ethernetFrame = mDevice->getNextEthernet(filter);
            IPv4Packet* ipv4Packet = dynamic_cast<IPv4Packet*> (ethernetFrame.getPayload());
            TcpSegment* tcpSegment = dynamic_cast<TcpSegment*> (ipv4Packet->getData());

            std::string client = ipv4Packet->getSourceAddress().toString() + ":" + std::to_string(tcpSegment->getSourcePort());
            auto iterator = mConnections.find(client);

            if ((iterator == mConnections.end() || mConnections[client].state == TcpConnectionState::CLOSED) && tcpSegment->getFlags(TcpFlag::SYN)) {
                std::srand(std::time(0));

                mConnections[client] = {
                    TcpConnectionState::SYN_RECEIVED,
                    tcpSegment->getSourcePort(),
                    ipv4Packet->getSourceAddress(),
                    tcpSegment->getWindowSize(),
                    tcpSegment->getSequenceNumber() + 1,
                    static_cast<uint32_t> (std::rand()), {}
                };

                TcpSegment responseSegment;
                responseSegment.setSequenceNumber(mConnections[client].serverSequenceNumber);
                responseSegment.setAckNumber(tcpSegment->getSequenceNumber() + 1);
                responseSegment.setFlags(TcpFlag::SYN | TcpFlag::ACK, true);
                responseSegment.setDestinationPort(mConnections[client].port);
                responseSegment.setSourcePort(port);

                IPv4Packet responsePacket(mDevice->getIP(), mConnections[client].address);
                responsePacket.setProtocol(IPv4Protocol::TCP);
                responsePacket.setData(responseSegment);

                ethernet.send(responsePacket);

                // std::cout << client << " SYN_RECEIVED" << std::endl;
            } else if (mConnections[client].state == TcpConnectionState::SYN_RECEIVED && tcpSegment->getFlags(TcpFlag::ACK)) {
                mConnections[client].state = TcpConnectionState::ESTABLISHED;
                mConnections[client].serverSequenceNumber = tcpSegment->getAckNumber();
                mConnections[client].window = tcpSegment->getWindowSize();

                // std::cout << client << " ESTABLISHED" << std::endl;
            } else if (mConnections[client].state == TcpConnectionState::ESTABLISHED && tcpSegment->getFlags(TcpFlag::FIN)) {
                mConnections[client].state = TcpConnectionState::FIN_RECEIVED;
                mConnections[client].clientSequenceNumber = tcpSegment->getSequenceNumber() + 1;

                TcpSegment responseSegment;
                responseSegment.setSequenceNumber(mConnections[client].serverSequenceNumber);
                responseSegment.setAckNumber(tcpSegment->getSequenceNumber() + 1);
                responseSegment.setFlags(TcpFlag::FIN | TcpFlag::ACK, true);
                responseSegment.setDestinationPort(mConnections[client].port);
                responseSegment.setSourcePort(port);

                IPv4Packet responsePacket(mDevice->getIP(), mConnections[client].address);
                responsePacket.setProtocol(IPv4Protocol::TCP);
                responsePacket.setData(responseSegment);

                ethernet.send(responsePacket);

                // std::cout << client << " FIN_RECEIVED" << std::endl;
            } else if (mConnections[client].state == TcpConnectionState::FIN_RECEIVED && tcpSegment->getFlags(TcpFlag::ACK)) {
                mConnections[client].state = TcpConnectionState::CLOSED;
                mConnections[client].serverSequenceNumber = tcpSegment->getAckNumber();

                // std::cout << client << " CLOSED" << std::endl;
            } else if (mConnections[client].state == TcpConnectionState::ESTABLISHED) {
                mConnections[client].segments[tcpSegment->getSequenceNumber() + tcpSegment->getData().size()] = {
                    tcpSegment->getData(),
                    true
                };

                mConnections[client].clientSequenceNumber += tcpSegment->getData().size();

                TcpSegment responseSegment;
                responseSegment.setSequenceNumber(mConnections[client].serverSequenceNumber);
                responseSegment.setAckNumber(mConnections[client].clientSequenceNumber);
                responseSegment.setFlags(TcpFlag::ACK, true);
                responseSegment.setDestinationPort(mConnections[client].port);
                responseSegment.setSourcePort(port);

                IPv4Packet responsePacket(mDevice->getIP(), mConnections[client].address);
                responsePacket.setProtocol(IPv4Protocol::TCP);
                responsePacket.setData(responseSegment);

                ethernet.send(responsePacket);

                // std::cout << client << " DATA INCOMING : " << tcpSegment->getData() << std::endl;

                if (tcpSegment->getFlags(TcpFlag::PSH)) {
                    std::string data;
                    auto segments = mConnections[client].segments;

                    for (auto it = segments.begin(); it != segments.end(); ++it) {
                        data += it->second.data;
                    }

                    mConnections[client].segments.clear();

                    return TcpMessage {
                        client,
                        data
                    };
                }
            }
        }
    }

    void TcpServer::send(TcpMessage message)
    {
        EthernetProtocol ethernet(mDevice);

        if (mConnections.count(message.client) > 0) {
            if (mConnections[message.client].state == TcpConnectionState::ESTABLISHED) {
                // std::cout << message.value.size() << " - " << mConnections[message.client].window << std::endl;

                size_t numberOfSegments = 1;

                if (message.value.size() > mConnections[message.client].window) {
                    numberOfSegments = std::ceil(message.value.size() / mConnections[message.client].window);
                }

                std::vector<TcpSessionSegment> segments;

                size_t minsize = message.value.size() / numberOfSegments;
                int extra = message.value.size() - minsize * numberOfSegments;

                for (size_t i = 0, offset = 0; i < numberOfSegments; ++i, --extra) {
                    size_t size = minsize + (extra > 0 ? 1 : 0);
                    if ((offset + size) < message.value.size()) {
                        segments.push_back(TcpSessionSegment {
                            message.value.substr(offset,size),
                            false
                        });
                    } else {
                        segments.push_back(TcpSessionSegment {
                            message.value.substr(offset, message.value.size() - offset),
                            false
                        });
                    }

                    offset += size;
                }

                for (size_t i = 0; i < segments.size(); i++) {

                    // std::cout << segments[i].data.size() << std::endl;

                    TcpSegment segment;
                    segment.setSequenceNumber(mConnections[message.client].serverSequenceNumber);
                    segment.setAckNumber(mConnections[message.client].clientSequenceNumber);
                    segment.setFlags(TcpFlag::ACK, true);
                    segment.setDestinationPort(mConnections[message.client].port);
                    segment.setSourcePort(mPort);
                    segment.setData(segments[i].data);

                    if (i == segments.size()) {
                        segment.setFlags(TcpFlag::PSH, true);
                    }

                    IPv4Packet responsePacket(mDevice->getIP(), mConnections[message.client].address);
                    responsePacket.setProtocol(IPv4Protocol::TCP);
                    responsePacket.setData(segment);

                    ethernet.send(responsePacket);

                    EthernetFrame ethernetFrame = mDevice->getNextEthernet("tcp dst port " + std::to_string(mPort));
                    IPv4Packet* ipv4Packet = dynamic_cast<IPv4Packet*> (ethernetFrame.getPayload());
                    TcpSegment* tcpSegment = dynamic_cast<TcpSegment*> (ipv4Packet->getData());

                    std::string client = ipv4Packet->getSourceAddress().toString() + ":" + std::to_string(tcpSegment->getSourcePort());

                    if (message.client == client) {
                        mConnections[message.client].serverSequenceNumber += segments[i].data.size();
                        segments[i].acknowledged = true;
                    }
                }
            }
        }
    }
}
