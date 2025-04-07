#pragma once
#include "Observer.h"
#include <set>
#include <memory>

struct Observable {
  void AddObserver(std::shared_ptr<Observer> o) {
    observers.insert(o);
  };
  void RemoveObserver(std::shared_ptr<Observer> o) {
    observers.erase(o);
  };
  void NotifyObservers() {
    for (auto& i : observers) {
      i->HandleEvent();
    }
  };
  std::set<std::shared_ptr<Observer>> observers;
};