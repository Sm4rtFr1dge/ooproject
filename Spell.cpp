#include "Spell.h"

Spell::Spell(float startX, float startY, sf::Vector2f dir, sf::Color color, float dmg, SpellEffect effect) {
    shape.setRadius(10.f);
    shape.setFillColor(color);
    shape.setPosition(startX, startY);
    shape.setOrigin(10.f, 10.f);
    
    float speed = 450.f; 
    velocity = dir * speed;
    damage = dmg;
    effectType = effect;
    active = true;
    lifetime = 3.0f; // Spells last 3 seconds before disappearing naturally
}

void Spell::update(sf::Time dt) {
    if (!active) return;
    
    // 1. Move
    shape.move(velocity * dt.asSeconds());

    // 2. Screen Wrapping 
    sf::Vector2f pos = shape.getPosition();
    bool wrapped = false;

    if (pos.x < 0) { 
        pos.x = 800.f; 
        wrapped = true; 
    }

    if (pos.x > 800) {pos.x = 0.f; 
        wrapped = true; 
    }
    if (pos.y < 0) { 
        pos.y = 600.f; 
        wrapped = true; 
    }

    if (pos.y > 600) { 
        pos.y = 0.f; 
        wrapped = true; 
    }

    if (wrapped) {
        shape.setPosition(pos);
    }

    // 3. Lifetime Management
    // Wrapped spells should eventually fade so the game doesn't crash from too many objects
    lifetime -= dt.asSeconds();
    if (lifetime <= 0) {
        active = false;
    }
}

void Spell::draw(sf::RenderWindow& window) {
    if (active) window.draw(shape);
}

bool Spell::isActive() const { return active; }
void Spell::deactivate() { active = false; }
sf::FloatRect Spell::getBounds() const { return shape.getGlobalBounds(); }
float Spell::getDamage() const { return damage; }
SpellEffect Spell::getEffect() const { return effectType; }