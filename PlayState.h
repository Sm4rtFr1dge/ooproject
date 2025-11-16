#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include "State.h"
#include <SFML/Graphics.hpp>

class PlayState : public State {
private:
    sf::Font font;
    sf::Text label;

public:
    PlayState(Game* game);
    ~PlayState() override;

    void onEnter() override {}
    void onExit() override {}

    void handleInput(sf::Event& event) override;
    void update(float dt) override {}
    void render(sf::RenderWindow& window) override;
};

#endif
