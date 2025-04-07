#include "RectWithTextComponent.h"

#include "ButtonComponent.h"

void RectWithTextComponent::Draw(sf::RenderWindow& render) {

  auto new_pos = RerenderPos();

  text->SetPos(new_pos);
  rect->SetPos(new_pos);

  text->HandleEvent();
  sf::Vector2f text_size = {text->GetGlobalBounds().width, text->GetGlobalBounds().height};
  sf::Vector2f rect_size = CalcWidthHeight(text_size);
  rect->SetSize(rect_size.x, rect_size.y);

  if (isVisible) {
    rect->Draw(render);
    text->Draw(render);
  }
};

void RectWithTextComponent::SetCalcWidth(std::function<sf::Vector2f(const sf::Vector2f &)> new_calc_function) {
  CalcWidthHeight = new_calc_function;
}

std::shared_ptr<RectComponent> RectWithTextComponent::GetRectComponent() {
  return rect;
};
std::shared_ptr<ObserverTextComponent> RectWithTextComponent::GetTextComponent() {
  return text;
};