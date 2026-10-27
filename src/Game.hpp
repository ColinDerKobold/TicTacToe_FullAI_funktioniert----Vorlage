#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <array>
#include <optional>
#include <random>
#include <string>
#include <vector>

class Game
{
public:
    Game();
    int run();

private:
    enum class Screen { Menu, Settings, Playing, Multiplayer };
    enum class Mark { Empty, X, O };
    enum class Result { Playing, HumanWon, BotWon, Draw };

    sf::RenderWindow window;
    sf::Font font;
    bool fontLoaded = false;
    sf::Music music;
    Screen screen = Screen::Menu;

    float volume = 42.f;
    float brightness = 1.f;
    bool musicLoaded = false;

    std::array<Mark, 9> board{};
    Result result = Result::Playing;
    bool botThinking = false;
    sf::Clock botClock;
    std::mt19937 rng{std::random_device{}()};

    // Multiplayer-Menü
    bool showMultiplayerMenu = false;

    sf::RectangleShape multiplayerButton;
    sf::Text* multiplayerButtonText = nullptr;

    sf::Color background() const;
    sf::Color panel() const;
    sf::Color textColor() const;
    sf::Color mutedColor() const;
    sf::Color accent() const;
    sf::Color adjusted(sf::Color color) const;

    void loadResources();
    void processEvents();
    void update();
    void render();
    void renderBackground();
    void renderMenu();
    void renderSettings();
    void renderGame();
    void renderMultiplayer();
    void drawText(const std::string& value, unsigned size, sf::Vector2f position,
                  sf::Color color, bool centered = false);
    void drawPanel(sf::FloatRect bounds, sf::Color color, float outline = 0.f);
    bool pointIn(sf::Vector2f point, sf::FloatRect bounds) const;

    void startGame();
    void returnToMenu();
    void handleClick(sf::Vector2f point);
    void humanMove(int index);
    void botMove();
    Result evaluateBoard() const;
    bool boardFull() const;
    std::vector<int> availableMoves() const;
    void resetBoard();
};
