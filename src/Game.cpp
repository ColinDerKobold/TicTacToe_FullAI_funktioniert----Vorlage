#include "Game.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <cstdint>

namespace
{
    constexpr unsigned WINDOW_WIDTH = 1280;
    constexpr unsigned WINDOW_HEIGHT = 800;
    constexpr float PI = 3.1415926535f;

    const std::array<std::array<int, 3>, 8> WIN_LINES{{
        {{0, 1, 2}}, {{3, 4, 5}}, {{6, 7, 8}},
        {{0, 3, 6}}, {{1, 4, 7}}, {{2, 5, 8}},
        {{0, 4, 8}}, {{2, 4, 6}}
    }};

    sf::Vector2f centerOf(const sf::FloatRect& r)
    {
        return {r.position.x + r.size.x / 2.f, r.position.y + r.size.y / 2.f};
    }
}

Game::Game()
    : window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "STARBOARD | Tic-Tac-Toe",
             sf::Style::Titlebar | sf::Style::Close)
{
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);
    loadResources();
    resetBoard();
}

int Game::run()
{
    while (window.isOpen())
    {
        processEvents();
        update();
        render();
    }
    return 0;
}

void Game::loadResources()
{
    const std::vector<std::string> fontPaths = {
#ifdef _WIN32
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf",
#else
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
#endif
    };

    for (const auto& path : fontPaths)
    {
        if (font.openFromFile(path))
        {
            fontLoaded = true;
            break;
        }
    }

    const std::filesystem::path musicPath =
        std::filesystem::path(STARBOARD_ASSET_DIR) / "audio" / "starboard_theme.wav";
    if (music.openFromFile(musicPath.string()))
    {
        musicLoaded = true;
        music.setLooping(true);
        music.setVolume(volume);
        music.play();
    }
    else
    {
        std::cerr << "Musikdatei konnte nicht geladen werden: " << musicPath << '\n';
    }
}

sf::Color Game::adjusted(sf::Color c) const
{
    auto scale = [this](std::uint8_t value) {
        return static_cast<std::uint8_t>(
            std::clamp(static_cast<int>(value * brightness), 0, 255)
        );
    };

    return {scale(c.r), scale(c.g), scale(c.b), c.a};
}

sf::Color Game::background() const { return adjusted({12, 18, 34}); }
sf::Color Game::panel() const { return adjusted({22, 31, 52}); }
sf::Color Game::textColor() const { return adjusted({238, 244, 255}); }
sf::Color Game::mutedColor() const { return adjusted({139, 157, 185}); }
sf::Color Game::accent() const { return adjusted({82, 220, 205}); }

bool Game::pointIn(sf::Vector2f p, sf::FloatRect r) const
{
    return r.contains(p);
}

void Game::drawText(const std::string& value, unsigned size, sf::Vector2f pos,
                    sf::Color color, bool centered)
{
    if (!fontLoaded) return;
    sf::Text text(font, value, size);
    text.setFillColor(color);
    if (centered)
    {
        const auto b = text.getLocalBounds();
        text.setOrigin({b.position.x + b.size.x / 2.f, b.position.y + b.size.y / 2.f});
    }
    text.setPosition(pos);
    window.draw(text);
}

void Game::drawPanel(sf::FloatRect bounds, sf::Color color, float outline)
{
    sf::RectangleShape shape(bounds.size);
    shape.setPosition(bounds.position);
    shape.setFillColor(adjusted(color));
    shape.setOutlineThickness(outline);
    shape.setOutlineColor(adjusted({49, 68, 95}));
    window.draw(shape);
}

void Game::renderBackground()
{
    window.clear(background());

    // Subtle star field and soft geometric rings.
    for (int i = 0; i < 72; ++i)
    {
        const float x = static_cast<float>((i * 173 + 43) % static_cast<int>(WINDOW_WIDTH));
        const float y = static_cast<float>((i * 97 + 29) % static_cast<int>(WINDOW_HEIGHT));
        sf::CircleShape star((i % 5 == 0) ? 2.0f : 1.0f);
        star.setPosition({x, y});
        star.setFillColor(adjusted({65, 104, 135,static_cast<std::uint8_t>(60 + (i % 4) * 25)}));
        window.draw(star);
    }

    sf::CircleShape ring(245.f);
    ring.setPosition({-165.f, 490.f});
    ring.setFillColor(sf::Color::Transparent);
    ring.setOutlineThickness(1.f);
    ring.setOutlineColor(adjusted({35, 92, 115, 100}));
    window.draw(ring);

    sf::CircleShape ring2(185.f);
    ring2.setPosition({1050.f, -90.f});
    ring2.setFillColor(sf::Color::Transparent);
    ring2.setOutlineThickness(1.f);
    ring2.setOutlineColor(adjusted({35, 92, 115, 100}));
    window.draw(ring2);

    drawText("S T A R B O A R D", 15, {54.f, 34.f}, mutedColor());
    drawText("TIC-TAC-TOE  /  SOLO PROTOCOL", 12, {54.f, 58.f}, mutedColor());
    sf::RectangleShape line({1172.f, 1.f});
    line.setPosition({54.f, 91.f});
    line.setFillColor(adjusted({47, 66, 91}));
    window.draw(line);
}

void Game::render()
{
    renderBackground();
    switch (screen)
    {
        case Screen::Menu: renderMenu(); break;
        case Screen::Settings: renderSettings(); break;
        case Screen::Playing: renderGame(); break;
        case Screen::Multiplayer: renderMultiplayer(); break;
    }
    window.display();
}

void Game::renderMenu()
{
    drawText("A LITTLE STRATEGY.", 16, {640.f, 188.f}, accent(), true);
    drawText("A LOT OF CHARACTER.", 39, {640.f, 235.f}, textColor(), true);
    drawText("Take your turn. Find your line. Own the board.", 17,
             {640.f, 294.f}, mutedColor(), true);

    const std::array<std::string, 4> labels = {
        "PLAY GAME",
        "SETTINGS",
        "MULTIPLAYER",
        "EXIT"
    };

    const std::array<sf::FloatRect, 4> buttons{{
        {{440.f, 340.f}, {400.f, 65.f}},
        {{440.f, 420.f}, {400.f, 65.f}},
        {{440.f, 500.f}, {400.f, 65.f}},
        {{440.f, 580.f}, {400.f, 65.f}}
    }};

    for (std::size_t i = 0; i < buttons.size(); ++i)
    {
        const bool primary = i == 0;
        drawPanel(buttons[i], primary ? sf::Color(29, 75, 83) : sf::Color(22, 31, 52),
                  primary ? 2.f : 1.f);
        drawText(labels[i], 18, centerOf(buttons[i]), primary ? accent() : textColor(), true);
    }

    drawText("PLAYER 01  /  VS  THE MACHINE", 12, {640.f, 700.f}, mutedColor(), true);
    drawText("A SMALL GAME FOR BIG BRAIN MOMENTS", 11, {640.f, 725.f}, mutedColor(), true);
}

void Game::renderMultiplayer()
{
    drawText("MULTIPLAYER", 32,
             {640.f, 175.f}, textColor(), true);

    drawText("PLAY TOGETHER. OWN THE BOARD.",
             16, {640.f, 215.f}, mutedColor(), true);

    // Button 1: Lobby hosten
    drawPanel({{440.f, 300.f}, {400.f, 65.f}},
              {29, 75, 83}, 2.f);

    drawText("HOST LOBBY", 18,
             {640.f, 332.f}, accent(), true);

    // Button 2: Lobby beitreten
    drawPanel({{440.f, 390.f}, {400.f, 65.f}},
              {22, 31, 52}, 1.f);

    drawText("JOIN LOBBY", 18,
             {640.f, 422.f}, textColor(), true);

    // Button 3: Zurück
    drawPanel({{440.f, 560.f}, {400.f, 58.f}},
              {22, 31, 52}, 1.f);

    drawText("BACK TO MENU", 16,
             {640.f, 589.f}, textColor(), true);

    drawText("Online matchmaking will be added next.",
             13, {640.f, 665.f}, mutedColor(), true);
}

void Game::renderSettings()
{
    drawText("TUNE YOUR EXPERIENCE", 32, {640.f, 165.f}, textColor(), true);
    drawText("A couple of sliders. Nothing complicated.", 16, {640.f, 207.f}, mutedColor(), true);

    drawPanel({{300.f, 275.f}, {680.f, 290.f}}, {22, 31, 52}, 1.f);
    drawText("MUSIC VOLUME", 16, {360.f, 320.f}, textColor());
    drawText(std::to_string(static_cast<int>(volume)) + "%", 16, {920.f, 320.f}, accent(), true);

    sf::RectangleShape track({560.f, 8.f});
    track.setPosition({360.f, 365.f});
    track.setFillColor(adjusted({54, 69, 91}));
    window.draw(track);
    sf::RectangleShape fill({560.f * volume / 100.f, 8.f});
    fill.setPosition({360.f, 365.f});
    fill.setFillColor(accent());
    window.draw(fill);
    sf::CircleShape knob(10.f);
    knob.setOrigin({10.f, 10.f});
    knob.setPosition({360.f + 560.f * volume / 100.f, 369.f});
    knob.setFillColor(textColor());
    window.draw(knob);
    drawText("0%", 12, {360.f, 390.f}, mutedColor());
    drawText("100%", 12, {920.f, 390.f}, mutedColor(), true);

    drawText("SCREEN BRIGHTNESS", 16, {360.f, 435.f}, textColor());
    drawText(std::to_string(static_cast<int>(brightness * 100.f)) + "%", 16,
             {920.f, 435.f}, accent(), true);
    sf::RectangleShape btrack({560.f, 8.f});
    btrack.setPosition({360.f, 480.f});
    btrack.setFillColor(adjusted({54, 69, 91}));
    window.draw(btrack);
    sf::RectangleShape bfill({560.f * (brightness - 0.55f) / 0.75f, 8.f});
    bfill.setPosition({360.f, 480.f});
    bfill.setFillColor(accent());
    window.draw(bfill);
    sf::CircleShape bknob(10.f);
    bknob.setOrigin({10.f, 10.f});
    bknob.setPosition({360.f + 560.f * (brightness - 0.55f) / 0.75f, 484.f});
    bknob.setFillColor(textColor());
    window.draw(bknob);

    drawPanel({{440.f, 610.f}, {400.f, 58.f}}, {29, 75, 83}, 1.f);
    drawText("BACK TO MENU", 16, {640.f, 639.f}, accent(), true);
    drawText("TIP: Set music to 0% for complete silence.", 13,
             {640.f, 695.f}, mutedColor(), true);
}

void Game::renderGame()
{
    drawText("YOUR MOVE", 27, {640.f, 135.f}, textColor(), true);

    std::string status = "Your mark is X  /  Bot is O";
    if (result == Result::HumanWon) status = "NICE LINE. YOU WIN.";
    else if (result == Result::BotWon) status = "THE BOT GOT THIS ONE.";
    else if (result == Result::Draw) status = "A PERFECTLY EVEN STALEMATE.";
    else if (botThinking) status = "BOT IS THINKING...";
    drawText(status, 15, {640.f, 178.f},
             (result == Result::HumanWon) ? accent() : mutedColor(), true);

    const float boardX = 460.f;
    const float boardY = 235.f;
    const float cell = 120.f;
    const float gap = 12.f;

    for (int i = 0; i < 9; ++i)
    {
        const int row = i / 3;
        const int col = i % 3;
        sf::FloatRect r({boardX + col * (cell + gap), boardY + row * (cell + gap)},
                        {cell, cell});
        drawPanel(r, {22, 31, 52}, 1.f);

        if (board[static_cast<std::size_t>(i)] == Mark::X)
        {
            const float x = r.position.x + 34.f, y = r.position.y + 34.f;
            sf::RectangleShape a({68.f, 7.f});
            a.setOrigin({34.f, 3.5f}); a.setPosition({x + 34.f, y + 34.f});
            a.setRotation(sf::degrees(45.f)); a.setFillColor(accent()); window.draw(a);
            sf::RectangleShape b({68.f, 7.f});
            b.setOrigin({34.f, 3.5f}); b.setPosition({x + 34.f, y + 34.f});
            b.setRotation(sf::degrees(-45.f)); b.setFillColor(accent()); window.draw(b);
        }
        else if (board[static_cast<std::size_t>(i)] == Mark::O)
        {
            sf::CircleShape circle(31.f);
            circle.setPosition({r.position.x + 29.f, r.position.y + 29.f});
            circle.setFillColor(sf::Color::Transparent);
            circle.setOutlineThickness(7.f);
            circle.setOutlineColor(adjusted({255, 174, 106}));
            window.draw(circle);
        }
        else
        {
            sf::CircleShape dot(3.f);
            dot.setPosition({r.position.x + cell / 2.f - 3.f, r.position.y + cell / 2.f - 3.f});
            dot.setFillColor(adjusted({55, 73, 95}));
            window.draw(dot);
        }
    }

    drawPanel({{380.f, 650.f}, {230.f, 52.f}}, {22, 31, 52}, 1.f);
    drawPanel({{670.f, 650.f}, {230.f, 52.f}}, {29, 75, 83}, 1.f);
    drawText("MAIN MENU  [M]", 13, {495.f, 676.f}, textColor(), true);
    drawText("NEW ROUND  [N]", 13, {785.f, 676.f}, accent(), true);
    drawText("Click a tile to place X", 13, {640.f, 730.f}, mutedColor(), true);
}

void Game::processEvents()
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }
        else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
        {
            if (key->code == sf::Keyboard::Key::Escape)
            {
                if (screen == Screen::Playing || screen == Screen::Settings) returnToMenu();
                else window.close();
            }
            if (screen == Screen::Playing)
            {
                if (key->code == sf::Keyboard::Key::R || key->code == sf::Keyboard::Key::N)
                    resetBoard();
                if (key->code == sf::Keyboard::Key::M) returnToMenu();
            }
            if (screen == Screen::Settings && key->code == sf::Keyboard::Key::R)
            {
                volume = 42.f; brightness = 1.f;
                if (musicLoaded) music.setVolume(volume);
            }
            if (screen == Screen::Menu && (key->code == sf::Keyboard::Key::Enter ||
                                           key->code == sf::Keyboard::Key::Space))
                startGame();
        }
        else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouse->button == sf::Mouse::Button::Left)
            {
                const auto p = window.mapPixelToCoords(mouse->position);
                handleClick(p);
            }
        }
        else if (const auto* moved = event->getIf<sf::Event::MouseMoved>())
        {
            if (screen == Screen::Settings)
            {
                const auto p = window.mapPixelToCoords(moved->position);
                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
                {
                    if (p.y >= 340.f && p.y <= 390.f && p.x >= 350.f && p.x <= 930.f)
                    {
                        volume = std::clamp((p.x - 360.f) / 560.f * 100.f, 0.f, 100.f);
                        if (musicLoaded) music.setVolume(volume);
                    }
                    if (p.y >= 455.f && p.y <= 510.f && p.x >= 350.f && p.x <= 930.f)
                        brightness = std::clamp(0.55f + (p.x - 360.f) / 560.f * 0.75f, 0.55f, 1.30f);
                }
            }
        }
    }
}

void Game::handleClick(sf::Vector2f p)
{
    if (screen == Screen::Menu)
    {
        // PLAY GAME
        if (pointIn(p, {{440.f, 340.f}, {400.f, 65.f}}))
        {
            startGame();
        }

        // SETTINGS
        else if (pointIn(p, {{440.f, 420.f}, {400.f, 65.f}}))
        {
            screen = Screen::Settings;
        }

        // MULTIPLAYER
        else if (pointIn(p, {{440.f, 500.f}, {400.f, 65.f}}))
        {
            screen = Screen::Multiplayer;
        }

        // EXIT
        else if (pointIn(p, {{440.f, 580.f}, {400.f, 65.f}}))
        {
            window.close();
        }
    }
    else if (screen == Screen::Settings)
    {
        if (p.y >= 340.f && p.y <= 390.f && p.x >= 350.f && p.x <= 930.f)
        {
            volume = std::clamp((p.x - 360.f) / 560.f * 100.f, 0.f, 100.f);
            if (musicLoaded) music.setVolume(volume);
        }
        else if (p.y >= 455.f && p.y <= 510.f && p.x >= 350.f && p.x <= 930.f)
            brightness = std::clamp(0.55f + (p.x - 360.f) / 560.f * 0.75f, 0.55f, 1.30f);
        else if (pointIn(p, {{440.f, 610.f}, {400.f, 58.f}}))
            screen = Screen::Menu;
    }
    else if (screen == Screen::Multiplayer)
    {
        // HOST LOBBY
        if (pointIn(p, {{440.f, 300.f}, {400.f, 65.f}}))
        {
            // Hier starten wir im nächsten Schritt den Netzwerk-Host.
            // Noch keine echte Lobby-Verbindung.
        }

        // JOIN LOBBY
        else if (pointIn(p, {{440.f, 390.f}, {400.f, 65.f}}))
        {
            // Hier verbinden wir im nächsten Schritt den Netzwerk-Client.
            // Dafür brauchen wir später IP-Adresse und Port.
        }

        // BACK TO MENU
        else if (pointIn(p, {{440.f, 560.f}, {400.f, 58.f}}))
        {
            screen = Screen::Menu;
        }
    }
    else if (screen == Screen::Playing && result == Result::Playing && !botThinking)
    {
        const float bx = 460.f, by = 235.f, cell = 120.f, gap = 12.f;
        for (int i = 0; i < 9; ++i)
        {
            const int row = i / 3, col = i % 3;
            sf::FloatRect r({bx + col * (cell + gap), by + row * (cell + gap)}, {cell, cell});
            if (pointIn(p, r)) { humanMove(i); break; }
        }
        if (pointIn(p, {{380.f, 650.f}, {230.f, 52.f}})) returnToMenu();
        if (pointIn(p, {{670.f, 650.f}, {230.f, 52.f}})) resetBoard();
    }
    else if (screen == Screen::Playing)
    {
        if (pointIn(p, {{380.f, 650.f}, {230.f, 52.f}})) returnToMenu();
        if (pointIn(p, {{670.f, 650.f}, {230.f, 52.f}})) resetBoard();
    }
}

void Game::startGame()
{
    resetBoard();
    screen = Screen::Playing;
}

void Game::returnToMenu()
{
    screen = Screen::Menu;
}

void Game::resetBoard()
{
    board.fill(Mark::Empty);
    result = Result::Playing;
    botThinking = false;
}

void Game::humanMove(int index)
{
    if (index < 0 || index >= 9 || board[static_cast<std::size_t>(index)] != Mark::Empty)
        return;

    board[static_cast<std::size_t>(index)] = Mark::X;
    result = evaluateBoard();
    if (result != Result::Playing) return;

    botThinking = true;
    botClock.restart();
}

void Game::update()
{
    if (screen == Screen::Playing && botThinking && botClock.getElapsedTime().asSeconds() > 0.38f)
        botMove();
}

std::vector<int> Game::availableMoves() const
{
    std::vector<int> moves;
    for (int i = 0; i < 9; ++i)
        if (board[static_cast<std::size_t>(i)] == Mark::Empty) moves.push_back(i);
    return moves;
}

bool Game::boardFull() const
{
    return std::none_of(board.begin(), board.end(), [](Mark m) { return m == Mark::Empty; });
}

Game::Result Game::evaluateBoard() const
{
    for (const auto& line : WIN_LINES)
    {
        const Mark a = board[static_cast<std::size_t>(line[0])];
        if (a != Mark::Empty && a == board[static_cast<std::size_t>(line[1])] &&
            a == board[static_cast<std::size_t>(line[2])])
            return a == Mark::X ? Result::HumanWon : Result::BotWon;
    }
    return boardFull() ? Result::Draw : Result::Playing;
}

void Game::botMove()
{
    auto moves = availableMoves();
    if (moves.empty()) { botThinking = false; result = evaluateBoard(); return; }

    // The bot sometimes blocks or wins, but has a deliberate chance of making a random move.
    // This keeps it beatable instead of using an unbeatable minimax algorithm.
    std::uniform_int_distribution<int> chance(0, 99);
    int chosen = -1;

    if (chance(rng) < 58)
    {
        // Try a winning move.
        for (int move : moves)
        {
            board[static_cast<std::size_t>(move)] = Mark::O;
            if (evaluateBoard() == Result::BotWon) { chosen = move; board[static_cast<std::size_t>(move)] = Mark::Empty; break; }
            board[static_cast<std::size_t>(move)] = Mark::Empty;
        }
        // Sometimes notice an immediate threat and block it.
        if (chosen == -1 && chance(rng) < 55)
        {
            for (int move : moves)
            {
                board[static_cast<std::size_t>(move)] = Mark::X;
                if (evaluateBoard() == Result::HumanWon) { chosen = move; board[static_cast<std::size_t>(move)] = Mark::Empty; break; }
                board[static_cast<std::size_t>(move)] = Mark::Empty;
            }
        }
    }

    if (chosen == -1)
    {
        std::uniform_int_distribution<std::size_t> pick(0, moves.size() - 1);
        chosen = moves[pick(rng)];
    }

    board[static_cast<std::size_t>(chosen)] = Mark::O;
    botThinking = false;
    result = evaluateBoard();
}
