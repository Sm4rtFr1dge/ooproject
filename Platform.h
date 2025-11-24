#ifndef PLATFORM_H
#define PLATFORM_H

#include <SFML/Graphics.hpp>

class Platform {
private:
    sf::Sprite sprite;

public:
    // We pass the texture by reference so we don't load it multiple times
    Platform(sf::Texture& texture, float x, float y, float width, float height);
    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
};

#endif