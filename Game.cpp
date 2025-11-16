#include "Game.h"
#include "State.h"
#include "MenuState.h"

Game::Game()
    : window(sf::VideoMode(1280, 720), "Magic Combat Game")
    , currentState(nullptr)
    , nextState(nullptr)
{
    window.setFramerateLimit(60);

    // Start with Menu Screen
    changeState(new MenuState(this));
}

Game::~Game() {
    if (currentState)
        delete currentState;
}

void Game::run() {
    sf::Clock clock;

    while (window.isOpen() && running) {

        sf::Event event;
        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                window.close();

            if (currentState)
                currentState->handleInput(event);
        }

        // Process deferred state change after event handling is complete
        if (nextState) {
            changeState(nextState);
            nextState = nullptr;
        }

        float dt = clock.restart().asSeconds();

        if (currentState)
            currentState->update(dt);

        window.clear();
        if (currentState)
            currentState->render(window);
        window.display();
    }
}

void Game::changeState(State* newState) {
    if (currentState) {
        currentState->onExit();
        delete currentState;
    }

    currentState = newState;

    if (currentState)
        currentState->onEnter();
}

void Game::queueStateChange(State* newState) {
    nextState = newState;
}
