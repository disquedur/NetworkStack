#include "IPv4Packet.h"

namespace NetworkStack
{
    IPv4Packet::IPv4Packet(const IPv4Packet& ipv4Packet)
    {
        mVersionAndHeaderLength = ipv4Packet.mVersionAndHeaderLength;
        mDifferentiatedServices = ipv4Packet.mDifferentiatedServices;
        mTotalLength = ipv4Packet.mTotalLength;
        mIdentification = ipv4Packet.mIdentification;
        mFlagsAndFragmentOffset = ipv4Packet.mFlagsAndFragmentOffset;
        mTimeToLive = ipv4Packet.mTimeToLive;
        mProtocol = ipv4Packet.mProtocol;
        mChecksum = ipv4Packet.mChecksum;
        mSource = ipv4Packet.mSource;
        mDestination = ipv4Packet.mDestination;

        if (ipv4Packet.mData) {
            mData = ipv4Packet.mData->clone();
        }
    }

    IPv4Packet::IPv4Packet(IPv4Address source, IPv4Address destination)
    {
        setVersion(4);
        setHeaderLength(5);
        setDifferentiatedServices(0);
        setIdentification(0);
        setFlags(IPv4Flag::DF | IPv4Flag::MF, false); // set all flags to false
        setFragmentOffset(0);
        setTimeToLive(64);
        setProtocol(IPv4Protocol::TCP);
        setChecksum(0);

        setSourceAddress(source);
        setDestinationAddress(destination);
    }

    IPv4Packet::IPv4Packet(const uint8_t* buffer, unsigned int size)
    {
        if (size < HeaderSize) {
            throw std::runtime_error("Unable to generate IPv4 header.");
        }

        mVersionAndHeaderLength = buffer[0];
        mDifferentiatedServices = buffer[1];
        mTotalLength = 256U * buffer[2] + buffer[3];
        mIdentification = 256U * buffer[4] + buffer[5];
        mFlagsAndFragmentOffset = 256U * buffer[6] + buffer[7];
        mTimeToLive = buffer[8];
        mProtocol = buffer[9];
        mChecksum = 256U * buffer[10] + buffer[11];
        mSource = IPv4Address(buffer + 12, IPv4Address::SIZE);
        mDestination = IPv4Address(buffer + 16, IPv4Address::SIZE);;

        buffer += HeaderSize;
        size -= HeaderSize;

        if (mProtocol == IPv4Protocol::TCP) {
            mData.reset(new TcpSegment(buffer, size));
        } else if (mProtocol == IPv4Protocol::ICMP) {
            mData.reset(new IcmpPacket(buffer, size));
        }
    }

    IPv4Packet& IPv4Packet::operator=(const IPv4Packet &ipv4Packet)
    {
        if (this != &ipv4Packet) {
            mVersionAndHeaderLength = ipv4Packet.mVersionAndHeaderLength;
            mDifferentiatedServices = ipv4Packet.mDifferentiatedServices;
            mTotalLength = ipv4Packet.mTotalLength;
            mIdentification = ipv4Packet.mIdentification;
            mFlagsAndFragmentOffset = ipv4Packet.mFlagsAndFragmentOffset;
            mTimeToLive = ipv4Packet.mTimeToLive;
            mProtocol = ipv4Packet.mProtocol;
            mChecksum = ipv4Packet.mChecksum;
            mSource = ipv4Packet.mSource;
            mDestination = ipv4Packet.mDestination;

            if (ipv4Packet.mData) {
                mData = ipv4Packet.mData->clone();
            }
        }

        return *this;
    }

    void IPv4Packet::calculateChecksum()
    {
        setChecksum(0);

        auto headerBuffer = buildHeader();

        uint32_t checksum = 0;
        for (size_t i = 0; i < HeaderSize; i += 2) {
            checksum += 256U * headerBuffer[i] + headerBuffer[i + 1];
        }

        while (checksum >> 16) {
            checksum = (checksum & 0xFFFF) + (checksum >> 16);
        }

        setChecksum(static_cast<uint16_t> (~checksum));
    }

    std::vector<uint8_t> IPv4Packet::buildHeader()
    {
        std::vector<uint8_t> buffer;

        buffer.push_back(mVersionAndHeaderLength);
        buffer.push_back(mDifferentiatedServices);

        buffer.push_back(mTotalLength);
        buffer.push_back(mTotalLength >> 8);

        buffer.push_back(mIdentification);
        buffer.push_back(mIdentification >> 8);

        buffer.push_back(mFlagsAndFragmentOffset);
        buffer.push_back(mFlagsAndFragmentOffset >> 8);

        buffer.push_back(mTimeToLive);
        buffer.push_back(mProtocol);

        buffer.push_back(mChecksum);
        buffer.push_back(mChecksum >> 8);

        auto sourceBuffer = mSource.toVector();
        buffer.insert(buffer.end(), sourceBuffer.begin(), sourceBuffer.end());

        auto destinationBuffer = mDestination.toVector();
        buffer.insert(buffer.end(), destinationBuffer.begin(), destinationBuffer.end());

        return buffer;
    }

    std::vector<uint8_t> IPv4Packet::build()
    {
        if (!mData) {
            throw std::runtime_error("Unable to generate packet without data.");
        }

        if (TcpSegment* data = dynamic_cast<TcpSegment*> (mData.get())) {
            data->calculateChecksum(this);
        }

        std::vector<uint8_t> dataBuffer = mData->build();
        setTotalLength(HeaderSize + dataBuffer.size());

        calculateChecksum();

        auto buffer = buildHeader();
        buffer.insert(buffer.end(), dataBuffer.begin(), dataBuffer.end());

        return buffer;
    }

    std::unique_ptr<Data> IPv4Packet::clone() const
    {
        return std::unique_ptr<Data> (new IPv4Packet(*this));
    }

    void IPv4Packet::setVersion(uint8_t version)
    {
        mVersionAndHeaderLength &= 0x0F;
        mVersionAndHeaderLength |= version << 4;
    }

    uint8_t IPv4Packet::getVersion() const
    {
        return mVersionAndHeaderLength >> 4;
    }

    void IPv4Packet::setHeaderLength(uint8_t headerLength)
    {
        mVersionAndHeaderLength &= 0xF0;
        mVersionAndHeaderLength |= headerLength & 0x0F;
    }

    uint8_t IPv4Packet::getHeaderLength() const
    {
        return mVersionAndHeaderLength & 0x0F;
    }

    void IPv4Packet::setDifferentiatedServices(uint8_t differentiatedServices)
    {
        mDifferentiatedServices = differentiatedServices;
    }

    uint8_t IPv4Packet::getDifferentiatedServices() const
    {
        return mDifferentiatedServices;
    }

    void IPv4Packet::setTotalLength(uint16_t totalLength)
    {
        mTotalLength = htons(totalLength);
    }

    uint16_t IPv4Packet::getTotalLength() const
    {
        return ntohs(mTotalLength);
    }

    void IPv4Packet::setIdentification(uint16_t identification)
    {
        mIdentification = htons(identification);
    }

    uint16_t IPv4Packet::getIdentification() const
    {
        return ntohs(mIdentification);
    }

    void IPv4Packet::setFlags(uint16_t flag, bool value)
    {
        mFlagsAndFragmentOffset &= htons(0x7FFF - flag);

        if (value) {
            mFlagsAndFragmentOffset |= htons(flag);
        } else {
            mFlagsAndFragmentOffset &= htons(0x7FFF - flag);
        }
    }

    bool IPv4Packet::getFlags(uint16_t flag) const
    {
        return (0 < (ntohs(mFlagsAndFragmentOffset) & flag));
    }

    void IPv4Packet::setFragmentOffset(uint16_t fragmentOffset)
    {
        if (fragmentOffset & 0x6000) {
            throw std::runtime_error("Fragment Offset too large.");
        }

        mFlagsAndFragmentOffset &= htons(0xE000);
        mFlagsAndFragmentOffset |= htons(fragmentOffset & 0x1FFF);
    }

    uint16_t IPv4Packet::getFragmentOffset() const
    {
        return ntohs(mFlagsAndFragmentOffset) & 0x1FFF;
    }

    void IPv4Packet::setTimeToLive(uint8_t timeToLive)
    {
        mTimeToLive = timeToLive;
    }

    uint8_t IPv4Packet::getTimeToLive() const
    {
        return mTimeToLive;
    }

    void IPv4Packet::setProtocol(uint8_t protocol)
    {
        mProtocol = protocol;
    }

    uint8_t IPv4Packet::getProtocol() const
    {
        return mProtocol;
    }

    void IPv4Packet::setChecksum(uint16_t checksum)
    {
        mChecksum = htons(checksum);
    }

    uint16_t IPv4Packet::getChecksum() const
    {
        return ntohs(mChecksum);
    }

    void IPv4Packet::setSourceAddress(const IPv4Address& source)
    {
        mSource = source;
    }

    IPv4Address IPv4Packet::getSourceAddress() const
    {
        return mSource;
    }

    void IPv4Packet::setDestinationAddress(const IPv4Address& destination)
    {
        mDestination = destination;
    }

    IPv4Address IPv4Packet::getDestinationAddress() const
    {
        return mDestination;
    }

    void IPv4Packet::setData(const Data& data)
    {
        mData = data.clone();

        if (dynamic_cast<TcpSegment*> (mData.get())) {
            setProtocol(IPv4Protocol::TCP);
        }
    }

    Data* IPv4Packet::getData() const
    {
        return mData.get();
    }
}