#include "CollisionManager.h"
#include <iostream>

bool CollisionManager::checkHits(Player* attacker, Player* victim) {
    std::vector<Spell>& spells = attacker->getSpells();
    bool hitDetected = false;

    for (auto& spell : spells) {
        if (spell.isActive()) {
            if (spell.getBounds().intersects(victim->getBounds())) {
                // 1. Deal Damage
                victim->takeDamage(spell.getDamage());
                
                // 2. Apply Status Effect (NEW)
                victim->applyStatusEffect(spell.getEffect());

                spell.deactivate();
                hitDetected = true;
            }
        }
    }
    return hitDetected;
}

void CollisionManager::checkEnvironment(Player* player, const std::vector<Platform>& platforms) {
    sf::FloatRect playerBounds = player->getBounds();
    
    for (const auto& platform : platforms) {
        if (playerBounds.intersects(platform.getBounds())) {
            sf::Vector2f pPos = player->getPosition();
            sf::FloatRect wall = platform.getBounds();
            float wallCenterY = wall.top + wall.height/2;
            float wallCenterX = wall.left + wall.width/2;

            float dx = pPos.x - wallCenterX;
            float dy = pPos.y - wallCenterY;
            
            if (std::abs(dx) > std::abs(dy)) {
                player->setPosition(pPos.x + (dx > 0 ? 2.0f : -2.0f), pPos.y);
            } else {
                player->setPosition(pPos.x, pPos.y + (dy > 0 ? 2.0f : -2.0f));
            }
        }
    }

    for (auto& spell : player->getSpells()) {
        if (spell.isActive()) {
            for (const auto& platform : platforms) {
                if (spell.getBounds().intersects(platform.getBounds())) {
                    spell.deactivate(); 
                }
            }
        }
    }
}