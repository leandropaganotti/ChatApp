#include "tcpsocket.h"

TcpSocket::TcpSocket()
{
    fd=0;
    running = false;
}

bool TcpSocket::conn(const char *host, unsigned int port)
{
    discon();

    struct sockaddr_in serv_addr;

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "socket creation error \n" << std::endl;
        return false;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, host, &serv_addr.sin_addr)<=0)
    {
        std::cerr << "invalid address" << std::endl;
        return false;
    }

    if (connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "connection failed" << std::endl;
        return false;
    }

    running = true;
    th = std::thread([this](){
        fd_set readfd;
        while(running)
        {
            FD_ZERO(&readfd);
            FD_SET(fd, &readfd);

            struct timeval timeout;
            timeout.tv_sec = 0;
            timeout.tv_usec = 1000 * 100;

            select( fd + 1 , &readfd, NULL , NULL , &timeout);

            if (FD_ISSET( fd , &readfd))
            {
                char buffer[1024] = {0};

                size_t n=0;
                m.lock();
                if ((n = read( fd , buffer, 1023)) > 0)
                {
                    m.unlock();
                    buffer[n] = '\0';
                    for (auto it = observers.begin() ; it != observers.end(); ++it)
                        (*it)->messageReceived(buffer);
                }
                else
                {
                    running = false;
                    close( fd );
                    for (auto it = observers.begin() ; it != observers.end(); ++it)
                        (*it)->clientDisconnected(NULL);
                }
            }
        }
    });

    for (auto it = observers.begin() ; it != observers.end(); ++it)
        (*it)->clientConnected(NULL);

    return true;
}

void TcpSocket::discon()
{
    running = false;
    if (th.joinable()) th.join();
    close(fd);
    fd=0;
}

void TcpSocket::registerObserver(Interface::TcpObserver *observer)
{
    observers.push_back(observer);
}

void TcpSocket::write(const char *msg)
{
    m.lock();
    send(fd , msg , strlen(msg) , 0 );
    m.unlock();
}
