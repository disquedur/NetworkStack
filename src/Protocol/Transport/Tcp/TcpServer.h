#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <map>
#include <string>
#include <ctime>
#include <cmath>

#include "../../AbstractProtocol.h"
#include "../../Link/Ethernet/EthernetProtocol.h"
#include "TcpConnection.h"

namespace NetworkStack
{
    class TcpServer : AbstractProtocol
    {
        using AbstractProtocol::AbstractProtocol;

        public:
            TcpMessage listen(int port);
            void send(TcpMessage);

        private:
            std::map<std::string, TcpClientSession> mConnections;
            int mPort;
    };
}

#endif