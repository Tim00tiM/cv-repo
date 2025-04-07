#include "Composite.h"

void Composite::Add(const std::shared_ptr<Component>& add) {
  children.push_back(add);
}

std::shared_ptr<Component> Composite::GetChildren(int ind) {
  return children[ind];
};

void Composite::Draw(sf::RenderWindow& render){
  if (!isVisible) {
    return;
  }
  for (auto& i : children) {
    i->Draw(render);
  }
}

int Composite::GetSize() {
  return children.size();
}

void Composite::HandleEvent() {
  for (auto& i : children) {
    i->HandleEvent();
  }
}