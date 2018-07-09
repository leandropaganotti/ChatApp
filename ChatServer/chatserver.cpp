#include "logger.h"
#include "chatserver.h"
#include "eventdata.h"
#include <sstream>
#include "utils.h"

ChatServer::ChatServer(std::unique_ptr<Interface::TcpServer> server): server(std::move(server))
{
    this->server->registerObserver(this);
}

void ChatServer::start(unsigned int port)
{
    server->start(port);
}

void ChatServer::stop()
{
    server->stop();
}

void ChatServer::clientConnected(void *_data)
{
    EventData *data = (EventData*)_data;

    users[data->clientid] = {"", false};
    std::ostringstream os;
    os << WHO << ":" << std::endl;
    server->sendMessage(os.str().c_str(), data->clientid);
}

void ChatServer::clientDisconnected(void *_data)
{
    EventData *data = (EventData*)_data;

    auto user = users.find(data->clientid);
    if(user != users.end())
    {
        std::ostringstream os;
        os << RMVED << ":" << data->clientid << ":" << user->second.username  << std::endl;
        server->sendMessage(os.str().c_str());
        users.erase(user);
    }
}

void ChatServer::messageReceived(void *_data)
{
    EventData *data = (EventData*)_data;

    std::string str = data->message;

    str = utils::trim(str);

    //TODO: parse message according to its type (maybe using json/xml format)
    // no validation performed at the moment
    // assuming all messages is right formated with valid contents

    if (str.size() < 4 || str[3] != ':')
    {
        logger << "invalid message received" << std::endl;
        return;
    }

    MessageType type = (MessageType)atoi(str.substr(0, 3).c_str());
    std::ostringstream os;

    switch(type)
    {
    case IAM:
    {
        auto user = users.find(data->clientid);
        if(user != users.end())
        {
            std::string username = str.substr(4);
            user->second.username = username;
            user->second.identified = true;
            os << YOUID << ":" << data->clientid << std::endl;
            server->sendMessage(os.str().c_str(), data->clientid);

            os.str("");
            os << ADDED << ":" << data->clientid << ":" << username << std::endl;

            for (auto it = users.begin() ; it != users.end(); ++it)
            {
                if (it->first != data->clientid && it->second.identified)
                    server->sendMessage(os.str().c_str(), it->first);
            }
        }
        else
        {
            logger << "something went wrong, clientid of origin is not in the list" << std::endl;
        }
        break;
    }
    case MSG:
    {
        auto user = users.find(data->clientid);
        if(user != users.end())
        {
            if (user->second.identified)
            {
                size_t i = str.find_first_of(':', 4);
                size_t j = str.find_first_of(':', i+1);
                size_t to = atoi(str.substr(i+1, j-i-1).c_str());

                auto to_client = users.find(to);
                if(to_client != users.end())
                {
                    if(to_client->second.identified)
                    {
                        os << str  << std::endl;
                        server->sendMessage(os.str().c_str(), to);
                    }
                    else
                    {
                        logger << "message could not be delivered: destine did not provide a username" << std::endl;
                    }
                }
                else
                {
                    logger << "something went wrong, clientid of destine is not in the list" << std::endl;
                }
            }
            else
            {
                logger << "message could not be delivered: origin did not provide a username" << std::endl;
            }
        }
        else
        {
            logger << "something went wrong, clientid of origin is not in the list" << std::endl;
        }
        break;
    }
    case ALL:
    {
        auto user = users.find(data->clientid);
        if(user != users.end())
        {
            if(user->second.identified)
            {
                os << ALL;
                for (auto it = users.begin() ; it != users.end(); ++it)
                    os << ":" << it->first << ":" << it->second.username;
                os << std::endl;

                server->sendMessage(os.str().c_str(), data->clientid);
            }
            else
            {
                logger << "message could not be delivered: origin did not provide a username" << std::endl;
            }

        }
        else
        {
            logger << "something went wrong, clientid of origin is not in the list" << std::endl;
        }

        break;
    }
    default:
        logger << "message type " << type << " does not heve a handler" << std::endl;
    }
}
