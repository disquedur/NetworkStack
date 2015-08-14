#include "IcmpTypeEchoRequest.h"
#include <iostream>

namespace NetworkStack
{
    IcmpTypeEchoRequest::IcmpTypeEchoRequest()
    {
    }

    IcmpTypeEchoRequest::IcmpTypeEchoRequest(const uint8_t* buffer, unsigned int size)
    {
        if (size < 8) {
            throw std::runtime_error("Echo request too small");
        }

        mIdentifier = 256U * buffer[4] + buffer[5];
        mSequenceNumber = 256U * buffer[6] + buffer[7];

        for (size_t i = 8; i < size; i++) {
            mData.push_back(buffer[i]);
        }
    }

    uint8_t IcmpTypeEchoRequest::getType() const
    {
        return TYPE;
    }

    std::vector<uint8_t> IcmpTypeEchoRequest::getRestOfHeader() const
    {
        std::vector<uint8_t> buffer;

        buffer.push_back(mIdentifier >> 8);
        buffer.push_back(mIdentifier & 0xFF);

        buffer.push_back(mSequenceNumber >> 8);
        buffer.push_back(mSequenceNumber & 0xFF);

        buffer.insert(buffer.end(), mData.begin(), mData.end());

        return buffer;
    }

    std::unique_ptr<IcmpType> IcmpTypeEchoRequest::clone() const
    {
        return std::unique_ptr<IcmpType> (new IcmpTypeEchoRequest(*this));
    }

    void IcmpTypeEchoRequest::setIdentifier(uint16_t identifier)
    {
        mIdentifier = identifier;
    }

    uint16_t IcmpTypeEchoRequest::getIdentifier() const
    {
        return mIdentifier;
    }

    void IcmpTypeEchoRequest::setSequenceNumber(uint16_t sequenceNumber)
    {
        mSequenceNumber = sequenceNumber;
    }

    uint16_t IcmpTypeEchoRequest::getSequenceNumber() const
    {
        return mSequenceNumber;
    }

    void IcmpTypeEchoRequest::setData(std::vector<uint8_t> data)
    {
        mData = data;
    }

    void IcmpTypeEchoRequest::setData(std::string data)
    {
        setData(std::vector<uint8_t>(data.begin(), data.end()));
    }

    std::string IcmpTypeEchoRequest::getData() const
    {
        return std::string(mData.begin(), mData.end());
    }
}