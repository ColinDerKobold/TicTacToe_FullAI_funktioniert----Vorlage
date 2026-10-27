#pragma once
#include <SFML/Network.hpp>
#include <atomic>
#include <mutex>
#include <optional>
#include <thread>
#include "Protocol.hpp"

class LobbyHost {
public:
    ~LobbyHost();
    bool start(unsigned short port = 53000);
    void stop();
    bool running() const { return active; }
    bool send(const net::Message& message);
    std::optional<net::Message> poll();

private:
    void loop();
    sf::TcpListener listener;
    sf::TcpSocket guest;
    std::thread thread;
    std::atomic_bool active{false}, connected{false};
    std::mutex mutex;
    std::optional<net::Message> latest;
};