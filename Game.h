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

public:
    Game();
    ~Game();
    void run();
};

#endif