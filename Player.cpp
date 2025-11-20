#include "Player.h"
#include <iostream>
#include <algorithm> 
#include <cstdlib>   

Player::Player(float x, float y, sf::Color color, int id) {
    playerId = id;
    body.setSize(sf::Vector2f(40.f, 60.f));
    body.setFillColor(color);
    body.setPosition(x, y);
    body.setOrigin(20.f, 30.f); 
    
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

        if (event.key.code == fireKey) elementQueue.push_back(FIRE);
        if (event.key.code == waterKey) elementQueue.push_back(WATER);
        if (event.key.code == lightKey) elementQueue.push_back(LIGHTNING);
        
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
    // Spawn slightly offset so we don't hit ourselves immediately
    sf::Vector2f spawnPos = body.getPosition() + (dir * 45.f); 

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
                shieldTimer = 5.0f; // NEW: Shield expires in 5 seconds
                spellCast = true;
                std::cout << "Shield Up! (5s duration)" << std::endl;
            }
        }
        else if (elementQueue[0] == FIRE && elementQueue[1] == LIGHTNING) {
            if (mp >= 25) {
                manaCost = 25;
                float newY = body.getPosition().y - 150.f;
                if (newY < 50) newY = 50; 
                body.setPosition(body.getPosition().x, newY);
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

    // NEW: Shield Timer Logic
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
        body.move(movement * currentSpeed * dtSec);
    }

    sf::Vector2f pos = body.getPosition();
    if (pos.x < 20.f) pos.x = 20.f;
    if (pos.x > 780.f) pos.x = 780.f;
    if (pos.y < 30.f) pos.y = 30.f;
    if (pos.y > 570.f) pos.y = 570.f;
    body.setPosition(pos);

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
    if (shieldHP > 0) body.setOutlineThickness(3);
    else body.setOutlineThickness(0);
    
    if (stunTimer > 0) body.setFillColor(sf::Color(100, 100, 100)); 
    else if (burnTimer > 0) body.setFillColor(sf::Color(255, 100, 0)); 
    else if (slowTimer > 0) body.setFillColor(sf::Color(0, 255, 255, 150)); 
    else {
        if (playerId == 1) body.setFillColor(sf::Color::Blue);
        else body.setFillColor(sf::Color::Red);
    }

    if (shieldHP > 0) body.setOutlineColor(sf::Color::White);

    window.draw(body);
    
    for (auto& spell : activeSpells) {
        spell.draw(window);
    }

    for (size_t i = 0; i < elementQueue.size(); i++) {
        sf::CircleShape indicator(5.f);
        indicator.setPosition(body.getPosition().x - 15 + (i * 12), body.getPosition().y - 50);
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
sf::FloatRect Player::getBounds() const { return body.getGlobalBounds(); }
std::vector<Spell>& Player::getSpells() { return activeSpells; }
void Player::setPosition(float x, float y) { body.setPosition(x, y); }
sf::Vector2f Player::getPosition() const { return body.getPosition(); }