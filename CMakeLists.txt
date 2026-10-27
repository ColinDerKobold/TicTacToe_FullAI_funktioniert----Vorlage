cmake_minimum_required(VERSION 3.24)
project(StarboardTicTacToe VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 3 COMPONENTS Graphics Window System Audio Network REQUIRED)

add_executable(StarboardTicTacToe
    src/main.cpp
    src/Game.cpp
    src/Game.hpp
        src/Protocol.hpp
        src/LobbyHost.hpp
        src/LobbyHost.cpp
        src/LobbyClient.hpp
        src/LobbyClient.cpp
        src/MultiplayerSession.hpp
)

target_link_libraries(StarboardTicTacToe PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
    SFML::Network
)

target_compile_definitions(StarboardTicTacToe PRIVATE
    STARBOARD_ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
)

if(MSVC)
    target_compile_options(StarboardTicTacToe PRIVATE /W4 /permissive-)
else()
    target_compile_options(StarboardTicTacToe PRIVATE -Wall -Wextra -Wpedantic)
endif()
