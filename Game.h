#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Player.h"
#include "CollisionManager.h"
#include "ShockManager.cpp"
#include "Platform.h"

enum GameState {
    STATE_MENU,
    STATE_CONTROLS,
    STATE_PLAYING,
    STATE_GAME_OVER
};

class Game {
private:
    sf::RenderWindow window;
    
    // --- SHADER STUFF ---
    sf::RenderTexture renderTexture; 
    sf::Shader scanlineShader;       
    sf::Sprite finalSprite;          
    
    Player* player1;
    Player* player2;
    CollisionManager collisionManager;
    ShockManager shockManager;
    
    // Restored Standard Background variables
    sf::Texture bgTexture;
    sf::Sprite bgSprite;
    
    sf::Texture platformTexture;
    std::vector<Platform> platforms;

    // --- UI / STATE VARIABLES ---
    GameState currentState;
    sf::Font font;

    sf::Text titleText;
    sf::Text startButton;
    sf::Text controlsButton;
    sf::Text exitButton;

    sf::Text controlsTitle;
    sf::Text controlsContent; 
    sf::Text backButton;

    sf::Text gameOverTitle;
    sf::Text winnerText;
    sf::Text restartButton;
    sf::Text quitButton;

    void initUI();
    void processEvents();
    void update(sf::Time dt);
    void draw();
    
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