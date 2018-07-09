#ifndef TCPSERVERIF_H
#define TCPSERVERIF_H

#include "tcpobserverif.h"
#include <cstddef>

namespace Interface {

    class TcpServer
    {
    public:
        virtual ~TcpServer(){}
        virtual bool start(unsigned int port) = 0;
        virtual void stop() = 0;
        virtual void sendMessage(const char *msg, size_t clientid=0) = 0; // clientid = 0 => broadcast to all clients
        virtual void registerObserver(Interface::TcpObserver *observer) = 0;
    };

}  // namespace Interfaces

#endif // TCPSERVERIF_H
