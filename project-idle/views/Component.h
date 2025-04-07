#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <functional>
#include "../src/Observer.h"

struct Component : public Observer{
protected:
  sf::Vector2<float> pos;
  std::shared_ptr<Component> anchor = nullptr;
  sf::Vector2f RerenderPos();
  bool isVisible = true;


public:

  std::function<sf::Vector2f()> GetMargin = []{ return sf::Vector2f({0, 0}); };
  virtual void Draw(sf::RenderWindow& render) = 0;
  std::function<void(sf::Event& e, sf::RenderWindow& window)> update = [](sf::Event& e, sf::RenderWindow& window){ return; };
  sf::Vector2<float> GetPos();
  virtual void SetPos(const sf::Vector2<float>& pos);
  virtual void SetPos(float x, float y);
  Component(const sf::Vector2<float>& pos) : pos(pos) {};
  Component(const std::shared_ptr<Component>& anch) : pos(anch->GetPos()), anchor(anch) {};
  void SetMargin(std::function<sf::Vector2f()>);
  bool GetVisibility();
  void SetVisibility(bool);
  void HandleEvent() override { };
};