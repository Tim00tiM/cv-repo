#include <thread>
#include <chrono>
#include <SFML/Graphics.hpp>
#include "src/Player.h"
#include "res/Colors.cpp"
#include <memory>
#include <filesystem>
#include "res/Fonts.h"

using namespace sf;
using namespace Colors;

const int one_second = 1000;

Player globals;
Fonts* fonts = &Fonts::Get();

void GameLoop() {
  auto last = sf::Clock();
  auto delta = last.restart().asMilliseconds();
  auto delta_save = last.restart().asMilliseconds();
  auto time_add = last.restart().asMilliseconds();
  int seconds_passed;
  while (true) {
    time_add = last.restart().asMilliseconds();
    delta += time_add;
    delta_save += time_add;
    if (delta >= one_second) {
      seconds_passed = delta / one_second;
      globals.CalculateTick(seconds_passed);
      delta -= seconds_passed * one_second;
    }
    if (delta_save >= one_second * 10) {
      delta_save -= one_second * 10;
      globals.SaveGame();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

int main() {
  fonts->InitFonts();
  std::thread gameLoop(GameLoop);
  gameLoop.detach();

  auto current_path = std::filesystem::absolute(std::filesystem::current_path());
  current_path/="idle-save.txt";

  if (std::filesystem::exists(current_path)) {
    globals.LoadGame();
  }

  Event null_event;
  std::vector<std::shared_ptr<Component>> to_draw;
  globals.UIInit(to_draw);
  RenderWindow win(VideoMode(1100, 800),
                   "Tree Idle", Style::Default);
  win.setFramerateLimit(10);
  while (win.isOpen()) {
    Event ev;
    while (win.pollEvent(ev)) {
      if (ev.type == Event::Closed) {
        win.close();
      }
      for (auto& i : to_draw) {
        i->update(ev, win);
      }
    }

    if (Keyboard::isKeyPressed(Keyboard::Escape)) {
      win.close();
    }
    win.clear();
    for (auto& i : to_draw) {
      i->Draw(win);
      i->update(null_event, win);
    }
    globals.NotifyObservers();
    win.display();
  }
}