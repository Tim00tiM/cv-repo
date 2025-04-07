#include "RectComponent.h"

void RectComponent::Draw(sf::RenderWindow &render) {
  auto new_pos = RerenderPos();
  rect_shape.setPosition(new_pos);
  if (isVisible) {
    render.draw(rect_shape);
  }
}

void RectComponent::SetSize(float wdth, float hght) {
  width = wdth;
  height = hght;
  rect_shape.setSize({width, height});
};

void RectComponent::SetFillColor(sf::Color clr) {
  color = clr;
  rect_shape.setFillColor(color);
}

void RectComponent::SetOutlineColor(sf::Color clr) {
  color = clr;
  rect_shape.setOutlineColor(color);
}

void RectComponent::SetOutlineWidth(float o_width) {
  outline_width = o_width;
  rect_shape.setOutlineThickness(outline_width);
};

float RectComponent::GetHeight() { return height; };

float RectComponent::GetWidth() { return width; };

bool RectComponent::Contains(const sf::Vector2f& point) {
  auto rect = rect_shape.getGlobalBounds();
  return rect.contains(point);
}