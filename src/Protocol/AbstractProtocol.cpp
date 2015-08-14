#include "AbstractProtocol.h"
#include "../Device/PCAP/PCAPDevice.h"

namespace NetworkStack
{
    AbstractProtocol::AbstractProtocol(std::shared_ptr<AbstractDevice> device)
//        : mDevice(std::move(device))
    {
        mDevice = std::make_shared<PCAPDevice>(device->getName());
    }
}