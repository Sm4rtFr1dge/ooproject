#include "Player.h"
#include <iostream>
#include <algorithm> 
#include <cstdlib>   

Player::Player(float x, float y, sf::Color color, int id) {
    playerId = id;
    
    sf::Image img;
    bool loaded = false;
    std::string filename = (playerId == 1) ? "player1.png" : "player2.png";

    if (img.loadFromFile(filename)) {
        loaded = true;

        // --- SMART BACKGROUND REMOVAL (Restored) ---
        // 1. Detect background color from top-left pixel
        sf::Color bgColor = img.getPixel(0, 0);
        int threshold = 50; // Tolerance for "dirty" background pixels

        for (unsigned int i = 0; i < img.getSize().x; ++i) {
            for (unsigned int j = 0; j < img.getSize().y; ++j) {
                sf::Color pixel = img.getPixel(i, j);

                // Calculate difference from background color
                int diffR = std::abs((int)pixel.r - (int)bgColor.r);
                int diffG = std::abs((int)pixel.g - (int)bgColor.g);
                int diffB = std::abs((int)pixel.b - (int)bgColor.b);

                bool isBackground = (diffR < threshold && diffG < threshold && diffB < threshold);
                
                // Also catch bright white/grey artifacts (grid lines)
                bool isArtifact = (pixel.r > 200 && pixel.g > 200 && pixel.b > 200);

                if (isBackground || isArtifact) {
                    img.setPixel(i, j, sf::Color::Transparent);
                }
            }
        }

        texture.loadFromImage(img);
        texture.setSmooth(false); // Keep pixel art sharp
    } else {
        std::cerr << "Error loading " << filename << std::endl;
        texture.create(40, 60);
        sf::Image fallbackImg;
        fallbackImg.create(40, 60, color);
        texture.update(fallbackImg);
    }

    sprite.setTexture(texture);
    
    // Dynamic Scaling logic (Keep this to prevent "Massive Sprite" issue)
    float targetHeight = 100.0f; 
    float currentHeight = (float)texture.getSize().y;
    float scaleFactor = (currentHeight > 0) ? targetHeight / currentHeight : 1.0f;

    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    
    sprite.setPosition(x, y);

    // Flipping Logic
    if (playerId == 2) {
        sprite.setScale(-scaleFactor, scaleFactor); 
    } else {
        sprite.setScale(scaleFactor, scaleFactor);
    }
    
    maxHp = 100.f;
    hp = maxHp;
    maxMp = 100.f;
    mp = maxMp;
    speed = 200.f;

    shieldHP = 0.f;
    shieldTimer = 0.f;
    burnTimer = 0.f;
    burnTickTimer = 0.f;
    stunTimer = 0.f;
    slowTimer = 0.f;
}

void Player::handleInput(sf::Event event) {
    if (stunTimer > 0) return;

    if (event.type == sf::Event::KeyPressed) {
        int fireKey = (playerId == 1) ? sf::Keyboard::Num1 : sf::Keyboard::I;
        int waterKey = (playerId == 1) ? sf::Keyboard::Num2 : sf::Keyboard::O;
        int lightKey = (playerId == 1) ? sf::Keyboard::Num3 : sf::Keyboard::P;
        int castKey = (playerId == 1) ? sf::Keyboard::Space : sf::Keyboard::Enter;

        // Queue Limit (Max 3)
        if (elementQueue.size() < 3) {
            if (event.key.code == fireKey) elementQueue.push_back(FIRE);
            if (event.key.code == waterKey) elementQueue.push_back(WATER);
            if (event.key.code == lightKey) elementQueue.push_back(LIGHTNING);
        }
        
        if (event.key.code == castKey) {
            castSpell();
        }
    }
}

void Player::castSpell() {
    if (elementQueue.empty()) return;
    std::sort(elementQueue.begin(), elementQueue.end());

    float manaCost = 0;
    bool spellCast = false;

    sf::Vector2f dir = (playerId == 1) ? sf::Vector2f(1.f, 0.f) : sf::Vector2f(-1.f, 0.f);
    
    // Spawn offset 80.f to avoid self-hit
    sf::Vector2f spawnPos = sprite.getPosition() + (dir * 80.f); 

    if (elementQueue.size() == 3 && 
        elementQueue[0] == FIRE && elementQueue[1] == WATER && elementQueue[2] == LIGHTNING) {
        if (mp >= 50) {
            manaCost = 50;
            activeSpells.emplace_back(spawnPos.x, spawnPos.y, dir, sf::Color::White, 40.f, EFFECT_STUN_ULTIMATE);
            spellCast = true;
        }
    }
    else if (elementQueue.size() == 2) {
        if (elementQueue[0] == FIRE && elementQueue[1] == WATER) {
            if (mp >= 20) {
                manaCost = 20;
                shieldHP = 10.f; 
                shieldTimer = 5.0f; 
                spellCast = true;
                std::cout << "Shield Up! (5s duration)" << std::endl;
            }
        }
        else if (elementQueue[0] == FIRE && elementQueue[1] == LIGHTNING) {
            if (mp >= 25) {
                manaCost = 25;
                float newY = sprite.getPosition().y - 150.f;
                if (newY < 50) newY = 50; 
                sprite.setPosition(sprite.getPosition().x, newY);
                spellCast = true;
            }
        }
        else if (elementQueue[0] == WATER && elementQueue[1] == LIGHTNING) {
            if (mp >= 20) {
                manaCost = 20;
                activeSpells.emplace_back(spawnPos.x, spawnPos.y, dir, sf::Color::Cyan, 15.f, EFFECT_SLOW);
                spellCast = true;
            }
        }
    }
    else if (elementQueue.size() == 1) {
        if (mp >= 10) {
            manaCost = 10;
            spellCast = true;
            Element e = elementQueue[0];
            if (e == FIRE) activeSpells.emplace_back(spawnPos.x, spawnPos.y, dir, sf::Color::Red, 10.f, EFFECT_BURN);
            else if (e == WATER) activeSpells.emplace_back(spawnPos.x, spawnPos.y, dir, sf::Color::Blue, 10.f, EFFECT_NONE);
            else if (e == LIGHTNING) activeSpells.emplace_back(spawnPos.x, spawnPos.y, dir, sf::Color::Yellow, 10.f, EFFECT_PARALYZE_CHANCE);
        }
    }

    if (spellCast) {
        mp -= manaCost;
        elementQueue.clear();
    } else {
        elementQueue.clear(); 
    }
}

void Player::update(sf::Time dt) {
    float dtSec = dt.asSeconds();

    if (shieldHP > 0) {
        shieldTimer -= dtSec;
        if (shieldTimer <= 0) {
            shieldHP = 0;
            std::cout << "Shield Expired!" << std::endl;
        }
    }

    if (stunTimer > 0) {
        stunTimer -= dtSec;
        if (stunTimer < 0) stunTimer = 0;
    }

    if (burnTimer > 0) {
        burnTimer -= dtSec;
        burnTickTimer += dtSec;
        if (burnTickTimer >= 1.0f) {
            takeDamage(2.f); 
            burnTickTimer = 0.f;
        }
    }

    float currentSpeed = speed;
    if (slowTimer > 0) {
        slowTimer -= dtSec;
        currentSpeed = speed * 0.5f;
    }

    if (stunTimer <= 0) {
        sf::Vector2f movement(0.f, 0.f);
        if (playerId == 1) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) movement.y -= 1.f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) movement.y += 1.f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) movement.x -= 1.f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) movement.x += 1.f;
        } else {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) movement.y -= 1.f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) movement.y += 1.f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) movement.x -= 1.f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) movement.x += 1.f;
        }
        sprite.move(movement * currentSpeed * dtSec);
    }

    // --- BOUNDARY CHECK (UPDATED) ---
    sf::Vector2f pos = sprite.getPosition();
    if (pos.x < 20.f) pos.x = 20.f;
    if (pos.x > 780.f) pos.x = 780.f;
    if (pos.y < 30.f) pos.y = 30.f;
    
    // FIX: Set to 550.f so Feet (y+50) touch 600.f
    if (pos.y > 550.f) pos.y = 550.f;
    
    sprite.setPosition(pos);

    if (mp < maxMp) mp += 5.f * dtSec;

    for (auto& spell : activeSpells) {
        spell.update(dt);
    }
}

void Player::takeDamage(float amount) {
    if (shieldHP > 0) {
        if (amount <= shieldHP) {
            shieldHP -= amount;
            amount = 0;
        } else {
            amount -= shieldHP;
            shieldHP = 0;
        }
    }
    hp -= amount;
    if (hp < 0) hp = 0;
}

void Player::applyStatusEffect(SpellEffect effect) {
    if (shieldHP > 0 && effect != EFFECT_NONE) return;

    switch (effect) {
        case EFFECT_BURN: burnTimer = 2.0f; burnTickTimer = 0.5f; break;
        case EFFECT_PARALYZE_CHANCE: if ((rand() % 100) < 10) stunTimer = 1.0f; break;
        case EFFECT_SLOW: slowTimer = 2.0f; break;
        case EFFECT_STUN_ULTIMATE: stunTimer = 1.5f; break;
        default: break;
    }
}

void Player::draw(sf::RenderWindow& window) {
    // Status Visuals
    if (stunTimer > 0) sprite.setColor(sf::Color(100, 100, 100)); 
    else if (burnTimer > 0) sprite.setColor(sf::Color(255, 100, 100)); 
    else if (slowTimer > 0) sprite.setColor(sf::Color(150, 255, 255)); 
    else sprite.setColor(sf::Color::White); 

    // Shield (Thick Outline)
    if (shieldHP > 0) {
        sf::CircleShape shield(45.f);
        shield.setOrigin(45.f, 45.f);
        shield.setPosition(sprite.getPosition());
        shield.setFillColor(sf::Color(255, 255, 255, 80)); 
        shield.setOutlineColor(sf::Color::White);
        shield.setOutlineThickness(5.f); 
        window.draw(shield);
    }

    window.draw(sprite);
    
    for (auto& spell : activeSpells) {
        spell.draw(window);
    }

    for (size_t i = 0; i < elementQueue.size(); i++) {
        sf::CircleShape indicator(5.f);
        indicator.setPosition(sprite.getPosition().x - 15 + (i * 12), sprite.getPosition().y - 60);
        switch(elementQueue[i]) {
            case FIRE: indicator.setFillColor(sf::Color::Red); break;
            case WATER: indicator.setFillColor(sf::Color::Blue); break;
            case LIGHTNING: indicator.setFillColor(sf::Color::Yellow); break;
            default: indicator.setFillColor(sf::Color::White);
        }
        window.draw(indicator);
    }
}

float Player::getHP() const { return hp; }
float Player::getMaxHP() const { return maxHp; }
float Player::getMP() const { return mp; }
float Player::getMaxMP() const { return maxMp; }
float Player::getShieldHP() const { return shieldHP; }
int Player::getPlayerId() const { return playerId; }

sf::FloatRect Player::getBounds() const { return sprite.getGlobalBounds(); }
std::vector<Spell>& Player::getSpells() { return activeSpells; }
void Player::setPosition(float x, float y) { sprite.setPosition(x, y); }
sf::Vector2f Player::getPosition() const { return sprite.getPosition(); }