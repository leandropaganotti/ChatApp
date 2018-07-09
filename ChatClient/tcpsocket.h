#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <iostream>
#include <atomic>
#include <sstream>
#include <mutex>
#include <tcpobserverif.h>
#include <vector>

class TcpSocket
{
public:
    TcpSocket();
    bool conn(const char *host, unsigned int port);
    void discon();
    void registerObserver(Interface::TcpObserver *observer);
    void write(const char * msg);

private:
    int fd;
    std::thread th;
    std::atomic<bool> running;
    std::vector<Interface::TcpObserver*> observers;
    std::mutex m;
};


#endif // TCPSOCKET_H
