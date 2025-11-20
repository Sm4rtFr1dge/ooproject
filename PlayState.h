#pragma once
#include "State.h"
#include <vector>

struct Spell {
    sf::CircleShape shape;
    sf::Vector2f pos, vel;
    int damage, owner;
};

class Player; // forward

class PlayState : public State {
public:
    PlayState(Game* game);
    void handleEvent(sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

private:
    Player p1, p2;
    std::vector<Spell> spells;
    sf::Font font;
    sf::Text winnerText;
    bool gameOver = false;

    void castSpell(Player& caster);
};