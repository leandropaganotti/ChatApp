#include "tcpserver.h"

TcpServer::TcpServer()
{
    running  = false;
    serverfd = 0;
    clientIDCounter = 0;
}

TcpServer::~TcpServer()
{
    stop();
}

bool TcpServer::start(unsigned int port)
{
    stop();

    logger << "starting server on port " << port << std::endl;

    // AF_INET=IPv4, SOCK_STREAM=2way,0=tcp
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if ( serverfd < 0)
    {
        logger << "failed to create server socket" << std::endl;
        return false;
    }

    struct sockaddr_in address;
    int addrlen = sizeof(address);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );

    if (bind(serverfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        logger << "failed to bind" << std::endl;
        return false;
    }

    if (listen(serverfd, 5) < 0)
    {
        logger << "failed to listen on port " << address.sin_port << std::endl;
        return false;
    }

    running = true;
    monitor = std::thread(&TcpServer::clientMonitor, this);
    logger << "waiting connections..." << std::endl;
    EventData data;
    while(running)
    {
        int clientfd;
        if ((clientfd = accept(serverfd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            logger << "error accepting new client connection" << std::endl;
        }
        else
        {
            char ip[32];
            inet_ntop(AF_INET, &(address.sin_addr.s_addr), ip, 32);
            m.lock();
            clients[++clientIDCounter] = {clientfd, address, true};
            m.unlock();
            logger << "new client connection: clientid=" << clientIDCounter << ", ip=" << ip << std::endl;
            data.clientid = clientIDCounter;
            for (auto it = observers.begin() ; it != observers.end(); ++it)
                (*it)->clientConnected(&data);
        }
    }
    return true;
}

void TcpServer::stop()
{
    logger << "stopping server..." << std::endl;

    running = false;

    if(monitor.joinable()) monitor.join();

    if(serverfd) close(serverfd);

    for (std::map<size_t, ClientData>::iterator it=clients.begin(); it!=clients.end();)
    {
        close(it->second.fd);
    }
}

void TcpServer::clientMonitor()
{
    logger << "monitoring connections..." << std::endl;
    fd_set clientfds;
    m.lock();
    while(running)
    {
        FD_ZERO(&clientfds);
        int maxfd = 0;
        for (std::map<size_t, ClientData>::iterator it=clients.begin(); it!=clients.end();)
        {
            if(it->second.fd > maxfd) maxfd = it->second.fd;

            if (it->second.connected) // if client is still connected
            {
                FD_SET( it->second.fd, &clientfds);
                ++it;
            }
            else
            {
                it = clients.erase(it);
            }
        }
        m.unlock();

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 1000 * 250;
        select( maxfd + 1 , &clientfds , NULL , NULL , &timeout);

        m.lock();
        EventData data;        
        for (std::map<size_t, ClientData>::iterator it=clients.begin(); it!=clients.end();++it)
        {
            data.clientid = it->first;

            int fd = it->second.fd;
            if (FD_ISSET( fd , &clientfds))
            {
                int n = 0;
                char buffer[1024]{0};

                char ip[32];
                inet_ntop(AF_INET, &(it->second.address.sin_addr.s_addr), ip, 32);

                if ((n = read( fd , buffer, 1023)) > 0)
                {
                    buffer[n] = '\0';
                    data.message = buffer;

                    logger << "data received: clientid=" << it->first << ", ip=" << ip << ", data=" << buffer << std::endl;
                    for (auto it = observers.begin() ; it != observers.end(); ++it)
                        (*it)->messageReceived(&data);
                }
                else
                {
                    logger << "client disconnected: clientid=" << it->first << ", ip=" << ip << std::endl;
                    it->second.connected = false;
                    close( fd );
                    for (auto it = observers.begin() ; it != observers.end(); ++it)
                        (*it)->clientDisconnected(&data);
                }
            }
        }
    }
}

void TcpServer::sendMessage(const char *msg, size_t clientid)
{
    if (clientid)
        send(clients[clientid].fd, msg, strlen(msg), 0 );
    else
        for (auto it = clients.begin() ; it != clients.end(); ++it)
            send(it->second.fd, msg , strlen(msg), 0 );

    //logger << "data sent: clientid=" << clientid << ", data=" << msg << std::endl;
}

void TcpServer::registerObserver(Interface::TcpObserver *observer)
{
    observers.push_back(observer);
}
