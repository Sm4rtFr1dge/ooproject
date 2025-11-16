#include "PlayState.h"
#include "MenuState.h"
#include "Game.h"

PlayState::PlayState(Game* game) : State(game) {
    if (!font.loadFromFile("arial.ttf")) {
        // Font failed to load; label may be invisible. Keep going.
    }

    label.setFont(font);
    label.setString("Play State - Press Escape to return");
    label.setCharacterSize(32);
    label.setFillColor(sf::Color::White);

    // Center-ish position; will be adjusted when rendering window
    sf::FloatRect bounds = label.getLocalBounds();
    label.setPosition(100.f, 200.f);
}

PlayState::~PlayState() {}

void PlayState::handleInput(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            game->getWindow().close();  // Just close instead of transitioning
        }
    } else if (event.type == sf::Event::Closed) {
        game->getWindow().close();
    }
}

void PlayState::render(sf::RenderWindow& window) {
    window.draw(label);
}
