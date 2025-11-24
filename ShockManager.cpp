#ifndef SHOCKMANAGER_H
#define SHOCKMANAGER_H

#include <iostream>

class ShockManager {
public:
    // Call this function when a player gets hit
    void sendShock(int intensity) {
        // --- TODO: Add Arduino Serial Code Here ---
        // Example logic:
        // 1. Open Serial Port (COM3 etc)
        // 2. Write 'intensity' byte
        
        // For now, we print to console to prove it works
        std::cout << ">>> SHOCK TRIGGERED! Intensity: " << intensity << " <<<" << std::endl;
    }
};

#endif