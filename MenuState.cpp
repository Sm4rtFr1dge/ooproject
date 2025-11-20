#include "MenuState.h"
#include "PlayState.h"
#include "Game.h"

MenuState::MenuState(Game* game) : State(game) {
    if (!font.loadFromFile("arial.ttf")) {
        fprintf(stderr, "Failed to load font in MenuState\n");
    }

    startButton = new Button({500, 250}, {300, 70}, font, "Start Game");
    exitButton  = new Button({500, 350}, {300, 70}, font, "Exit");
}

MenuState::~MenuState() {
    delete startButton;
    delete exitButton;
}

void MenuState::handleInput(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mp = sf::Mouse::getPosition(game->getWindow());

        if (startButton->isClicked(mp, true))
            game->queueStateChange(new PlayState(game));

        if (exitButton->isClicked(mp, true))
            game->getWindow().close();
    }
}

void MenuState::render(sf::RenderWindow& window) {
    startButton->draw(window);
    exitButton->draw(window);
}
