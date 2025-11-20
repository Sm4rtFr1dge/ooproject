#ifndef SPELL_H
#define SPELL_H

#include <SFML/Graphics.hpp>

// Define effects that spells can carry
enum SpellEffect { 
    EFFECT_NONE, 
    EFFECT_BURN,            
    EFFECT_PARALYZE_CHANCE, 
    EFFECT_SLOW,            
    EFFECT_STUN_ULTIMATE    
};

class Spell {
private:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float damage;
    bool active;
    SpellEffect effectType;
    float lifetime; // NEW: Spells die after a while to prevent lag

public:
    Spell(float startX, float startY, sf::Vector2f dir, sf::Color color, float dmg, SpellEffect effect);
    
    void update(sf::Time dt);
    void draw(sf::RenderWindow& window);
    
    bool isActive() const;
    void deactivate();
    sf::FloatRect getBounds() const;
    float getDamage() const;
    SpellEffect getEffect() const;
};

#endif