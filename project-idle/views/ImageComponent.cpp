#include "ImageComponent.h"

void ImageComponent::Draw(sf::RenderWindow &render) {
  auto new_pos = RerenderPos();
  sprite.setPosition(new_pos);
  if (isVisible) {
    render.draw(sprite);
  }
}

void ImageComponent::SetSize(float x, float y) {
  float old_x = sprite.getGlobalBounds().width;
  float old_y = sprite.getGlobalBounds().height;
  float x_scale = x / old_x;
  float y_scale = y / old_y;
  sprite.setScale(x_scale, y_scale);
}

void ImageComponent::SetImage(const std::string &path_to_image) {
  texture.loadFromFile(path_to_image);
  sprite.setTexture(texture);
}