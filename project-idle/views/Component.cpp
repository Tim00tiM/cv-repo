#include "Component.h"

sf::Vector2<float> Component::GetPos() {
  auto new_pos = RerenderPos();
  return new_pos;
}


sf::Vector2f Component::RerenderPos() {
  if (anchor != nullptr) {
    pos = anchor->GetPos();
  }
  return pos + GetMargin();
}

void Component::SetMargin(std::function<sf::Vector2f()> new_margin_func) {
  GetMargin = new_margin_func;
}

void Component::SetPos(const sf::Vector2<float>& pos) {
  this->pos = pos;
}

void Component::SetPos(float x, float y) {
  pos.x = x;
  pos.y = y;
}

bool Component::GetVisibility() {
  return isVisible;
}

void Component::SetVisibility(bool new_isVisible) {
  isVisible =  new_isVisible;
}