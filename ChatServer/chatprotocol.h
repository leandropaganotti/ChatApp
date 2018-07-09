#ifndef CHATPROTOCOL_H
#define CHATPROTOCOL_H

enum MessageType
{
    WHO=100,    // server to client - ask username      - who:
    IAM,        // client to server - inform username   - iam:username
    MSG,        // client to client - msg to delever    - msg:idfrom:idto:message
    ADDED,      // server to client - new client added  - added:id:username
    RMVED,      // server to client - client removed    - rmved:id:username
    ALL,        // server to client - list of users     - all:id1:username1: ... :idN:usernameN (client can send to ask for list: all:)
    YOUID       // server to client - inform user id    - youid:id
};

#endif // CHATPROTOCOL_H

