#pragma once

#include "Component.h"

class RectComponent : public Component {
private:
  float width = 0;
  float height = 0;
  float x = 0;
  float y = 0;
  sf::Color color = sf::Color::Black;
  sf::Color outline_color = sf::Color::Transparent;
  float outline_width = 0;
  
  sf::RectangleShape rect_shape;
  
public:
  RectComponent(const sf::Vector2<float>& pos) : Component(pos) {
    rect_shape = sf::RectangleShape();
    rect_shape.setSize({0.f, 0.f});
    rect_shape.setFillColor(sf::Color::Black);
    rect_shape.setOutlineColor(sf::Color::Transparent);
    rect_shape.setOutlineThickness(0.f);
    rect_shape.setPosition(0.f, 0.f);
  };

  RectComponent(const std::shared_ptr<Component>& anchr) : RectComponent(anchr->GetPos()) {
    anchor = anchr;
    auto new_pos = RerenderPos();
    rect_shape.setPosition(new_pos);
  };

  RectComponent() : Component(sf::Vector2f(0, 0)) {
    rect_shape = sf::RectangleShape();
  }
  
  void SetSize(float width, float height);
  void SetFillColor(sf::Color color);
  void SetOutlineColor(sf::Color color);
  void SetOutlineWidth(float o_width);

  float GetWidth();
  float GetHeight();
  virtual void Draw(sf::RenderWindow& render) override;

  bool Contains(const sf::Vector2f& point);
  
};
