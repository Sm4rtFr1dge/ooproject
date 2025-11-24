#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Spell.h"

enum Element { FIRE, WATER, EARTH, LIGHTNING };

class Player {
private:
    // Changed from RectangleShape to Sprite
    sf::Sprite sprite;
    sf::Texture texture;
    
    float hp;
    float maxHp;
    float mp;
    float maxMp;
    float speed;
    int playerId; 
    
    // Status Effects
    float shieldHP;     
    float shieldTimer; 
    float burnTimer;    
    float burnTickTimer;
    float stunTimer;    
    float slowTimer;    
    
    std::vector<Element> elementQueue;
    std::vector<Spell> activeSpells; 

    void castSpell(); 

public:
    Player(float x, float y, sf::Color color, int id);
    
    void handleInput(sf::Event event);
    void update(sf::Time dt);
    void draw(sf::RenderWindow& window);
    
    void takeDamage(float amount);
    void applyStatusEffect(SpellEffect effect);

    float getHP() const;
    float getMaxHP() const;
    float getMP() const;
    float getMaxMP() const;
    float getShieldHP() const;

    void setPosition(float x, float y);
    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;
    
    std::vector<Spell>& getSpells();
};

#endif