#pragma once
#include <SFML/Network.hpp>
#include <optional>
#include "Protocol.hpp"

class LobbyClient {
public:
    bool join(const std::string& ip, unsigned short port = 53000);
    void leave();
    bool connected() const { return isConnected; }
    bool send(const net::Message& message);
    std::optional<net::Message> receive();

private:
    sf::TcpSocket socket;
    bool isConnected{false};
};