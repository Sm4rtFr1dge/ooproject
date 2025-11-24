#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Player.h"
#include "CollisionManager.h"
#include "ShockManager.cpp"
#include "Platform.h"

class Game {
private:
    sf::RenderWindow window;
    Player* player1;
    Player* player2;
    CollisionManager collisionManager;
    ShockManager shockManager;
    
    sf::Texture platformTexture;
    std::vector<Platform> platforms;

    sf::Texture bgTexture;
    sf::Sprite bgSprite;

    void processEvents();
    void update(sf::Time dt);
    void draw();
    void drawHUD(); // New Function for Top-of-screen bars
    void checkVictory();
    Player* player1;
    Player* player2;
    CollisionManager collisionManager;
    ShockManager shockManager;
    
    sf::Texture platformTexture;
    std::vector<Platform> platforms;

    sf::Texture bgTexture;
    sf::Sprite bgSprite;

    // --- UI / STATE VARIABLES ---
    GameState currentState;
    sf::Font font;

    // Menu Elements
    sf::Text titleText;
    sf::Text startButton;
    sf::Text controlsButton;
    sf::Text exitButton;

    // Controls Elements
    sf::Text controlsTitle;
    sf::Text controlsContent; // The big block of text
    sf::Text backButton;

    // Helpers
    void initUI();
    void processEvents();
    void update(sf::Time dt);
    void draw();
    
    // Specific Drawers
    void drawMenu();
    void drawControls();
    void drawHUD(); 
    void drawGame();

    void checkVictory();
    void resetGame(); // Helper to restart

public:
    Game();
    ~Game();
    void run();
};

#endif