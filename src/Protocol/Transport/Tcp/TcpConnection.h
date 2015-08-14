#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

namespace NetworkStack
{
    namespace TcpConnectionState
    {
        const int SYN_RECEIVED = 1;
        const int ESTABLISHED = 2;
        const int FIN_RECEIVED = 3;
        const int CLOSED = 4;
    }

    struct TcpSessionSegment
    {
        std::string data;
        bool acknowledged;
    };

    struct TcpClientSession
    {
        int state;
        int port;
        IPv4Address address;
        uint16_t window;
        uint32_t clientSequenceNumber;
        uint32_t serverSequenceNumber;
        std::map<int, TcpSessionSegment> segments;
    };

    struct TcpMessage
    {
        std::string client;
        std::string value;
    };
}

#endif