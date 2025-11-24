#include "Game.h"
#include <iostream>

Game::Game() : window(sf::VideoMode(800, 600), "Fight Club: Elemental Combat") {
    window.setFramerateLimit(60);
    
    // Start in the Menu
    currentState = STATE_MENU;

    // Load Assets
    if (!font.loadFromFile("arial.ttf")) {
        // Try a system font fallback if specific file fails (Linux/Windows specific)
        // For this code, we just print error. YOU MUST HAVE font.ttf
        std::cerr << "ERROR: font.ttf not found! Text will not display." << std::endl;
    }

    // Initialize UI Text Objects
    initUI();

    player1 = new Player(100.f, 300.f, sf::Color::Blue, 1);
    player2 = new Player(700.f, 300.f, sf::Color::Red, 2);

    // if (!platformTexture.loadFromFile("obstacle.png")) {
    //     platformTexture.create(50, 50); 
    // }

    if (!bgTexture.loadFromFile("background.jpg")) {
        // No bg
    } else {
        bgSprite.setTexture(bgTexture);
        sf::Vector2u size = bgTexture.getSize();
        float scaleX = 800.0f / size.x;
        float scaleY = 600.0f / size.y;
        bgSprite.setScale(scaleX, scaleY);
    }

    // platforms.emplace_back(platformTexture, 350.f, 50.f, 100.f, 50.f);
    // platforms.emplace_back(platformTexture, 350.f, 500.f, 100.f, 50.f);
}

Game::~Game() {
    delete player1;
    delete player2;
}

void Game::initUI() {
    // Helper lambda to setup button text
    auto setupText = [&](sf::Text& text, std::string str, int size, float x, float y) {
        text.setFont(font);
        text.setString(str);
        text.setCharacterSize(size);
        text.setFillColor(sf::Color::White);
        text.setOutlineColor(sf::Color::Black);
        text.setOutlineThickness(2);
        // Center origin
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.width / 2, bounds.height / 2);
        text.setPosition(x, y);
    };

    setupText(titleText, "FIGHT CLUB", 60, 400, 100);
    titleText.setFillColor(sf::Color(255, 50, 50)); // Red title

    setupText(startButton, "START GAME", 40, 400, 250);
    setupText(controlsButton, "CONTROLS & COMBOS", 40, 400, 350);
    setupText(exitButton, "EXIT", 40, 400, 450);

    // Controls Screen
    setupText(controlsTitle, "INSTRUCTIONS", 50, 400, 50);
    
    std::string helpText = 
        "PLAYER 1 (Blue):\n"
        "Move: W A S D\n"
        "Elements: 1 (Fire), 2 (Water), 3 (Lightning)\n"
        "Cast: SPACE\n\n"
        "PLAYER 2 (Yellow):\n"
        "Move: Arrows\n"
        "Elements: I (Fire), O (Water), P (Lightning)\n"
        "Cast: ENTER\n\n"
        "COMBOS:\n"
        "- Fire + Water: SHIELD (Blocks dmg)\n"
        "- Fire + Lightning: TELEPORT UP\n"
        "- Water + Lightning: SLOW ENEMY\n"
        "- Fire + Water + Lightning: ULTIMATE STUN";

    setupText(controlsContent, helpText, 20, 400, 300);
    // Re-center the content block manually as it's multi-line
    sf::FloatRect b = controlsContent.getLocalBounds();
    controlsContent.setOrigin(b.width/2, b.height/2);

    setupText(backButton, "BACK TO MENU", 30, 400, 550);
}

void Game::resetGame() {
    // Reset players logic if needed (HP/MP)
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
        if (event.type == sf::Event::Closed)
            window.close();
        
        // Input handling based on State
        if (currentState == STATE_PLAYING) {
            player1->handleInput(event);
            player2->handleInput(event);
            
            // Optional: Press Escape to go back to menu
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                currentState = STATE_MENU;
            }
        }
        else if (currentState == STATE_MENU) {
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                
                if (startButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    resetGame(); // Reset HP before starting
                    currentState = STATE_PLAYING;
                }
                else if (controlsButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    currentState = STATE_CONTROLS;
                }
                else if (exitButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    window.close();
                }
            }
        }
        else if (currentState == STATE_CONTROLS) {
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    currentState = STATE_MENU;
                }
            }
        }
    }
}

void Game::update(sf::Time dt) {
    // Update logic depending on state
    if (currentState == STATE_MENU) {
        // Hover Effects
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        
        startButton.setFillColor(startButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color::Yellow : sf::Color::White);
        controlsButton.setFillColor(controlsButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color::Yellow : sf::Color::White);
        exitButton.setFillColor(exitButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color::Yellow : sf::Color::White);
    }
    else if (currentState == STATE_CONTROLS) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        backButton.setFillColor(backButton.getGlobalBounds().contains(mousePos.x, mousePos.y) ? sf::Color::Yellow : sf::Color::White);
    }
    else if (currentState == STATE_PLAYING) {
        player1->update(dt);
        player2->update(dt);

        // Collision Logic
        if (collisionManager.checkHits(player1, player2)) shockManager.sendShock(1);
        if (collisionManager.checkHits(player2, player1)) shockManager.sendShock(1);
        if (collisionManager.checkHits(player1, player1)) shockManager.sendShock(1);
        if (collisionManager.checkHits(player2, player2)) shockManager.sendShock(1);

        collisionManager.checkEnvironment(player1, platforms);
        collisionManager.checkEnvironment(player2, platforms);

        checkVictory();
    }
}

void Game::draw() {
    window.clear(sf::Color::Black); 
    
    // Always draw Background (maybe dimmed in menu?)
    if (bgTexture.getSize().x > 0) {
        window.draw(bgSprite);
    }

    // State-based Drawing
    if (currentState == STATE_MENU) {
        drawMenu();
    }
    else if (currentState == STATE_CONTROLS) {
        drawControls();
    }
    else if (currentState == STATE_PLAYING) {
        drawGame();
    }

    window.display();
}

void Game::drawMenu() {
    // Semi-transparent overlay to make text pop
    sf::RectangleShape overlay(sf::Vector2f(800, 600));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);

    window.draw(titleText);
    window.draw(startButton);
    window.draw(controlsButton);
    window.draw(exitButton);
}

void Game::drawControls() {
    sf::RectangleShape overlay(sf::Vector2f(800, 600));
    overlay.setFillColor(sf::Color(0, 0, 0, 200)); // Darker for reading
    window.draw(overlay);

    window.draw(controlsTitle);
    window.draw(controlsContent);
    window.draw(backButton);
}

void Game::drawGame() {
    for (auto& platform : platforms) {
        platform.draw(window);
    }

    player1->draw(window);
    player2->draw(window);
    
    drawHUD();
}

void Game::drawHUD() {
    float barWidth = 300.f;
    float hpHeight = 20.f;
    float mpHeight = 10.f;
    float edgePadding = 20.f; 
    float topPadding = 20.f;

    // === PLAYER 1 ===
    sf::RectangleShape p1HpBg(sf::Vector2f(barWidth, hpHeight));
    p1HpBg.setPosition(edgePadding, topPadding);
    p1HpBg.setFillColor(sf::Color(50, 0, 0)); 
    window.draw(p1HpBg);

    float p1HpPct = player1->getHP() / player1->getMaxHP();
    sf::RectangleShape p1Hp(sf::Vector2f(barWidth * p1HpPct, hpHeight));
    p1Hp.setPosition(edgePadding, topPadding);
    p1Hp.setFillColor(sf::Color::Green);
    window.draw(p1Hp);

    sf::RectangleShape p1MpBg(sf::Vector2f(barWidth, mpHeight));
    p1MpBg.setPosition(edgePadding, topPadding + hpHeight + 5); 
    p1MpBg.setFillColor(sf::Color(0, 0, 50)); 
    window.draw(p1MpBg);

    float p1MpPct = player1->getMP() / player1->getMaxMP();
    sf::RectangleShape p1Mp(sf::Vector2f(barWidth * p1MpPct, mpHeight));
    p1Mp.setPosition(edgePadding, topPadding + hpHeight + 5);
    p1Mp.setFillColor(sf::Color::Cyan);
    window.draw(p1Mp);

    // === PLAYER 2 ===
    sf::RectangleShape p2HpBg(sf::Vector2f(barWidth, hpHeight));
    p2HpBg.setPosition(800 - edgePadding - barWidth, topPadding);
    p2HpBg.setFillColor(sf::Color(50, 0, 0));
    window.draw(p2HpBg);

    float p2HpPct = player2->getHP() / player2->getMaxHP();
    sf::RectangleShape p2Hp(sf::Vector2f(barWidth * p2HpPct, hpHeight));
    p2Hp.setPosition(800 - edgePadding - barWidth, topPadding);
    p2Hp.setFillColor(sf::Color::Green);
    window.draw(p2Hp);

    sf::RectangleShape p2MpBg(sf::Vector2f(barWidth, mpHeight));
    p2MpBg.setPosition(800 - edgePadding - barWidth, topPadding + hpHeight + 5);
    p2MpBg.setFillColor(sf::Color(0, 0, 50));
    window.draw(p2MpBg);

    float p2MpPct = player2->getMP() / player2->getMaxMP();
    sf::RectangleShape p2Mp(sf::Vector2f(barWidth * p2MpPct, mpHeight));
    p2Mp.setPosition(800 - edgePadding - barWidth, topPadding + hpHeight + 5);
    p2Mp.setFillColor(sf::Color::Cyan);
    window.draw(p2Mp);
}

void Game::checkVictory() {
    if (player1->getHP() <= 0 || player2->getHP() <= 0) {
        // Simple victory for now - restart game
        // In a full polished game, switch to STATE_GAME_OVER
        std::cout << "Game Over!" << std::endl;
        currentState = STATE_MENU; // Go back to menu
        resetGame();
    }
}