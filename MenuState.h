#ifndef MENUSTATE_H
#define MENUSTATE_H

#include "State.h"
#include "Button.h"

class MenuState : public State {
private:
    sf::Font font;
    Button* startButton;
    Button* exitButton;

public:
    MenuState(Game* game);
    ~MenuState();

    void onEnter() override {}
    void onExit() override {}

    void handleInput(sf::Event& event) override;
    void update(float dt) override {}
    void render(sf::RenderWindow& window) override;
};

#endif
