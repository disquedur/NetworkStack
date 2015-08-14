#ifndef ABSTRACT_PROTOCOL_H
#define ABSTRACT_PROTOCOL_H

#include <memory>
#include "../Device/AbstractDevice.h"

namespace NetworkStack
{
    class AbstractProtocol
    {
        public:
            AbstractProtocol(std::shared_ptr<AbstractDevice> device);
            virtual ~AbstractProtocol() {};

        protected:
            std::shared_ptr<AbstractDevice> mDevice;
    };
}

#endif