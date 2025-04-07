#pragma once
#include <SFML/Graphics.hpp>
#include "RectComponent.h"
#include "ObserverTextComponent.h"
#include "Composite.h"


class ButtonComponent : public Component {
private:
  std::shared_ptr<Component> content;
  std::function<sf::Vector2f(const sf::Vector2f&)> CalcWidthHeight;

public:

  ButtonComponent(const sf::Vector2<float>& pos) : Component(pos) {
    content = std::make_shared<Composite>(pos);
  };
  ButtonComponent(const std::shared_ptr<Component>& anch) : Component(anch) {
    content = std::make_shared<Composite>(anch);
  };

  void SetCalcWidth(std::function<sf::Vector2f(const sf::Vector2f&)> new_calc_function);

  void Draw(sf::RenderWindow& render) override;

  std::shared_ptr<Component>& GetContentComponent();
};

