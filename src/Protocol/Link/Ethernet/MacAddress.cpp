#include "MacAddress.h"

namespace NetworkStack
{
    MacAddress::MacAddress() : mAddress(std::vector<uint8_t>(SIZE, 0))
    {
    }

    MacAddress::MacAddress(const std::vector<uint8_t> address) : mAddress(address)
    {
        if (mAddress.size() != SIZE) {
            throw std::runtime_error("Bad MAC Address length.");
        }
    }

    MacAddress::MacAddress(const uint8_t* address, int size) : MacAddress()
    {
        if (size != SIZE) {
            throw std::runtime_error("MAC Address too large.");
        }

        for (int i = 0; i < size; i++) {
            mAddress[i] = address[i];
        }
    }

    MacAddress::MacAddress(const std::string& address) : MacAddress()
    {
        unsigned int bytes[6];

        if (std::sscanf(address.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x",
            &bytes[0], &bytes[1], &bytes[2],
            &bytes[3], &bytes[4], &bytes[5]) != 6)
        {
            throw std::runtime_error("Bad MAC Address.");
        }

        mAddress.assign(&bytes[0], &bytes[6]);
    }

    int MacAddress::getSize() const
    {
        return SIZE;
    }

    std::string MacAddress::toString() const
    {
        std::stringstream stream;
        stream.setf(std::ios::hex, std::ios::basefield);

        for (int i = 0; i < SIZE; i++) {
            stream << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(mAddress[i]);

            if (i < 5) {
                stream << ":";
            }
        }

        return stream.str();
    }

    std::vector<uint8_t> MacAddress::toVector() const
    {
        return mAddress;
    }

    std::unique_ptr<HardwareAddress> MacAddress::clone() const
    {
        return std::unique_ptr<HardwareAddress> (new MacAddress(*this));
    }
}