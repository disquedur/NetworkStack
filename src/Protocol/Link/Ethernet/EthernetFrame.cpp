#include "EthernetFrame.h"

namespace NetworkStack
{
    EthernetFrame::EthernetFrame(const EthernetFrame& ethernetPacket)
    {
        mDestination = ethernetPacket.mDestination;
        mSource = ethernetPacket.mSource;
        mProtocol = ethernetPacket.mProtocol;

        if (ethernetPacket.mPayload) {
            mPayload = ethernetPacket.mPayload->clone();
        }
    }

    EthernetFrame::EthernetFrame(const uint8_t* buffer, unsigned int size)
    {
        if (size < HeaderSize) {
            throw std::runtime_error("Unable to generate Ethernet header.");
        }

        mDestination = MacAddress(buffer, MacAddress::SIZE);
        mSource = MacAddress(buffer + MacAddress::SIZE, MacAddress::SIZE);

        setProtocol(256U * buffer[12] + buffer[13]);

        size -= HeaderSize;
        if (size > 0) {
            buffer += HeaderSize;

            if (getProtocol() == PROTOCOL_ARP) {
                mPayload.reset(new ArpPacket(buffer, size));
            } else if (getProtocol() == PROTOCOL_IP) {
                mPayload.reset(new IPv4Packet(buffer, size));
            }
        }
    }

    EthernetFrame::EthernetFrame(MacAddress source, MacAddress destination)
    {
        setSource(source);
        setDestination(destination);
    }

    EthernetFrame& EthernetFrame::operator=(const EthernetFrame& ethernetPacket)
    {
        if (this != &ethernetPacket) {
            mDestination = ethernetPacket.mDestination;
            mSource = ethernetPacket.mSource;
            mProtocol = ethernetPacket.mProtocol;

            if (ethernetPacket.mPayload) {
                mPayload = ethernetPacket.mPayload->clone();
            }
        }

        return *this;
    }

    std::vector<uint8_t> EthernetFrame::build()
    {
        std::vector<uint8_t> buffer;

        auto destinationBuffer = mDestination.toVector();
        buffer.insert(buffer.end(), destinationBuffer.begin(), destinationBuffer.end());

        auto sourceBuffer = mSource.toVector();
        buffer.insert(buffer.end(), sourceBuffer.begin(), sourceBuffer.end());

        buffer.push_back(mProtocol);
        buffer.push_back(mProtocol >> 8);

        if (mPayload) {
            std::vector<uint8_t> payloadBuffer = mPayload->build();
            buffer.insert(buffer.end(), payloadBuffer.begin(), payloadBuffer.end());
        }

        // 1536 byte is the ethernet frame maximum size
        if (buffer.size() > PAYLOAD_SIZE) {
            throw std::runtime_error("Unable to generate Ethernet frame (payload too big).");
        }

        return buffer;
    }

    std::unique_ptr<Data> EthernetFrame::clone() const
    {
        return std::unique_ptr<Data> (new EthernetFrame(*this));
    }

    void EthernetFrame::setDestination(MacAddress destination)
    {
        mDestination = destination;
    }

    MacAddress EthernetFrame::getDestination() const
    {
        return mDestination;
    }

    void EthernetFrame::setSource(MacAddress source)
    {
        mSource = source;
    }

    MacAddress EthernetFrame::getSource() const
    {
        return mSource;
    }

    void EthernetFrame::setProtocol(uint16_t protocol)
    {
        mProtocol = htons(protocol);
    }

    uint16_t EthernetFrame::getProtocol() const
    {
        return ntohs(mProtocol);
    }

    void EthernetFrame::setPayload(Data& payload)
    {
        mPayload = payload.clone();

        if (dynamic_cast<ArpPacket*> (&payload)) {
            setProtocol(PROTOCOL_ARP);
        } else if (dynamic_cast<IPv4Packet*> (&payload)) {
            setProtocol(PROTOCOL_IP);
        }
    }

    Data* EthernetFrame::getPayload() const
    {
        if (!mPayload) {
            throw std::runtime_error("Ethernet payload is empty.");
        }

        return mPayload.get();
    }
}