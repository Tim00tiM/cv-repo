#include "Fonts.h"

void Fonts::InitFonts() {
  Fonts::asimov.loadFromFile("./res/asimov.otf");
  Fonts::bold.loadFromFile("./res/bold.ttf");
}

void Fonts::Destruct() {
  delete instance;
  instance = nullptr;
}

Fonts& Fonts::Get() {
  if (nullptr == instance) instance = new Fonts;
  instance->InitFonts();
  return *instance;
}

Fonts* Fonts::instance = nullptr;
