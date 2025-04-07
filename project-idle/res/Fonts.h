#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;
struct Fonts {
private:
  Fonts() = default; // no public constructor
  ~Fonts() = default; // no public destructor
  static Fonts* instance; // declaration class variable

public:
  static Fonts& Get();

  Fonts(const Fonts&) = delete; // rule of three
  Fonts& operator=(const Fonts&) = delete;

  static void Destruct();

  Font asimov;
  Font bold;
  void InitFonts();
};
