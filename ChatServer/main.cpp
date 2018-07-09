#include "tcpserver.h"
#include "chatserver.h"

ChatServer *app = nullptr;

void signal_handler(int)
{
    if(app) app->stop();
    exit(0);
}

int main() {

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    ChatServer chatapp(std::unique_ptr<Interface::TcpServer>( new TcpServer() ));

    app = &chatapp;

    chatapp.start(12345);

    return 0;
}
