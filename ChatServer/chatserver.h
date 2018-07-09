#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <map>
#include <memory>
#include "tcpserver.h"
#include "chatprotocol.h"

class ChatServer: public Interface::TcpObserver
{    
public:
    ChatServer(std::unique_ptr<Interface::TcpServer> server);
    void start(unsigned int port);
    void stop();

private:
    void clientConnected(void *_data);
    void clientDisconnected(void *_data);
    void messageReceived(void *_data);

    struct UserInfo
    {
        std::string username;
        bool identified;
    };

    std::map<size_t, UserInfo> users;

    std::unique_ptr<Interface::TcpServer> server;
};

#endif // CHATSERVER_H
