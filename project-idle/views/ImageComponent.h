#pragma once
#include "Component.h"

class ImageComponent : public Component {
private:
  sf::Sprite sprite;
  sf::Texture texture;

public:
  ImageComponent(const sf::Vector2f& coords) : Component(coords) {};
  ImageComponent(const std::shared_ptr<Component>& anch) : Component(anch) {};

  void SetImage(const std::string& path_to_image);
  void Draw(sf::RenderWindow &render) override;
  void SetSize(float x, float y);
};

