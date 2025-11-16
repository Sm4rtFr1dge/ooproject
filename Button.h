#ifndef BUTTON_H
#define BUTTON_H

#include <SFML/Graphics.hpp>

class Button {
private:
    sf::RectangleShape shape;
    sf::Text label;
    sf::Font font;  // Store font to keep it alive

public:
    Button(sf::Vector2f pos, sf::Vector2f size, const sf::Font& font, const std::string& text);

    bool isHovered(sf::Vector2i mousePos);
    bool isClicked(sf::Vector2i mousePos, bool mousePressed);

    void draw(sf::RenderWindow& window);
};

#endif
