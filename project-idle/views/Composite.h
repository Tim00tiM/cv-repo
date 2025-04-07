#pragma once
#include "../src/Observer.h"
#include <SFML/Graphics.hpp>
#include "Component.h"
#include <set>
#include <memory>
#include <iostream>

struct Composite : public Component {
private:
  std::vector<std::shared_ptr<Component>> children;
public:

  void Add(const std::shared_ptr<Component>& add);
  void Draw(sf::RenderWindow& render) override;
  int GetSize();
  std::shared_ptr<Component> GetChildren(int ind);

  Composite() = delete;
  Composite(const sf::Vector2<float>& pos) : Component(pos) {
    update = [this](sf::Event& e, sf::RenderWindow& window){
      if (!isVisible) {
        return;
      }
      for (auto& i : children) {
        i->update(e, window);
    } };
  }
  Composite(const std::shared_ptr<Component>& anch) : Component(anch) {
    update = [this](sf::Event& e, sf::RenderWindow& window){
      if (!isVisible) {
        return;
      }
      for (auto& i : children) {
        i->update(e, window);
      } };
  }

  void HandleEvent() override;

};


