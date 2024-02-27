#include "net/net.h"


class CustomServer : public net::server_interface<CustomMsgTypes> {
public:
    CustomServer(uint16_t nPort) : net::server_interface<CustomMsgTypes>(nPort) {

    }

protected:
    virtual bool OnClientConnect(std::shared_ptr<net::connection<CustomMsgTypes>> client) {
        net::message msg;
        msg.hid = (uint32_t)CustomMsgTypes::ServerAccept;
        client->Send(msg);
        return true;
    }

    virtual void OnClientDisconnect(std::shared_ptr<net::connection<CustomMsgTypes>> client) {
        std::cout << "Removing client [" << client->GetID() << "]\n";
    }

    virtual void OnMessage(std::shared_ptr<net::connection<CustomMsgTypes>> client, net::message& msg) {
        switch (msg.hid)
        {
        case (uint32_t)CustomMsgTypes::ServerPing:
        {
            std::cout << "[" << client->GetID() << "]: Server Ping\n";
            client->Send(msg);
        }
        break;

        case (uint32_t)CustomMsgTypes::MessageAll:
        {
            std::cout << "[" << client->GetID() << "]: Message All\n";
            //net::message _msg;
            //_msg.hid = (uint32_t)CustomMsgTypes::ServerMessage;
            ////_msg << client->GetID();
            //_msg.body.push_back(client->GetID());
            //_msg.body.insert(_msg.body.end(), msg.body.begin(), msg.body.end());
            //MessageAllClients(_msg);
            msg.hid = (uint32_t)CustomMsgTypes::ServerMessage;
            //_msg << client->GetID();
            msg.body.insert(msg.body.begin(), client->GetID());
            //msg.body.push_front(client->GetID());
            //_msg.body.insert(_msg.body.end(), msg.body.begin(), msg.body.end());
            MessageAllClients(msg);
        }
        break;
        }
    }
};

int main(int argc, char* argv[]) {
    CustomServer server(7777);
    server.Start();
    while (1) {
        server.Update();
    }
    system("pause");
    return 0;
}