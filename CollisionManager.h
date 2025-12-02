#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H

#include "Player.h"
#include "Platform.h"
#include <vector>

class CollisionManager {
public:
    // Spells vs Players
    bool checkHits(Player* attacker, Player* victim);
    
    // Players vs Environment (Walls/Floor)
    void checkEnvironment(Player* player, const std::vector<Platform>& platforms);

    // Player vs Player (Body blocking)
    void checkPlayerCollision(Player* p1, Player* p2);
};

#endif