#include "Platform.h"

Platform::Platform(sf::Texture& texture, float x, float y, float width, float height) {
    sprite.setTexture(texture);
    sprite.setPosition(x, y);
    
    // Calculate scale based on desired width/height vs image size
    sf::Vector2u textureSize = texture.getSize();
    float scaleX = width / (float)textureSize.x;
    float scaleY = height / (float)textureSize.y;
    
    sprite.setScale(scaleX, scaleY);
}

void Platform::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::FloatRect Platform::getBounds() const {
    return sprite.getGlobalBounds();
}