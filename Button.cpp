#include "Button.h"

Button::Button(sf::Vector2f pos, sf::Vector2f size, const sf::Font& font, const std::string& text) {
    
    shape.setPosition(pos);
    shape.setSize(size);
    shape.setFillColor(sf::Color(40, 40, 40));

    label.setFont(font);
    label.setString(text);
    label.setCharacterSize(28);
    label.setFillColor(sf::Color::White);
    label.setPosition(pos.x + 20, pos.y + 10);
}

bool Button::isHovered(sf::Vector2i mousePos) {
    return shape.getGlobalBounds().contains(mousePos.x, mousePos.y);
}

bool Button::isClicked(sf::Vector2i mousePos, bool mousePressed) {
    return mousePressed && isHovered(mousePos);
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(label);
}
