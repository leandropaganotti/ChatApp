#include "chatclient.h"
#include "mainwindow.h"

ChatClient::ChatClient(QString host, int port, QString username)
{
    userid = 0;
    this->host=host;
    this->port = port;
    this->username = username;
}

ChatClient::~ChatClient()
{
    socket.discon();
}

bool ChatClient::connect()
{
    socket.registerObserver(this);
    return socket.conn(host.toStdString().c_str(), port);
}

void ChatClient::writeTo(const char *msg, size_t to)
{
    std::stringstream os;
    os << MSG << ":" << userid << ":" << to << ":" << msg;
    socket.write(os.str().c_str());
}

void ChatClient::messageReceived(void *data)
{
    std::string str((const char *)data);

    if (str.size() < 4 || str[3] != ':')
    {
        std::cerr << "invalid message received" << std::endl;
        return;
    }

    MessageType type = (MessageType)atoi(str.substr(0, 3).c_str());
    std::ostringstream os;

    switch(type)
    {
    case WHO:
    {
        os << IAM << ":" << username.toStdString();
        socket.write(os.str().c_str());
        break;
    }
    case YOUID:
    {
        std::string id = str.substr(4);
        userid = atoi(id.c_str());
        os << ALL << ":";
        socket.write(os.str().c_str());
        break;
    }
    case ALL:
        emit allUsers(QString((const char *)data));
        break;
    case MSG:
        emit newMessage(QString((const char *)data));
        break;
    case ADDED:
        emit newUser(QString((const char *)data));
        break;
    case RMVED:
        emit userRemoved(QString((const char *)data));
        break;
    default:
        std::cerr << "message type " << type << " does not heve a handler" << std::endl;
    }

}

void ChatClient::clientConnected(void *)
{
    std::cout << "client connected" << std::endl;
}

void ChatClient::clientDisconnected(void *)
{
    std::cout << "client disconnected" << std::endl;
}
