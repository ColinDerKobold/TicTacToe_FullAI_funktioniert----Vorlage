#include "LobbyClient.hpp"


bool LobbyClient::join(const std::string& ip, unsigned short port)
{
    leave();

    auto address = sf::IpAddress::resolve(ip);

    if (!address.has_value())
        return false;

    socket.setBlocking(true);

    if (socket.connect(*address, port, sf::seconds(5)) !=
        sf::Socket::Status::Done)
    {
        return false;
    }

    socket.setBlocking(false);
    isConnected = true;

    return true;
}


void LobbyClient::leave() {
    socket.disconnect();
    isConnected = false;
}

bool LobbyClient::send(const net::Message& m) {
    if (!isConnected) return false;
    auto p = net::encode(m);
    auto s = socket.send(p);
    if (s == sf::Socket::Status::Disconnected || s == sf::Socket::Status::Error)
        isConnected = false;
    return s == sf::Socket::Status::Done;
}

std::optional<net::Message> LobbyClient::receive() {
    if (!isConnected) return std::nullopt;
    sf::Packet p;
    auto s = socket.receive(p);
    if (s == sf::Socket::Status::Done) {
        net::Message m;
        if (net::decode(p, m)) return m;
    } else if (s == sf::Socket::Status::Disconnected || s == sf::Socket::Status::Error) {
        isConnected = false;
    }
    return std::nullopt;
}