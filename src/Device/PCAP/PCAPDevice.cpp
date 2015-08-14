#include "PCAPDevice.h"
#include <iostream>

namespace NetworkStack
{
    PCAPDevice::PCAPDevice(std::string name) : mName(name)
    {
        char errorBuffer[PCAP_ERRBUF_SIZE];

        if (mName.empty()) {
            mName.assign(pcap_lookupdev(errorBuffer));

            if (mName.empty()) {
                throw std::runtime_error(errorBuffer);
            }
        }

        initialize();
    }

    void PCAPDevice::initialize()
    {
        char errorBuffer[PCAP_ERRBUF_SIZE];

        if (mHandler != nullptr) {
            pcap_close(mHandler);
        }

        mHandler = pcap_open_live(mName.c_str(), 65536, 0, 250, errorBuffer);
        if (mHandler == nullptr) {
            throw std::runtime_error(errorBuffer);
        }

        if (pcap_lookupnet(mName.c_str(), &mAddress, &mMask, errorBuffer) == -1) {
            throw std::runtime_error(errorBuffer);
        }
    }

    std::string PCAPDevice::getName()
    {
        return mName;
    }

    void PCAPDevice::sendEthernet(EthernetFrame packet)
    {
        std::vector<uint8_t> buffer = packet.build();
        if (pcap_inject(mHandler, buffer.data(), buffer.size()) == -1) {
            throw std::runtime_error("Error injecting Ethernet packet.");
        }
    }

    EthernetFrame PCAPDevice::getNextEthernet(const std::string payloadType)
    {
        struct bpf_program filter;
        struct pcap_pkthdr* pkthdr;
        const uint8_t* frame;

        if (mPayloadType != payloadType) {
            if (pcap_compile(mHandler, &filter, payloadType.c_str(), 1, mMask) == -1) {
                throw std::runtime_error("Error compiling filter.");
            }

            if (pcap_setfilter(mHandler, &filter) == -1) {
                throw std::runtime_error("Error loading filter.");
            }

            mPayloadType = payloadType;
        }

        while (!(pcap_next_ex(mHandler, &pkthdr, &frame)));

        //pcap_next_ex(mHandler, &pkthdr, &frame);

        return EthernetFrame(frame, pkthdr->len);
    }

    IPv4Address PCAPDevice::getNetworkAddress() const
    {
        return IPv4Address(mAddress);
    }

    IPv4Address PCAPDevice::getMask() const
    {
        return IPv4Address(mMask);
    }

    // No way to get it with pcap
    MacAddress PCAPDevice::getMac() const
    {
        int fd, returnValue;
        struct ifreq ifr;
        size_t if_name_len = mName.size();

        memcpy(ifr.ifr_name, mName.c_str(), if_name_len);
        ifr.ifr_name[if_name_len] = 0;

        fd = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (fd == -1) {
            throw std::runtime_error("Error getting MAC.");
        }

        returnValue = ioctl(fd, SIOCGIFHWADDR, &ifr);
        close(fd);

        if (returnValue == -1) {
            throw std::runtime_error("Error getting MAC.");
        }

        if (ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER) {
            throw std::runtime_error("Not an Ethernet interface.");
        }

        return MacAddress(reinterpret_cast<uint8_t*> (ifr.ifr_hwaddr.sa_data), 6);
    }

    IPv4Address PCAPDevice::getIP() const
    {
        char errorBuffer[PCAP_ERRBUF_SIZE];
        pcap_if_t *alldevs;
        int status = pcap_findalldevs(&alldevs, errorBuffer);

        if (status != 0) {
            throw std::runtime_error("Error getting IP.");
        }

        for (pcap_if_t* device = alldevs; device != NULL; device = device->next) {
            if (strcmp(device->name, mName.c_str()) == 0) {
                for (pcap_addr_t* address = device->addresses; address != NULL; address = address->next) {
                    if (address->addr->sa_family == AF_INET) {
                        pcap_freealldevs(alldevs);

                        return IPv4Address(inet_ntoa(reinterpret_cast<struct sockaddr_in*> (address->addr)->sin_addr));
                    }
                }
            }
        }

        throw std::runtime_error("No IP address found.");
    }

    IPv4Address PCAPDevice::getGatway() const
    {
        char* gateway = NULL;

        FILE* fp = popen("netstat -rn", "r");
        char line[256] = {0x0};

        while (fgets(line, sizeof(line), fp) != NULL) {
            char* destination;
            destination = strndup(line, 15);

            char* iface;
            iface = strndup(line + 73, 4);

            if (strcmp("0.0.0.0        ", destination) == 0 && strcmp(iface, mName.c_str()) == 0) {
                gateway = strndup(line + 16, 15);
            }

            free(destination);
            free(iface);
        }

        pclose(fp);

        if (gateway == nullptr) {
            return IPv4Address("0.0.0.0");
        }

        return IPv4Address(gateway);
    }
}