#include "ArpPacket.h"

namespace NetworkStack
{
    ArpPacket::ArpPacket()
    {
        mHeader.hardwareType = ArpHardwareType::ETHERNET;
        mHeader.protocolType = ArpStackType::IPV4;
        mHeader.hardwareAddressLength = MacAddress::SIZE;
        mHeader.protocolAddressLength = IPv4Address::SIZE;
        mHeader.operation = ArpOperation::REQUEST;
    }

    ArpPacket::ArpPacket(const ArpPacket& arpPacket)
    {
        mHeader = arpPacket.mHeader;

        if (arpPacket.mSenderHardwareAddress) {
            mSenderHardwareAddress = arpPacket.mSenderHardwareAddress->clone();
        }

        if (arpPacket.mSenderInternetAddress) {
            mSenderInternetAddress= arpPacket.mSenderInternetAddress->clone();
        }

        if (arpPacket.mTargetHardwareAddress) {
            mTargetHardwareAddress = arpPacket.mTargetHardwareAddress->clone();
        }

        if (arpPacket.mTargetInternetAddress) {
            mTargetInternetAddress = arpPacket.mTargetInternetAddress->clone();
        }
    }

    ArpPacket::ArpPacket(const uint8_t* packet, unsigned int size)
    {
        if (size < HeaderSize) {
            throw std::runtime_error("Unable to generate ARP header.");
        }

        mHeader.hardwareType = 256U * packet[0] + packet[1];
        mHeader.protocolType = 256U * packet[2] + packet[3];
        mHeader.hardwareAddressLength = packet[4];
        mHeader.protocolAddressLength = packet[5];
        mHeader.operation = 256U * packet[6] + packet[7];

        int index = 8;

        if (size - index > mHeader.hardwareAddressLength) {
            if (mHeader.hardwareType == ArpHardwareType::ETHERNET) {
                mSenderHardwareAddress.reset(new MacAddress(packet + index, mHeader.hardwareAddressLength));
            }
        }

        index += mHeader.hardwareAddressLength;

        if (size - index > mHeader.protocolAddressLength) {
            if (mHeader.protocolType == ArpStackType::IPV4) {
                mSenderInternetAddress.reset(new IPv4Address(packet + index, mHeader.protocolAddressLength));
            }
        }

        index += mHeader.protocolAddressLength;

        if (size - index > mHeader.hardwareAddressLength) {
            if (mHeader.hardwareType == ArpHardwareType::ETHERNET) {
                mTargetHardwareAddress.reset(new MacAddress(packet + index, mHeader.hardwareAddressLength));
            }
        }

        index += mHeader.hardwareAddressLength;

        if (size - index > mHeader.protocolAddressLength) {
            if (mHeader.protocolType == ArpStackType::IPV4) {
                mTargetInternetAddress.reset(new IPv4Address(packet + index, mHeader.protocolAddressLength));
            }
        }
    }

    ArpPacket& ArpPacket::operator=(const ArpPacket& arpPacket)
    {
        if (this != &arpPacket) {
            mHeader = arpPacket.mHeader;

            if (arpPacket.mSenderHardwareAddress) {
                mSenderHardwareAddress = arpPacket.mSenderHardwareAddress->clone();
            }

            if (arpPacket.mSenderInternetAddress) {
                mSenderInternetAddress= arpPacket.mSenderInternetAddress->clone();
            }

            if (arpPacket.mTargetHardwareAddress) {
                mTargetHardwareAddress = arpPacket.mTargetHardwareAddress->clone();
            }

            if (arpPacket.mTargetInternetAddress) {
                mTargetInternetAddress = arpPacket.mTargetInternetAddress->clone();
            }
        }

        return *this;
    }

    std::vector<uint8_t> ArpPacket::build()
    {
        std::vector<uint8_t> buffer;

        buffer.push_back(mHeader.hardwareType >> 8);
        buffer.push_back(mHeader.hardwareType & 0x00FF);
        buffer.push_back(mHeader.protocolType >> 8);
        buffer.push_back(mHeader.protocolType & 0x00FF);
        buffer.push_back(mHeader.hardwareAddressLength);
        buffer.push_back(mHeader.protocolAddressLength);
        buffer.push_back(mHeader.operation >> 8);
        buffer.push_back(mHeader.operation & 0x00FF);

        if (!mSenderHardwareAddress) {
            throw std::runtime_error("Arp Packet: Sender Hardware Address is mandatory.");
        }

        std::vector<uint8_t> address = mSenderHardwareAddress->toVector();
        buffer.insert(buffer.end(), address.begin(), address.end());

        if (!mSenderInternetAddress) {
            throw std::runtime_error("Arp Packet: Sender Internet Address is mandatory.");
        }

        address = mSenderInternetAddress->toVector();
        buffer.insert(buffer.end(), address.begin(), address.end());


        if (mTargetHardwareAddress) {
            address = mTargetHardwareAddress->toVector();
            buffer.insert(buffer.end(), address.begin(), address.end());
        } else {
            for (int i = 0; i < mHeader.hardwareAddressLength; i++) {
                buffer.push_back(0);
            }
        }

        if (!mTargetInternetAddress) {
            throw std::runtime_error("Arp Packet: Target Internet Address is mandatory.");
        }

        address = mTargetInternetAddress->toVector();
        buffer.insert(buffer.end(), address.begin(), address.end());

        return buffer;
    }

    std::unique_ptr<Data> ArpPacket::clone() const
    {
        return std::unique_ptr<Data> (new ArpPacket(*this));
    }


    uint16_t ArpPacket::getHardwareType() const
    {
        return mHeader.hardwareType;
    }

    uint16_t ArpPacket::getProtocolType() const
    {
        return mHeader.protocolType;
    }

    uint8_t ArpPacket::getHardwareAddressLength() const
    {
        return mHeader.hardwareAddressLength;
    }

    uint8_t ArpPacket::getprotocolAddressLength() const
    {
        return mHeader.protocolAddressLength;
    }

    uint16_t ArpPacket::getOperation() const
    {
        return mHeader.operation;
    }

    void ArpPacket::setSenderHardwareAddress(const HardwareAddress& hardwareAddress)
    {
        mSenderHardwareAddress = hardwareAddress.clone();
    }

    HardwareAddress* ArpPacket::getSenderHardwareAddress() const
    {
        return mSenderHardwareAddress.get();
    }

    void ArpPacket::setSenderInternetAddress(const InternetAddress& internetAddress)
    {
        mSenderInternetAddress = internetAddress.clone();
    }

    InternetAddress* ArpPacket::getSenderInternetAddress() const
    {
        return mSenderInternetAddress.get();
    }

    void ArpPacket::setTargetHardwareAddress(const HardwareAddress& hardwareAddress)
    {
        mTargetHardwareAddress = hardwareAddress.clone();
    }

    HardwareAddress* ArpPacket::getTargetHardwareAddress() const
    {
        return mTargetHardwareAddress.get();
    }

    void ArpPacket::setTargetInternetAddress(const InternetAddress& internetAddress)
    {
        mTargetInternetAddress = internetAddress.clone();
    }

    InternetAddress* ArpPacket::getTargetInternetAddress() const
    {
        return mTargetInternetAddress.get();
    }
}