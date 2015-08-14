#ifndef ABSTRACT_DEVICE_H
#define ABSTRACT_DEVICE_H

#include <exception>
#include "../Protocol/Internet/IPv4/IPv4Address.h"
#include "../Protocol/Link/Ethernet/MacAddress.h"
#include "../Protocol/Link/Ethernet/EthernetFrame.h"

namespace NetworkStack
{
    class AbstractDevice
    {
        public:
            virtual ~AbstractDevice() {};

            virtual std::string getName() = 0;
            virtual void sendEthernet(EthernetFrame packet) = 0;
            virtual EthernetFrame getNextEthernet(const std::string payloadType) = 0;
            virtual IPv4Address getNetworkAddress() const = 0;
            virtual IPv4Address getMask() const = 0;
            virtual MacAddress getMac() const = 0;
            virtual IPv4Address getIP() const = 0;
            virtual IPv4Address getGatway() const = 0;
    };
}

#endif