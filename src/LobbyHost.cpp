#include "LobbyHost.hpp"

LobbyHost::~LobbyHost() { stop(); }

bool LobbyHost::start(unsigned short port) {
    if (active || listener.listen(port) != sf::Socket::Status::Done) return false;
    listener.setBlocking(false);
    active = true;
    thread = std::thread(&LobbyHost::loop, this);
    return true;
}

void LobbyHost::stop() {
    active = false;
    listener.close();
    guest.disconnect();
    if (thread.joinable()) thread.join();
    connected = false;
}

void LobbyHost::loop() {
    while (active) {
        if (!connected) {
            if (listener.accept(guest) == sf::Socket::Status::Done) {
                guest.setBlocking(false);
                connected = true;
                auto p = net::encode({net::Type::Hello, 0, 0, "StarboardTicTacToe"});
                guest.send(p);
            }
        } else {
            sf::Packet p;
            auto status = guest.receive(p);
            if (status == sf::Socket::Status::Done) {
                net::Message m;
                if (net::decode(p, m)) {
                    std::lock_guard<std::mutex> lock(mutex);
                    latest = std::move(m);
                }
            } else if (status == sf::Socket::Status::Disconnected ||
                       status == sf::Socket::Status::Error) {
                guest.disconnect();
                connected = false;
                       }
        }
        sf::sleep(sf::milliseconds(5));
    }
}

bool LobbyHost::send(const net::Message& m) {
    if (!connected) return false;
    auto p = net::encode(m);
    return guest.send(p) == sf::Socket::Status::Done;
}

std::optional<net::Message> LobbyHost::poll() {
    std::lock_guard<std::mutex> lock(mutex);
    auto result = std::move(latest);
    latest.reset();
    return result;
}