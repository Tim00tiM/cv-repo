#pragma once
#include "../src/Observer.h"
#include "TextComponent.h"


class ObserverTextComponent : public TextComponent {
protected:
  std::function<std::string()> CalcNewString;
public:
  ObserverTextComponent(const Color& text_color, int size, const Vector2<float>& pos, Font& font) :
          TextComponent("", text_color, size, pos, font) {
  }
  ObserverTextComponent(const Color& text_color, int size, const std::shared_ptr<Component>& anchor, Font& font) :
          TextComponent("", text_color, size, anchor, font) {
  }

  void SetCalcNewString(std::function<std::string()> new_CalcNewString);

  ObserverTextComponent() : TextComponent() {};

  void HandleEvent() override;
};
