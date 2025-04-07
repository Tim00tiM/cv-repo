#pragma once
#include <SFML/Graphics.hpp>
#include "RectComponent.h"
#include "ObserverTextComponent.h"


class RectWithTextComponent : public Component {
private:
  std::shared_ptr<RectComponent> rect;
  std::shared_ptr<ObserverTextComponent> text;
  std::function<sf::Vector2f(const sf::Vector2f&)> CalcWidthHeight;

public:

  RectWithTextComponent(const sf::Vector2<float>& pos) : Component(pos) {
    rect = std::make_shared<RectComponent>();
    text = std::make_shared<ObserverTextComponent>();
  };
  RectWithTextComponent(const std::shared_ptr<Component>& anch) : Component(anch) {
    rect = std::make_shared<RectComponent>();
    text = std::make_shared<ObserverTextComponent>();
  };

  void SetCalcWidth(std::function<sf::Vector2f(const sf::Vector2f&)> new_calc_function);

  void Draw(sf::RenderWindow& render) override;

  std::shared_ptr<RectComponent> GetRectComponent();
  std::shared_ptr<ObserverTextComponent> GetTextComponent();
};

