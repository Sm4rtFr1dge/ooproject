#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H

#include "Player.h"
#include "Platform.h"
#include <vector>

class CollisionManager {
public:
    bool checkHits(Player* attacker, Player* victim);
    
    // New function for platforms
    void checkEnvironment(Player* player, const std::vector<Platform>& platforms);
};

#endif