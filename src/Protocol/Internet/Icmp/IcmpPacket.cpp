#include "IcmpPacket.h"
#include <memory>

namespace NetworkStack
{
    IcmpPacket::IcmpPacket()
    {
    }

    IcmpPacket::IcmpPacket(const IcmpPacket& icmpPacket)
    {
        mChecksum = icmpPacket.mChecksum;

        if (icmpPacket.mType) {
            mType = icmpPacket.mType->clone();
        }
    }

    IcmpPacket::IcmpPacket(const uint8_t* buffer, unsigned int size)
    {
        uint8_t type = buffer[0];

        if (type == IcmpTypeEchoRequest::TYPE) {
            mType = std::unique_ptr<IcmpTypeEchoRequest>(new IcmpTypeEchoRequest(buffer, size));
        } else if (type == IcmpTypeEchoReply::TYPE) {
            mType = std::unique_ptr<IcmpTypeEchoReply>(new IcmpTypeEchoReply(buffer, size));
        }

        mChecksum = 254U * buffer[2] + buffer[3];
    }

    IcmpPacket& IcmpPacket::operator=(const IcmpPacket& icmpPacket)
    {
        if (this != &icmpPacket) {
            mChecksum = icmpPacket.mChecksum;

            if (icmpPacket.mType) {
                mType = icmpPacket.mType->clone();
            }
        }

        return *this;
    }

    void IcmpPacket::calculateChecksum()
    {
        setChecksum(0);
        std::vector<uint8_t> buffer = buildWithoutCalculateChecksum();

        uint32_t checksum = 0;

        if (buffer.size() % 2) {
            buffer.push_back(0);
        }

        for (size_t i = 0; i < buffer.size(); i += 2) {
            checksum += 256U * buffer[i] + buffer[i + 1];
        }

        while (checksum >> 16) {
            checksum = (checksum & 0xFFFF) + (checksum >> 16);
        }

        setChecksum(static_cast<uint16_t> (~checksum));
    }

    std::vector<uint8_t> IcmpPacket::buildWithoutCalculateChecksum()
    {
        if (!mType) {
            throw std::runtime_error("No Type set in IcmpPacket.");
        }

        std::vector<uint8_t> buffer;

        buffer.push_back(mType->getType());
        buffer.push_back(mType->getCode());

        buffer.push_back(mChecksum >> 8);
        buffer.push_back(mChecksum & 0xFF);

        std::vector<uint8_t> restOfHeader = mType->getRestOfHeader();
        buffer.insert(buffer.end(), restOfHeader.begin(), restOfHeader.end());

        return buffer;
    }

    std::vector<uint8_t> IcmpPacket::build()
    {
        calculateChecksum();

        return buildWithoutCalculateChecksum();
    }

    std::unique_ptr<Data> IcmpPacket::clone() const
    {
        return std::unique_ptr<Data> (new IcmpPacket(*this));
    }

    void IcmpPacket::setChecksum(uint16_t checksum)
    {
        mChecksum = checksum;
    }

    uint16_t IcmpPacket::getChecksum() const
    {
        return ntohs(mChecksum);
    }

    void IcmpPacket::setType(const IcmpType& type)
    {
        mType = type.clone();
    }

    IcmpType* IcmpPacket::getType() const
    {
        return mType.get();
    }
}