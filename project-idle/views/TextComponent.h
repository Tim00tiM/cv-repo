#pragma once
#include "Component.h"
#include <SFML/Graphics.hpp>

using namespace sf;

class TextComponent : public Component {
protected:
  String string;

  Color text_color;
  int size;
  Text text;
  Font font_;

public:
  TextComponent() : Component(sf::Vector2f(0, 0)) {
    text = Text();
  };
  TextComponent(const std::shared_ptr<Component>& anch) : Component(anch->GetPos()) {
    anchor = anch;
    text = Text();
  };
  TextComponent(const String& string, const Color& text_color, int size, const Vector2<float>& pos, const Font& font) :
          Component(pos), string(string), text_color(text_color), size(size), font_(font) {
    text = Text();
    text.setFont(font_);
    text.setString(string);
    text.setCharacterSize(size);
    text.setFillColor(text_color);
    text.setPosition(pos);
  }

  TextComponent(const String& string, const Color& text_color, int size, const std::shared_ptr<Component>& anch, const Font& font) :
          TextComponent(string, text_color, size, anch->GetPos(), font) {
    anchor = anch;
    text.setPosition(pos);
  }

  void Draw(sf::RenderWindow &render) override;

  void SetString(const String &string);

  void SetTextColor(const Color &textColor);

  void SetSize(int size);

  void SetFont(const Font &font);

  Rect<float> GetGlobalBounds();
};
