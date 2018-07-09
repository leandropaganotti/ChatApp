#ifndef TCPOBSERVERIF
#define TCPOBSERVERIF

namespace Interface {

class TcpObserver
{
public:
    virtual ~TcpObserver(){}
    virtual void clientConnected(void *data) = 0;
    virtual void clientDisconnected(void *data) = 0;
    virtual void messageReceived(void *data) = 0;
};

}
#endif // TCPOBSERVERIF

