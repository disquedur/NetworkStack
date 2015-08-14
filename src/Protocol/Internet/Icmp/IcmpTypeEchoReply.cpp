#include "IcmpTypeEchoReply.h"
#include <iostream>

namespace NetworkStack
{
    IcmpTypeEchoReply::IcmpTypeEchoReply()
    {
    }

    IcmpTypeEchoReply::IcmpTypeEchoReply(IcmpTypeEchoRequest request)
    {
        mIdentifier = request.getIdentifier();
        mSequenceNumber = request.getSequenceNumber();
        setData(request.getData());
    }

    IcmpTypeEchoReply::IcmpTypeEchoReply(const uint8_t* buffer, unsigned int size)
    {
        if (size < 8) {
            throw std::runtime_error("Echo reply too small");
        }

        mIdentifier = 254U * buffer[4] + buffer[5];
        mSequenceNumber = 254 * buffer[6] + buffer[7];

        for (size_t i = 8; i < size; i++) {
            mData.push_back(buffer[i]);
        }
    }

    uint8_t IcmpTypeEchoReply::getType() const
    {
        return TYPE;
    }

    std::vector<uint8_t> IcmpTypeEchoReply::getRestOfHeader() const
    {
        std::vector<uint8_t> buffer;

        buffer.push_back(mIdentifier >> 8);
        buffer.push_back(mIdentifier & 0xFF);

        buffer.push_back(mSequenceNumber >> 8);
        buffer.push_back(mSequenceNumber & 0xFF);

        buffer.insert(buffer.end(), mData.begin(), mData.end());

        return buffer;
    }

    std::unique_ptr<IcmpType> IcmpTypeEchoReply::clone() const
    {
        return std::unique_ptr<IcmpType> (new IcmpTypeEchoReply(*this));
    }

    void IcmpTypeEchoReply::setIdentifier(uint16_t identifier)
    {
        mIdentifier = identifier;
    }

    uint16_t IcmpTypeEchoReply::getIdentifier() const
    {
        return mIdentifier;
    }

    void IcmpTypeEchoReply::setSequenceNumber(uint16_t sequenceNumber)
    {
        mSequenceNumber = sequenceNumber;
    }

    uint16_t IcmpTypeEchoReply::getSequenceNumber() const
    {
        return mSequenceNumber;
    }

    void IcmpTypeEchoReply::setData(std::vector<uint8_t> data)
    {
        mData = data;
    }

    void IcmpTypeEchoReply::setData(std::string data)
    {
        setData(std::vector<uint8_t>(data.begin(), data.end()));
    }

    std::string IcmpTypeEchoReply::getData() const
    {
        return std::string(mData.begin(), mData.end());
    }
}