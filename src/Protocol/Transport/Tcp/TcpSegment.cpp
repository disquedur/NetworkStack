#include "TcpSegment.h"
#include <iostream>

namespace NetworkStack
{
    TcpSegment::TcpSegment()
    {
        setSourcePort(0);
        setDestinationPort(0);
        setSequenceNumber(0);
        setAckNumber(0);
        setFlags(0xFF, false); // set all flags to false
        setDataOffset(5);
        setWindowSize(65392);
        setChecksum(0);
        setUrgentPointer(0);
    }

    TcpSegment::TcpSegment(const uint8_t* packet, unsigned int size)
    {
        if (size < HeaderSize) {
            throw std::runtime_error("Unable to generate TCP Segment.");
        }

        mSourcePort = 256U * packet[0] + packet[1];
        mDestinationPort = 256U * packet[2] + packet[3];
        mSequenceNumber = packet[4] | (packet[5] << 8) | (packet[6] << 16) | (packet[7] << 24);
        mAckNumber = packet[8] | (packet[9] << 8) | (packet[10] << 16) | (packet[11] << 24);
        mDataOffset = packet[12];
        mFlags = packet[13];
        mWindowSize = 256U * packet[14] + packet[15];
        mChecksum = 256U * packet[16] + packet[17];
        mUrgentPointer = 256U * packet[18] + packet[19];

        for (size_t i = HeaderSize; i < size; i++) {
            if (packet[i] != 0) {
                mData.push_back(packet[i]);
            }
        }
    }

    void TcpSegment::calculateChecksum(const IPv4Packet* ipv4Packet)
    {
        if (ipv4Packet == nullptr) {
            throw new std::runtime_error("Unable to calculate TCP checksum without IPv4 packet.");
        }

        setChecksum(0);

        std::vector<uint8_t> pseudoHeaderBuffer;
        std::vector<uint8_t> tcpSegmentBuffer = build();

        std::vector<uint8_t> address = ipv4Packet->getSourceAddress().toVector();
        pseudoHeaderBuffer.insert(pseudoHeaderBuffer.end(), address.begin(), address.end());

        address = ipv4Packet->getDestinationAddress().toVector();
        pseudoHeaderBuffer.insert(pseudoHeaderBuffer.end(), address.begin(), address.end());

        pseudoHeaderBuffer.push_back(0);
        pseudoHeaderBuffer.push_back(ipv4Packet->getProtocol());

        pseudoHeaderBuffer.push_back(tcpSegmentBuffer.size() >> 8);
        pseudoHeaderBuffer.push_back(tcpSegmentBuffer.size() & 0xFF);

        uint32_t checksum = 0;

        for (size_t i = 0; i < pseudoHeaderBuffer.size(); i += 2) {
            checksum += 256U * pseudoHeaderBuffer[i] + pseudoHeaderBuffer[i + 1];
        }

        for (size_t i = 0; i < tcpSegmentBuffer.size() - 1; i += 2) {
            checksum += 256U * tcpSegmentBuffer[i] + tcpSegmentBuffer[i + 1];
        }

        if (tcpSegmentBuffer.size() % 2) {
            checksum += tcpSegmentBuffer.back() << 8;
        }

        while (checksum >> 16) {
            checksum = (checksum & 0xFFFF) + (checksum >> 16);
        }

        setChecksum(static_cast<uint16_t> (~checksum));
    }

    std::vector<uint8_t> TcpSegment::build()
    {
        std::vector<uint8_t> buffer;

        buffer.push_back(mSourcePort);
        buffer.push_back(mSourcePort >> 8);

        buffer.push_back(mDestinationPort);
        buffer.push_back(mDestinationPort >> 8);

        buffer.push_back(mSequenceNumber);
        buffer.push_back(mSequenceNumber >> 8);
        buffer.push_back(mSequenceNumber >> 16);
        buffer.push_back(mSequenceNumber >> 24);

        buffer.push_back(mAckNumber);
        buffer.push_back(mAckNumber >> 8);
        buffer.push_back(mAckNumber >> 16);
        buffer.push_back(mAckNumber >> 24);

        buffer.push_back(mDataOffset);
        buffer.push_back(mFlags);

        buffer.push_back(mWindowSize);
        buffer.push_back(mWindowSize >> 8);

        buffer.push_back(mChecksum);
        buffer.push_back(mChecksum >> 8);

        buffer.push_back(mUrgentPointer);
        buffer.push_back(mUrgentPointer >> 8);

        buffer.insert(buffer.end(), mData.begin(), mData.end());

        return buffer;
    }

    std::unique_ptr<Data> TcpSegment::clone() const
    {
        return std::unique_ptr<Data> (new TcpSegment(*this));
    }

    void TcpSegment::setSourcePort(uint16_t port)
    {
        mSourcePort = htons(port);
    }

    uint16_t TcpSegment::getSourcePort() const
    {
        return (mSourcePort);
    }

    void TcpSegment::setDestinationPort(uint16_t port)
    {
        mDestinationPort = htons(port);
    }

    uint16_t TcpSegment::getDestinationPort() const
    {
        return (mDestinationPort);
    }

    void TcpSegment::setSequenceNumber(uint32_t sequenceNumber)
    {
        mSequenceNumber = htonl(sequenceNumber);
    }

    uint32_t TcpSegment::getSequenceNumber() const
    {
        return ntohl(mSequenceNumber);
    }

    void TcpSegment::setAckNumber(uint32_t ackNumber)
    {
        mAckNumber = htonl(ackNumber);
    }

    uint32_t TcpSegment::getAckNumber() const
    {
        return ntohl(mAckNumber);
    }

    void TcpSegment::setDataOffset(uint8_t dataOffset)
    {
        mDataOffset &= 0x0F;
        mDataOffset |= (dataOffset << 4);
    }

    uint8_t TcpSegment::getDataOffset() const
    {
        uint8_t dataOffset = mDataOffset;

        return ((dataOffset & 0xF0) >> 4);
    }

    void TcpSegment::setFlags(uint8_t flags, bool value)
    {
        if (value) {
            mFlags |= flags;
        } else {
            mFlags &= (flags ^ 0xFF);
        }
    }

    bool TcpSegment::getFlags(uint8_t flags) const
    {
        return (0 < (mFlags & flags));
    }

    void TcpSegment::setWindowSize(uint16_t windowSize)
    {
        mWindowSize = htons(windowSize);
    }

    uint16_t TcpSegment::getWindowSize() const
    {
        return ntohs(mWindowSize);
    }

    void TcpSegment::setChecksum(uint16_t checksum)
    {
        mChecksum = htons(checksum);
    }

    uint16_t TcpSegment::getChecksum() const
    {
        return ntohs(mChecksum);
    }

    void TcpSegment::setUrgentPointer(uint16_t urgentPointer)
    {
        mUrgentPointer = htons(urgentPointer);
    }

    uint16_t TcpSegment::getUrgentPointer() const
    {
        return ntohs(mUrgentPointer);
    }

    void TcpSegment::setData(std::string data)
    {
        mData = std::vector<uint8_t>(data.begin(), data.end());
    }

    std::string TcpSegment::getData() const
    {
        return std::string(mData.begin(), mData.end());
    }
}