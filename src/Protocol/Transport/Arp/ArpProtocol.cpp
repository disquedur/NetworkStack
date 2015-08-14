#include "ArpProtocol.h"
#include <iostream>
#include <thread>
#include <future>
#include <unistd.h>

namespace NetworkStack
{
    MacAddress ArpProtocol::resolveMac(IPv4Address ip)
    {
        if (mTable.count(ip.toString()) == 0) {
            ArpPacket arpRequest;
            arpRequest.setSenderHardwareAddress(mDevice->getMac());
            arpRequest.setSenderInternetAddress(mDevice->getIP());
            arpRequest.setTargetInternetAddress(ip);

            EthernetFrame frameRequest(mDevice->getMac(), MacAddress("FF:FF:FF:FF:FF:FF"));
            frameRequest.setPayload(arpRequest);

            auto response = std::async(waitResponse, ip, mDevice);
            mDevice->sendEthernet(frameRequest);

            return mTable[ip.toString()] = response.get();
        }

        return mTable[ip.toString()];
    }

    MacAddress ArpProtocol::waitResponse(IPv4Address ip, std::shared_ptr<AbstractDevice> device)
    {
        for (int tries = 5; tries > 0; tries--) {
            EthernetFrame frameResponse = device->getNextEthernet("arp src " + ip.toString()
                                        + " and arp dst " + device->getIP().toString());

            ArpPacket* arpPacket = dynamic_cast<ArpPacket*> (frameResponse.getPayload());

            if (arpPacket->getOperation() == ArpOperation::RESPONSE
                && arpPacket->getHardwareType() == ArpHardwareType::ETHERNET) {

                return *(dynamic_cast<MacAddress*> (arpPacket->getSenderHardwareAddress()));
            }
        }

        throw std::runtime_error("Unable to resolve Mac Address (no arp response).");
    }
}