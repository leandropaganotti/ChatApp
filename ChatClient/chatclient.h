#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include "chatprotocol.h"
#include "tcpobserverif.h"
#include "tcpsocket.h"
#include "uiinterface.h"
#include <QObject>
#include <QString>

class ChatClient: public QObject, public Interface::TcpObserver
{
    Q_OBJECT
public:
    ChatClient(QString host, int port, QString username);
    ~ChatClient();
    bool connect();
    void writeTo(const char*msg, size_t to);
    // TcpObserver interface
signals:
    void allUsers(QString);
    void newMessage(QString);
    void newUser(QString);
    void userRemoved(QString);

private:
    void messageReceived(void *data);
    void clientConnected(void *);
    void clientDisconnected(void *);

    TcpSocket socket;
    size_t userid;
    QString host;
    QString username;
    int port;
};






#endif // CHATCLIENT_H
