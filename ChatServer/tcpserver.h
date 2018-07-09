#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <csignal>
#include <atomic>
#include <thread>
#include <mutex>
#include <map>
#include <vector>

#include "tcpserverif.h"
#include "logger.h"
#include "eventdata.h"

class TcpServer: public Interface::TcpServer
{
    struct ClientData
    {
        int fd;
        struct sockaddr_in address;
        bool connected;
    };
public:
    TcpServer();
    ~TcpServer();
    bool start(unsigned int port) override;
    void stop() override;
    void sendMessage(const char *msg, size_t clientIDCounter=0) override;
    void registerObserver(Interface::TcpObserver *observer) override;

private:
    std::atomic_bool running;
    int serverfd;
    std::thread monitor;
    std::mutex m;
    std::map<size_t, ClientData> clients;
    size_t clientIDCounter;
    std::vector<Interface::TcpObserver*> observers;

    void clientMonitor();
};

#endif // TCPSERVER_H
