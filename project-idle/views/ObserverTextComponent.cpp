#include "ObserverTextComponent.h"
#include <iostream>


void ObserverTextComponent::HandleEvent() {
  string = CalcNewString();
  text.setString(string);
}

void ObserverTextComponent::SetCalcNewString(std::function<std::string()> new_CalcNewString) {
  CalcNewString = new_CalcNewString;
}