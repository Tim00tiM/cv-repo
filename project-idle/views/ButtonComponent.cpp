#include "ButtonComponent.h"

void ButtonComponent::Draw(sf::RenderWindow& render) {

  auto new_pos = RerenderPos();

  content->SetPos(new_pos);

  content->HandleEvent();

  if (isVisible) {
    content->Draw(render);
  }
};

void ButtonComponent::SetCalcWidth(std::function<sf::Vector2f(const sf::Vector2f &)> new_calc_function) {
  CalcWidthHeight = new_calc_function;
}

std::shared_ptr<Component>& ButtonComponent::GetContentComponent() {
  return content;
};