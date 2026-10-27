
#pragma once

#include <SFML/Network.hpp>
#include <cstdint>
#include <string>
#include <utility>

namespace net
{
    enum class Type : std::uint8_t
    {
        Hello = 1,
        Move = 2,
        State = 3,
        Disconnect = 4
    };

    struct Message
    {
        Type type{Type::Hello};
        std::int32_t a{0};
        std::int32_t b{0};
        std::string text;
    };

    inline sf::Packet encode(const Message& message)
    {
        sf::Packet packet;

        packet << static_cast<std::uint8_t>(message.type)
               << message.a
               << message.b
               << message.text;

        return packet;
    }

    inline bool decode(sf::Packet& packet, Message& message)
    {
        std::uint8_t type{};
        std::int32_t a{};
        std::int32_t b{};
        std::string text;

        if (!(packet >> type >> a >> b >> text))
            return false;

        if (type < 1 || type > 4)
            return false;

        message = {
            static_cast<Type>(type),
            a,
            b,
            std::move(text)
        };

        return true;
    }
}
