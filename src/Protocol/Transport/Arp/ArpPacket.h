#ifndef ARP_PACKET_H
#define ARP_PACKET_H

#include <stdint.h>
#include <stdexcept>

#include "../../Data.h"
#include "../../Link/HardwareAddress.h"
#include "../../Internet/InternetAddress.h"
#include "../../Internet/IPv4/IPv4Address.h"
#include "../../Link/Ethernet/MacAddress.h"

namespace NetworkStack
{
    namespace ArpHardwareType
    {
        const uint16_t ETHERNET = 0x0001;
    }

    namespace ArpStackType
    {
        const uint16_t IPV4 = 0x0800;
    }

    namespace ArpOperation
    {
        const uint16_t REQUEST = 1;
        const uint16_t RESPONSE = 2;
    }

    typedef struct ArpHeader {
        uint16_t hardwareType;
        uint16_t protocolType;
        uint8_t hardwareAddressLength;
        uint8_t protocolAddressLength;
        uint16_t operation;
    } ArpHeader;

    class ArpPacket : public Data
    {
        public:
            static const unsigned int HeaderSize = 8;

            ArpPacket();
            ArpPacket(const ArpPacket&);
            ArpPacket(const uint8_t* packet, unsigned int size);

            ArpPacket& operator=(const ArpPacket&);

            virtual std::vector<uint8_t> build();
            virtual std::unique_ptr<Data> clone() const;

            // Getters Setters
            uint16_t getHardwareType() const;
            uint16_t getProtocolType() const;
            uint8_t getHardwareAddressLength() const;
            uint8_t getprotocolAddressLength() const;
            uint16_t getOperation() const;
            void setSenderHardwareAddress(const HardwareAddress& hardwareAddress);
            HardwareAddress* getSenderHardwareAddress() const;
            void setSenderInternetAddress(const InternetAddress& internetAddress);
            InternetAddress* getSenderInternetAddress() const;
            void setTargetHardwareAddress(const HardwareAddress& hardwareAddress);
            HardwareAddress* getTargetHardwareAddress() const;
            void setTargetInternetAddress(const InternetAddress& internetAddress);
            InternetAddress* getTargetInternetAddress() const;

        private:
            ArpHeader mHeader;
            std::unique_ptr<HardwareAddress> mSenderHardwareAddress;
            std::unique_ptr<InternetAddress> mSenderInternetAddress;
            std::unique_ptr<HardwareAddress> mTargetHardwareAddress;
            std::unique_ptr<InternetAddress> mTargetInternetAddress;
    };
}

#endif