#pragma once
#include "LobbyHost.hpp"
#include "LobbyClient.hpp"

class MultiplayerSession {
public:
    bool host(unsigned short port = 53000) { return hostServer.start(port); }
    bool join(const std::string& ip, unsigned short port = 53000) {
        return client.join(ip, port);
    }
    void leave() { client.leave(); hostServer.stop(); }

    LobbyHost& hostConnection() { return hostServer; }
    LobbyClient& clientConnection() { return client; }

private:
    LobbyHost hostServer;
    LobbyClient client;
};