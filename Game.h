#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Player.h"
#include "CollisionManager.h"
#include "ShockManager.cpp"
#include "Platform.h"

// Define Game States
enum GameState {
    STATE_MENU,
    STATE_CONTROLS,
    STATE_PLAYING,
    STATE_GAME_OVER
};

class Game {
private:
    sf::RenderWindow window;
    
    Player* player1;
    Player* player2;
    CollisionManager collisionManager;
   
    ShockManager shockManager{"/dev/ttyACM0"};

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
    sf::Text controlsContent; 
    sf::Text backButton;

    // Game Over Elements
    sf::Text gameOverTitle;
    sf::Text winnerText;
    sf::Text restartButton;
    sf::Text quitButton;

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
    void drawGameOver(); 

    void checkVictory();
    void resetGame(); 

public:
    Game();
    ~Game();
    void run();
};

#endif