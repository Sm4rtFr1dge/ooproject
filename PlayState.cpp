// PlayState.cpp (includes Player class)
#include "PlayState.h"
#include "MenuState.h"
#include "Game.h"
#include <iostream>
#include <sstream>
#include <algorithm>

// ================== ELEMENT & PLAYER ==================
enum Element { FIRE, WATER, EARTH, LIGHTNING };

const sf::Color ELEMENT_COLORS[] = {
    sf::Color::Red,
    sf::Color::Blue,
    sf::Color(139, 69, 19),
    sf::Color::Yellow
};

class Player {
public:
    Player(bool leftSide);
    void handleInput(sf::Event& e);
    std::vector<Element> consumeQueue();
    void update(float dt);
    void render(sf::RenderWindow& w);
    void takeDamage(int dmg) { hp -= dmg; if (hp < 0) hp = 0; }

    sf::Vector2f pos;
    sf::RectangleShape body;
    int hp = 100, mp = 100;
    bool isLeftSide;
    int id;

private:
    std::queue<Element> queue;
};

Player::Player(bool leftSide) : isLeftSide(leftSide), id(leftSide ? 1 : 2) {
    pos = leftSide ? sf::Vector2f(200, 400) : sf::Vector2f(1080, 400);
    body.setSize({80, 140});
    body.setFillColor(leftSide ? sf::Color(0, 100, 255) : sf::Color(255, 50, 50));
    body.setOutlineThickness(5);
    body.setOutlineColor(sf::Color::White);
    body.setPosition(pos);
}

void Player::handleInput(sf::Event& e) {
    if (e.type != sf::Event::KeyPressed) return;
    Element el = FIRE;
    if (isLeftSide) {
        if (e.key.code == sf::Keyboard::A) el = FIRE;
        else if (e.key.code == sf::Keyboard::S) el = WATER;
        else if (e.key.code == sf::Keyboard::D) el = EARTH;
        else if (e.key.code == sf::Keyboard::F) el = LIGHTNING;
        else return;
    } else {
        if (e.key.code == sf::Keyboard::J) el = FIRE;
        else if (e.key.code == sf::Keyboard::K) el = WATER;
        else if (e.key.code == sf::Keyboard::L) el = EARTH;
        else if (e.key.code == sf::Keyboard::Semicolon) el = LIGHTNING;
        else return;
    }
    if (queue.size() < 4) queue.push(el);
}

std::vector<Element> Player::consumeQueue() {
    std::vector<Element> v;
    while (!queue.empty()) { v.push_back(queue.front()); queue.pop(); }
    return v;
}

void Player::update(float dt) {
    mp += 8 * dt; if (mp > 100) mp = 100;

    float speed = 300;
    sf::Vector2f move(0, 0);
    if (isLeftSide) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) move.y -= speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) move.y += speed;
    } else {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) move.y -= speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) move.y += speed;
    }
    pos += move * dt;
    pos.y = std::clamp(pos.y, 100.f, 520.f);
    body.setPosition(pos);
}

void Player::render(sf::RenderWindow& w) {
    w.draw(body);

    // HP Bar
    sf::RectangleShape bar({160, 20});
    bar.setPosition(pos.x - 80, pos.y - 100);
    bar.setFillColor(sf::Color::Black); w.draw(bar);
    bar.setSize({160 * hp / 100.f, 20});
    bar.setFillColor(sf::Color::Red); w.draw(bar);

    // MP Bar
    bar.setSize({160, 12});
    bar.setPosition(pos.x - 80, pos.y - 75);
    bar.setFillColor(sf::Color::Black); w.draw(bar);
    bar.setSize({160 * mp / 100.f, 12});
    bar.setFillColor(sf::Color::Cyan); w.draw(bar);

    // Queued elements
    auto q = queue;
    float x = pos.x - 60;
    int i = 0;
    while (!q.empty()) {
        sf::CircleShape c(15);
        c.setFillColor(ELEMENT_COLORS[q.front()]);
        c.setOutlineThickness(3); c.setOutlineColor(sf::Color::White);
        c.setPosition(x + i*40, pos.y - 130);
        w.draw(c);
        q.pop(); i++;
    }
}

// ================== PLAYSTATE ==================
PlayState::PlayState(Game* g) : State(g), p1(true), p2(false) {
    font.loadFromFile("arial.ttf");
    winnerText.setFont(font);
    winnerText.setCharacterSize(80);
    winnerText.setFillColor(sf::Color::Yellow);
    winnerText.setStyle(sf::Text::Bold);
}

void PlayState::handleEvent(sf::Event& e) {
    if (gameOver) {
        if (e.type == sf::Event::KeyPressed) {
            if (e.key.code == sf::Keyboard::R) game->changeState(std::make_unique<PlayState>(game));
            if (e.key.code == sf::Keyboard::Escape) game->changeState(std::make_unique<MenuState>(game));
        }
        return;
    }

    if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape)
        game->changeState(std::make_unique<MenuState>(game));

    p1.handleInput(e);
    p2.handleInput(e);

    if (e.type == sf::Event::KeyPressed) {
        if (e.key.code == sf::Keyboard::Space && !p1.consumeQueue().empty()) castSpell(p1);
        if (e.key.code == sf::Keyboard::Enter && !p2.consumeQueue().empty()) castSpell(p2);
    }
}

void PlayState::castSpell(Player& caster) {
    auto combo = caster.consumeQueue();
    int cost = combo.size() == 4 ? 100 : 10 + (combo.size()-1)*20;
    if (caster.mp < cost) return;
    caster.mp -= cost;

    Spell s;
    s.owner = caster.id;
    s.pos = caster.pos + sf::Vector2f(caster.isLeftSide ? 80 : -80, -40);
    s.vel = sf::Vector2f(caster.isLeftSide ? 900 : -900, 0);
    s.damage = 15 + combo.size() * 20;
    if (combo.size() == 4) s.damage = 100;

    s.shape.setRadius(combo.size() >= 3 ? 40 : 22);
    s.shape.setFillColor(sf::Color::Magenta);
    if (combo.size() == 1) s.shape.setFillColor(ELEMENT_COLORS[combo[0]]);
    s.shape.setOrigin(s.shape.getRadius(), s.shape.getRadius());
    s.shape.setPosition(s.pos);
    s.shape.setOutlineThickness(4);
    s.shape.setOutlineColor(sf::Color::White);

    spells.push_back(s);
}

void PlayState::update(float dt) {
    if (gameOver) return;
    p1.update(dt); p2.update(dt);

    for (auto& s : spells) s.pos += s.vel * dt;
    for (auto& s : spells) s.shape.setPosition(s.pos);

    for (auto it = spells.begin(); it != spells.end(); ) {
        bool hit = false;
        if (it->owner == 1 && p2.body.getGlobalBounds().intersects(it->shape.getGlobalBounds())) {
            p2.takeDamage(it->damage); hit = true;
        }
        if (it->owner == 2 && p1.body.getGlobalBounds().intersects(it->shape.getGlobalBounds())) {
            p1.takeDamage(it->damage); hit = true;
        }
        if (hit || it->pos.x < -100 || it->pos.x > 1380) it = spells.erase(it);
        else ++it;
    }

    if (p1.hp <= 0 || p2.hp <= 0) {
        gameOver = true;
        winnerText.setString(p1.hp <= 0 ? "RED WINS!" : "BLUE WINS!");
        winnerText.setPosition(400, 300);
    }
}

void PlayState::render(sf::RenderWindow& w) {
    sf::RectangleShape ground({1280, 200});
    ground.setPosition(0, 520);
    ground.setFillColor(sf::Color(80, 40, 20));
    w.draw(ground);

    p1.render(w);
    p2.render(w);
    for (const auto& s : spells) w.draw(s.shape);

    if (gameOver) {
        sf::RectangleShape overlay({1280, 720});
        overlay.setFillColor(sf::Color(0, 0, 0, 180));
        w.draw(overlay);
        w.draw(winnerText);
        sf::Text t("R = Restart | ESC = Menu", font, 40);
        t.setPosition(400, 500); t.setFillColor(sf::Color::White);
        w.draw(t);
    }
}