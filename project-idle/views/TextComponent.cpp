#include "TextComponent.h"

void TextComponent::Draw(sf::RenderWindow &render) {
  auto new_pos = RerenderPos();
  text.setPosition(new_pos);
  if (isVisible) {
    render.draw(text);
  }
}

void TextComponent::SetString(const String &string) {
  this->string = string;
  text.setString(string);
}

void TextComponent::SetTextColor(const Color &textColor) {
  text_color = textColor;
  text.setFillColor(text_color);
}

void TextComponent::SetSize(int size) {
  TextComponent::size = size;
  text.setCharacterSize(size);
}

void TextComponent::SetFont(const Font &font) {
  font_ = font;
  text.setFont(font_);
}

Rect<float> TextComponent::GetGlobalBounds() {
  return text.getGlobalBounds();
}