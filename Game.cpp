#include "Game.h"
#include <iostream>
#include <cmath> // Required for sin/cos

// Helper for button bounds
sf::FloatRect getButtonBounds(const sf::Text& text) {
    sf::FloatRect bounds = text.getGlobalBounds();
    if (bounds.width <= 1.0f || bounds.height <= 1.0f) {
        float width = 250.f; float height = 50.f;
        return sf::FloatRect(text.getPosition().x - width/2.f, text.getPosition().y - height/2.f, width, height);
    }
    return sf::FloatRect(bounds.left - 10.f, bounds.top - 5.f, bounds.width + 20.f, bounds.height + 10.f);
}

void drawButtonWithBox(sf::RenderWindow& window, sf::Text& text) {
    sf::FloatRect bounds = getButtonBounds(text);
    sf::RectangleShape box(sf::Vector2f(bounds.width, bounds.height));
    box.setPosition(bounds.left, bounds.top);
    box.setFillColor(sf::Color(50, 50, 50, 200)); 
    box.setOutlineColor(sf::Color::White);
    box.setOutlineThickness(2);
    window.draw(box);
    window.draw(text);
}

// --- SHADER CODE (GLSL) ---
const std::string crtFragmentShader = R"(
    uniform sampler2D texture;
    uniform float time; 

    void main() {
        vec2 pos = gl_TexCoord[0].xy;
        vec4 color = texture2D(texture, pos);
        
        // Scanline Effect
        float scanline = sin((pos.y + time * 0.1) * 800.0) * 0.04;
        color -= vec4(scanline, scanline, scanline, 0.0);
        
        // Vignette Effect (Dark corners)
        float dist = distance(pos, vec2(0.5, 0.5));
        color *= smoothstep(0.8, 0.2, dist * 0.8);

        gl_FragColor = color;
    }
)";

// FIX IS HERE: Added ", shockManager("/dev/ttyACM0")" to the initializer list
Game::Game() : window(sf::VideoMode(800, 600), "Fight Club: Elemental Combat"), shockManager("/dev/ttyACM0") {
    window.setFramerateLimit(60);
    currentState = STATE_MENU;

    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "CRITICAL ERROR: 'font.ttf' not found!" << std::endl;
    }

    initUI();

    // --- SHADER INIT ---
    if (!sf::Shader::isAvailable()) {
        std::cerr << "Shaders are not available on this system." << std::endl;
    } else {
        if (!scanlineShader.loadFromMemory(crtFragmentShader, sf::Shader::Fragment)) {
            std::cerr << "Failed to load shader." << std::endl;
        }
        scanlineShader.setUniform("texture", sf::Shader::CurrentTexture);
    }

    // --- STATIC BACKGROUND INIT ---
    if (bgTexture.loadFromFile("background.jpg")) {
        bgSprite.setTexture(bgTexture);
        bgSprite.setOrigin(bgTexture.getSize().x / 2, bgTexture.getSize().y / 2);
        bgSprite.setPosition(400, 300); // Center it

        // Scale to cover window
        float scaleX = 800.0f / bgTexture.getSize().x;
        float scaleY = 600.0f / bgTexture.getSize().y;
        
        // Slight overscale for safety
        bgSprite.setScale(scaleX * 1.02f, scaleY * 1.02f);
    }

    player1 = new Player(100.f, 300.f, sf::Color::Blue, 1);
    player2 = new Player(700.f, 300.f, sf::Color::Red, 2);

    if (!platformTexture.loadFromFile("obstacle.png")) {
        platformTexture.create(50, 50); 
    }

    // --- LEVEL PLATFORMS ---
    // Only keeping the top platform and the floor
    platforms.emplace_back(platformTexture, 350.f, 50.f, 100.f, 50.f);
    
    // REMOVED MIDDLE BOTTOM BLOCK as requested
    // platforms.emplace_back(platformTexture, 350.f, 500.f, 100.f, 50.f);
    
    // The Floor (y=580, height=20)
    platforms.emplace_back(platformTexture, 0.f, 580.f, 800.f, 20.f);
}

Game::~Game() {
    delete player1;
    delete player2;
}

void Game::initUI() {
    auto setupText = [&](sf::Text& text, std::string str, int size, float x, float y) {
        text.setFont(font); text.setString(str); text.setCharacterSize(size);
        text.setFillColor(sf::Color::White); text.setOutlineColor(sf::Color::Black); text.setOutlineThickness(2);
        sf::FloatRect bounds = text.getLocalBounds();
        if (bounds.width > 0) text.setOrigin(bounds.width / 2, bounds.height / 2);
        text.setPosition(x, y);
    };

    setupText(titleText, "FIGHT CLUB", 60, 400, 100);
    titleText.setFillColor(sf::Color(255, 50, 50)); 
    setupText(startButton, "START GAME", 40, 400, 250);
    setupText(controlsButton, "CONTROLS & COMBOS", 40, 400, 350);
    setupText(exitButton, "EXIT", 40, 400, 450);

    setupText(controlsTitle, "INSTRUCTIONS", 50, 400, 50);
    setupText(controlsContent, "P1: WASD, Space\nP2: Arrows, Enter", 20, 400, 300);
    setupText(backButton, "BACK TO MENU", 30, 400, 550);

    setupText(gameOverTitle, "GAME OVER", 60, 400, 150);
    gameOverTitle.setFillColor(sf::Color::Red);
    setupText(winnerText, "PLAYER X WINS!", 40, 400, 250);
    setupText(restartButton, "PLAY AGAIN", 35, 400, 350);
    setupText(quitButton, "QUIT", 35, 400, 450);
}

void Game::resetGame() {
    delete player1;
    delete player2;
    player1 = new Player(100.f, 300.f, sf::Color::Blue, 1);
    player2 = new Player(700.f, 300.f, sf::Color::Red, 2);
}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Time dt = clock.restart();
        processEvents();
        update(dt);
        draw();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();
        
        if (currentState == STATE_PLAYING) {
            player1->handleInput(event);
            player2->handleInput(event);
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) currentState = STATE_MENU;
        }
        else if (currentState == STATE_MENU || currentState == STATE_CONTROLS || currentState == STATE_GAME_OVER) {
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (currentState == STATE_MENU) {
                    if (getButtonBounds(startButton).contains(mousePos.x, mousePos.y)) { resetGame(); currentState = STATE_PLAYING; }
                    else if (getButtonBounds(controlsButton).contains(mousePos.x, mousePos.y)) currentState = STATE_CONTROLS;
                    else if (getButtonBounds(exitButton).contains(mousePos.x, mousePos.y)) window.close();
                }
                else if (currentState == STATE_CONTROLS) {
                    if (getButtonBounds(backButton).contains(mousePos.x, mousePos.y)) currentState = STATE_MENU;
                }
                else if (currentState == STATE_GAME_OVER) {
                    if (getButtonBounds(restartButton).contains(mousePos.x, mousePos.y)) { resetGame(); currentState = STATE_PLAYING; }
                    else if (getButtonBounds(quitButton).contains(mousePos.x, mousePos.y)) window.close();
                }
            }
        }
    }
}

// Global time tracker for shader
static float gameTime = 0.0f;

void Game::update(sf::Time dt) {
    // --- 1. Update Time & Shader ---
    gameTime += dt.asSeconds();
    scanlineShader.setUniform("time", gameTime);

    // --- 2. Background Color Pulse (Breathing Effect) ---
    // Oscillates between 150 (darker) and 255 (bright)
    sf::Uint8 val = static_cast<sf::Uint8>(200 + 55 * std::sin(gameTime * 0.5f)); 
    bgSprite.setColor(sf::Color(val, val, val));

    if (currentState == STATE_PLAYING) {
        player1->update(dt);
        player2->update(dt);

        auto handleHit = [&](Player* attacker, Player* victim) {
            if (collisionManager.checkHits(attacker, victim)) {
                shockManager.sendShock(1);
            }
        };

        handleHit(player1, player2);
        handleHit(player2, player1);
        handleHit(player1, player1);
        handleHit(player2, player2);

        collisionManager.checkEnvironment(player1, platforms);
        collisionManager.checkEnvironment(player2, platforms);

        // --- NEW: Player-to-Player Collision Check ---
        collisionManager.checkPlayerCollision(player1, player2);

        checkVictory();
    }
}

void Game::draw() {
    window.clear(sf::Color::Black); 
    
    // --- DRAW BACKGROUND WITH SHADER ---
    if (bgTexture.getSize().x > 0) {
        // Apply the scanline/vignette shader ONLY to the background
        window.draw(bgSprite, &scanlineShader);
    }

    if (currentState == STATE_MENU) drawMenu();
    else if (currentState == STATE_CONTROLS) drawControls();
    else if (currentState == STATE_PLAYING) drawGame();
    else if (currentState == STATE_GAME_OVER) drawGameOver();

    window.display();
}

void Game::drawMenu() {
    sf::RectangleShape overlay(sf::Vector2f(800, 600)); overlay.setFillColor(sf::Color(0, 0, 0, 150)); window.draw(overlay);
    window.draw(titleText); drawButtonWithBox(window, startButton); drawButtonWithBox(window, controlsButton); drawButtonWithBox(window, exitButton);
}
void Game::drawControls() {
    sf::RectangleShape overlay(sf::Vector2f(800, 600)); overlay.setFillColor(sf::Color(0, 0, 0, 200)); window.draw(overlay);
    window.draw(controlsTitle); window.draw(controlsContent); drawButtonWithBox(window, backButton);
}
void Game::drawGameOver() {
    sf::RectangleShape overlay(sf::Vector2f(800, 600)); overlay.setFillColor(sf::Color(0, 0, 0, 200)); window.draw(overlay);
    window.draw(gameOverTitle); window.draw(winnerText); drawButtonWithBox(window, restartButton); drawButtonWithBox(window, quitButton);
}

void Game::drawGame() {
    for (auto& platform : platforms) platform.draw(window);
    player1->draw(window);
    player2->draw(window);
    drawHUD();
}

void Game::drawHUD() {
    float barWidth = 300.f; float hpHeight = 20.f; float mpHeight = 10.f; float edge = 20.f; float top = 20.f;
    auto drawBar = [&](float x, float y, float w, float h, float pct, sf::Color c) {
        sf::RectangleShape bg(sf::Vector2f(w, h)); bg.setPosition(x, y); bg.setFillColor(sf::Color(50, 0, 0)); window.draw(bg);
        if(pct < 0) pct = 0;
        sf::RectangleShape fg(sf::Vector2f(w * pct, h)); fg.setPosition(x, y); fg.setFillColor(c); window.draw(fg);
    };
    drawBar(edge, top, barWidth, hpHeight, player1->getHP()/player1->getMaxHP(), sf::Color::Green);
    drawBar(edge, top+hpHeight+5, barWidth, mpHeight, player1->getMP()/player1->getMaxMP(), sf::Color::Cyan);
    drawBar(800-edge-barWidth, top, barWidth, hpHeight, player2->getHP()/player2->getMaxHP(), sf::Color::Green);
    drawBar(800-edge-barWidth, top+hpHeight+5, barWidth, mpHeight, player2->getMP()/player2->getMaxMP(), sf::Color::Cyan);
}

void Game::checkVictory() {
    if (currentState != STATE_PLAYING) return; 
    if (player1->getHP() <= 0) {
        winnerText.setString("PLAYER 2 WINS!"); winnerText.setFillColor(sf::Color::Yellow);
        sf::FloatRect bounds = winnerText.getLocalBounds(); if(bounds.width > 0) winnerText.setOrigin(bounds.width/2, bounds.height/2);
        winnerText.setPosition(400, 250); currentState = STATE_GAME_OVER;
    }
    else if (player2->getHP() <= 0) {
        winnerText.setString("PLAYER 1 WINS!"); winnerText.setFillColor(sf::Color::Cyan);
        sf::FloatRect bounds = winnerText.getLocalBounds(); if(bounds.width > 0) winnerText.setOrigin(bounds.width/2, bounds.height/2);
        winnerText.setPosition(400, 250); currentState = STATE_GAME_OVER;
    }
}