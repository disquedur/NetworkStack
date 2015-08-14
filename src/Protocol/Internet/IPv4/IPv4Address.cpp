#include "IPv4Address.h"

namespace NetworkStack
{
    IPv4Address::IPv4Address()
    {
    }

    IPv4Address::IPv4Address(const uint32_t address)
    {
        mAddress = address;
    }

    IPv4Address::IPv4Address(const uint8_t* address, int size)
    {
        if (size != SIZE) {
            throw std::runtime_error("Bad IP address.");
        }

        mAddress = address[0] | (address[1] << 8) | (address[2] << 16) | (address[3] << 24);
    }

    IPv4Address::IPv4Address(const std::vector<uint8_t> address)
    {
        if (address.size() != SIZE) {
            throw std::runtime_error("Bad IP address.");
        }

        mAddress = address[0] | (address[1] << 8) | (address[2] << 16) | (address[3] << 24);
    }

    IPv4Address::IPv4Address(const std::string address)
    {
        if (!inet_aton(address.c_str(), reinterpret_cast<struct in_addr*> (&mAddress))) {
            throw std::runtime_error("Bad IP address.");
        }
    }

    bool IPv4Address::isInRange(IPv4Address network, IPv4Address mask)
    {
        uint32_t networkLower = network.toInt() & mask.toInt();
        uint32_t networkUpper = networkLower | (~mask.toInt());

        return (mAddress >= networkLower && mAddress <= networkUpper);
    }

    uint32_t IPv4Address::toInt() const
    {
        return mAddress;
    }

    int IPv4Address::getSize() const
    {
        return SIZE;
    }

    std::string IPv4Address::toString() const
    {
        std::stringstream stream;

        stream << static_cast<uint16_t>(mAddress & 0xFF) << ".";
        stream << static_cast<uint16_t>((mAddress >> 8) & 0xFF) << ".";
        stream << static_cast<uint16_t>((mAddress >> 16) & 0xFF) << ".";
        stream << static_cast<uint16_t>((mAddress >> 24) & 0xFF);

        return stream.str();
    }

    std::vector<uint8_t> IPv4Address::toVector() const
    {
        std::vector<uint8_t> address(4, 0);

        address[0] = mAddress & 0xFF;
        address[1] = (mAddress >> 8) & 0xFF;
        address[2] = (mAddress >> 16) & 0xFF;
        address[3] = (mAddress >> 24) & 0xFF;

        return address;
    }

    std::unique_ptr<InternetAddress> IPv4Address::clone() const
    {
        return std::unique_ptr<InternetAddress> (new IPv4Address(*this));
    }
}