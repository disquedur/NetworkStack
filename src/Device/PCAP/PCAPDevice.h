#ifndef PCAP_DEVICE_H
#define PCAP_DEVICE_H

#include <pcap.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <thread>

#include "../AbstractDevice.h"

namespace NetworkStack
{
    class PCAPDevice : public AbstractDevice
    {
        public:
            PCAPDevice(std::string name = std::string());
            void initialize();

            std::string getName();
            void sendEthernet(EthernetFrame packet);
            EthernetFrame getNextEthernet(const std::string payloadType);
            IPv4Address getNetworkAddress() const;
            IPv4Address getMask() const;
            MacAddress getMac() const;
            IPv4Address getIP() const;
            IPv4Address getGatway() const;

        private:
            PCAPDevice(const PCAPDevice&);
            PCAPDevice& operator=(const PCAPDevice&);

            std::string mName;
            pcap_t* mHandler = nullptr;
            bpf_u_int32 mAddress = 0, mMask = 0;

            std::string mPayloadType;
    };
}

#endif