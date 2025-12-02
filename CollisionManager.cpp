#include "CollisionManager.h"
#include <cmath> // For std::abs

bool CollisionManager::checkHits(Player* attacker, Player* victim) {
    std::vector<Spell>& spells = attacker->getSpells();
    bool hitDetected = false;

    for (auto& spell : spells) {
        if (spell.isActive()) {
            if (spell.getBounds().intersects(victim->getBounds())) {
                victim->takeDamage(spell.getDamage());
                victim->applyStatusEffect(spell.getEffect()); // Apply burn/stun
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
            
            float wallCenterY = wall.top + wall.height / 2;
            float wallCenterX = wall.left + wall.width / 2;
            float dx = pPos.x - wallCenterX;
            float dy = pPos.y - wallCenterY;

            // Calculate overlap amounts
            float overlapX = (playerBounds.width/2 + wall.width/2) - std::abs(dx);
            float overlapY = (playerBounds.height/2 + wall.height/2) - std::abs(dy);

            // Resolve on the shallowest axis
            if (overlapY < overlapX) {
                if (dy < 0) player->setPosition(pPos.x, pPos.y - overlapY);
                else player->setPosition(pPos.x, pPos.y + overlapY);
            } else {
                if (dx < 0) player->setPosition(pPos.x - overlapX, pPos.y);
                else player->setPosition(pPos.x + overlapX, pPos.y);
            }
        }
    }
}

// --- Player vs Player Body Blocking ---
void CollisionManager::checkPlayerCollision(Player* p1, Player* p2) {
    sf::FloatRect b1 = p1->getBounds();
    sf::FloatRect b2 = p2->getBounds();

    if (b1.intersects(b2)) {
        sf::Vector2f pos1 = p1->getPosition();
        sf::Vector2f pos2 = p2->getPosition();

        float dx = pos1.x - pos2.x;
        float dy = pos1.y - pos2.y;

        // Calculate overlap
        float overlapX = (b1.width / 2 + b2.width / 2) - std::abs(dx);
        float overlapY = (b1.height / 2 + b2.height / 2) - std::abs(dy);

        // Push apart on the axis with the least overlap 
        if (overlapX < overlapY) {
            float push = overlapX / 2.0f;
            if (dx > 0) { // P1 is to the right of P2
                p1->setPosition(pos1.x + push, pos1.y);
                p2->setPosition(pos2.x - push, pos2.y);
            } else {      // P1 is to the left
                p1->setPosition(pos1.x - push, pos1.y);
                p2->setPosition(pos2.x + push, pos2.y);
            }
        } else {
            float push = overlapY / 2.0f;
            if (dy > 0) { // P1 is below P2
                p1->setPosition(pos1.x, pos1.y + push);
                p2->setPosition(pos2.x, pos2.y - push);
            } else {      // P1 is above P2
                p1->setPosition(pos1.x, pos1.y - push);
                p2->setPosition(pos2.x, pos2.y + push);
            }
        }
    }
}