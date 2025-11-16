#ifndef STATE_H
#define STATE_H

#include <SFML/Graphics.hpp>

class Game;

class State {
protected:
    Game* game;

public:
    State(Game* game) : game(game) {}
    virtual ~State() = default;

    // Called when entering this state
    virtual void onEnter() {}

    // Called when leaving this state
    virtual void onExit() {}

    // Handle keyboard, mouse, window events
    virtual void handleInput(sf::Event& event) = 0;

    // Update game logic, timers, animations
    virtual void update(float dt) = 0;

    // Render sprites, UI, etc.
    virtual void render(sf::RenderWindow& window) = 0;
};

#endif
