#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>

class State; // Forward declaration

class Game {
private:
    sf::RenderWindow window;
    State* currentState;
    State* nextState;
    bool running = true;

public:
    Game();
    ~Game();

    void run();
    void changeState(State* newState);
    void queueStateChange(State* newState);

    sf::RenderWindow& getWindow() { return window; }
    bool isRunning() const { return running; }
    void quit() { running = false; }
};

#endif
