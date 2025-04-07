#include "Player.h"
#include "../views/ObserverTextComponent.h"
#include "../res/Colors.cpp"
#include "../res/Fonts.h"
#include "../views/ImageComponent.h"
#include "../views/ButtonComponent.h"
#include "../views/RectWithTextComponent.h"
#include "../res/Strings.cpp"
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace Colors;

void Player::UIInit(std::vector<std::shared_ptr<Component>>& to_drow) {
  std::shared_ptr<Component> menu = CreateMenuBar(to_drow); // create menu bar

  sf::Vector2f margin(30, 70);
  std::shared_ptr<RectComponent> menu_bar = std::dynamic_pointer_cast<RectComponent>(
          std::dynamic_pointer_cast<Composite>(menu)->GetChildren(0));
  margin.x += menu_bar->GetWidth();
  auto header = CreateHeader();
  to_drow.push_back(header);
  to_drow.push_back(menu);

  water_generators_component = std::make_shared<Composite>(margin);
  light_generators_component = std::make_shared<Composite>(margin);
  for (int i = 0; i < 4; ++i) {  //  create views for generators
    water_generators_component->Add(
            CreateGeneratorRow(water_generators_owned[i], water_generators[i], water_text_color, i, margin,
                               water_generator_bg, 'w'));
    light_generators_component->Add(
            CreateGeneratorRow(light_generators_owned[i], light_generators[i], light_text_color, i, margin,
                               light_generator_bg, 'l'));
  }

  atmosphere_component = CreateAtmosphereComponent();


}

std::shared_ptr<Component> Player::CreateGeneratorRow(BigInteger& generators_owned, Generator& generator,
                                                      Color text_color, int row, const sf::Vector2<float>& pos,
                                                      Color bg_color, char type){
  sf::Font asim = Fonts::Get().asimov;
  auto shared_ptr_gen_i = std::make_shared<Composite>(Vector2f({pos.x, static_cast<float>(pos.y + 50 + 50 * row)}));
  if (row == 0) { // Add tooltip
    auto tier = std::make_shared<TextComponent>("Tier", text_color, 25, shared_ptr_gen_i, asim);
    tier->SetMargin([tier] { return sf::Vector2f({10 - tier->GetGlobalBounds().width / 2, -40}); });
    shared_ptr_gen_i->Add(tier);

    auto amnt = std::make_shared<TextComponent>("Amount", text_color, 25, shared_ptr_gen_i, asim);
    amnt->SetMargin([amnt] { return sf::Vector2f({220 - amnt->GetGlobalBounds().width / 2, -40}); });
    shared_ptr_gen_i->Add(amnt);

    auto prod = std::make_shared<TextComponent>("Multiplier", text_color, 25, shared_ptr_gen_i, asim);
    prod->SetMargin([prod] { return sf::Vector2f({440 - prod->GetGlobalBounds().width / 2, -40}); });
    shared_ptr_gen_i->Add(prod);

    auto cst = std::make_shared<TextComponent>("Cost", text_color, 25, shared_ptr_gen_i, asim);
    cst->SetMargin([cst] { return sf::Vector2f({650 - cst->GetGlobalBounds().width / 2, -40}); });
    shared_ptr_gen_i->Add(cst);

    auto aut = std::make_shared<TextComponent>("Auto", text_color, 25, shared_ptr_gen_i, asim);
    aut->SetVisibility(false);
    aut->SetMargin([aut] { return sf::Vector2f({810 - aut->GetGlobalBounds().width / 2, -40}); });
    aut->update = [this, aut](sf::Event& e, sf::RenderWindow& window){
      if (atmosphere_unlocked) {
        aut->SetVisibility(true);
      } else {
        aut->SetVisibility(false);
    }};
    shared_ptr_gen_i->Add(aut);
  }

  if (row % 2 == 1) {
    auto bg = std::make_shared<RectComponent>(shared_ptr_gen_i);
    bg->SetSize(885, 50);
    bg->SetMargin([] { return sf::Vector2f(-20, 0); });
    bg->update = [this, bg](sf::Event& e, sf::RenderWindow& window){
      if (atmosphere_unlocked) {
        bg->SetSize(885, 50);
      } else {
        bg->SetSize(775, 50);
      }
    };
    bg->SetFillColor(bg_color);
    shared_ptr_gen_i->Add(bg);
  }

  shared_ptr_gen_i->Add(
          std::make_shared<TextComponent>(std::to_string(row + 1), text_color, 35, shared_ptr_gen_i, asim)); // tier of generator

  auto amount = std::make_shared<ObserverTextComponent>(text_color, 35, shared_ptr_gen_i, asim); // amount of generator
  amount->SetCalcNewString([&generators_owned] { return generators_owned.toString(); });
  amount->SetMargin([amount] { return sf::Vector2f({220 - amount->GetGlobalBounds().width / 2, 0}); });
  shared_ptr_gen_i->Add(amount);
  AddObserver(amount);

  auto production = std::make_shared<ObserverTextComponent>(text_color, 35, shared_ptr_gen_i, asim); // production of generator
  production->SetCalcNewString([this, &generator, row] { return (generator.production * this->water_atmosphere_multipliers[row]).toString(); });
  production->SetMargin([production] { return sf::Vector2f({440 - production->GetGlobalBounds().width / 2, 0}); });
  shared_ptr_gen_i->Add(production);
  AddObserver(production);

  auto cost_button = std::make_shared<ButtonComponent>(shared_ptr_gen_i->GetPos() + sf::Vector2f(650, 2));
  cost_button->update = [cost_button](sf::Event& e, sf::RenderWindow& window){
    cost_button->GetContentComponent()->update(e, window);
  };
  std::shared_ptr<RectWithTextComponent> cost = std::make_shared<RectWithTextComponent>(cost_button);
  cost_button->GetContentComponent() = cost;

  auto cost_text = cost->GetTextComponent();
  cost_text->SetCalcNewString([&generator] { return generator.cost.toString(); });
  cost_text->SetSize(35);
  cost_text->SetPos(cost->GetPos());
  cost_text->SetFont(asim);
  cost_text->HandleEvent();
  cost_text->SetMargin([cost_text] { return sf::Vector2f(-cost_text->GetGlobalBounds().width / 2, 0); });
  cost_text->SetTextColor(text_color);
  AddObserver(cost_text);

  auto cost_rect = cost->GetRectComponent();
  cost_rect->SetFillColor(bg_color);
  cost_rect->SetOutlineWidth(3);
  cost_rect->SetOutlineColor(grey);
  cost_rect->SetPos(cost->GetPos());
  cost->SetCalcWidth([](const sf::Vector2f &text_size) {
    float x = std::max(200.f, text_size.x + 20);
    float y = text_size.y + 10;
    return sf::Vector2f(x, y);
  });
  cost_rect->SetMargin([cost_text, cost_rect] {
    return sf::Vector2f(-cost_rect->GetWidth() / 2, 5);
  });

  cost->update = [this, &generator, cost_rect, type, &generators_owned](sf::Event& e, sf::RenderWindow& window) {
    if (!cost_rect->GetVisibility()) {
      return;
    }
    if (e.type == sf::Event::MouseButtonReleased) {
      auto i_pos = sf::Mouse::getPosition(window);
      float x = i_pos.x;
      float y = i_pos.y;
      sf::Vector2f pos = {x, y};
      if (cost_rect->Contains(pos)) {
        if (type == 'l') {
          TryToBuyGenerator(generator, light_amount, generators_owned);
        };
        if (type == 'w') {
          TryToBuyGenerator(generator, water_amount, generators_owned);
        };
      };
    }
  };
  

  shared_ptr_gen_i->Add(cost_button);

  auto buyer_button = std::make_shared<ButtonComponent>(shared_ptr_gen_i->GetPos() + sf::Vector2f(810, 2));
  buyer_button->SetVisibility(false);
  buyer_button->update = [this, buyer_button](sf::Event& e, sf::RenderWindow& window){
    if (atmosphere_unlocked) {
      buyer_button->SetVisibility(true);
    } else {
      buyer_button->SetVisibility(false);
      return;
    }
    buyer_button->GetContentComponent()->update(e, window);
  };
  std::shared_ptr<RectWithTextComponent> buyer = std::make_shared<RectWithTextComponent>(buyer_button);
  buyer_button->GetContentComponent() = buyer;

  auto buyer_text = buyer->GetTextComponent();
  buyer_text->SetCalcNewString([this, type, row, buyer_text] {
    State::State state = Locked;
    if (type == 'l') {
      state = this->light_generators_autobuyers[row];
    } else {
      state = this->water_generators_autobuyers[row];
    }
    switch(state) {
      case Locked:
        buyer_text->SetTextColor(grey);
        return "LOCK";
      case True:
        buyer_text->SetTextColor(green);
        return "ON";
      case False:
        buyer_text->SetTextColor(red);
        return "OFF";
    }});
  buyer_text->SetSize(35);
  buyer_text->SetPos(cost->GetPos());
  buyer_text->SetFont(asim);
  buyer_text->HandleEvent();
  buyer_text->SetMargin([buyer_text] { return sf::Vector2f(-buyer_text->GetGlobalBounds().width / 2, 0); });
  buyer_text->SetTextColor(text_color);
  AddObserver(buyer_text);

  auto buyer_rect = buyer->GetRectComponent();
  buyer_rect->SetFillColor(black);
  buyer_rect->SetOutlineWidth(3);
  buyer_rect->SetOutlineColor(grey);
  buyer_rect->SetPos(buyer->GetPos());
  buyer->SetCalcWidth([](const sf::Vector2f &text_size) {
    float x = 100;
    float y = text_size.y + 10;
    return sf::Vector2f(x, y);
  });
  buyer_rect->SetMargin([buyer_text, buyer_rect] {
    return sf::Vector2f(-buyer_rect->GetWidth() / 2, 5);
  });

  buyer->update = [this, buyer_rect, type, row](sf::Event& e, sf::RenderWindow& window) {
    if (!atmosphere_unlocked) {
      return;
    }
    if (e.type == sf::Event::MouseButtonReleased) {
      auto i_pos = sf::Mouse::getPosition(window);
      float x = i_pos.x;
      float y = i_pos.y;
      sf::Vector2f pos = {x, y};
      if (buyer_rect->Contains(pos)) {
        State::State* state;
        if (type == 'l') {
          state = &this->light_generators_autobuyers[row];
        } else {
          state = &this->water_generators_autobuyers[row];
        }
        if (*state == Locked) {
          return;
        }
        if (*state == True) {
          *state = False;
        } else {
          *state = True;
        }
      };
    }
  };

  shared_ptr_gen_i->Add(buyer_button);

  return shared_ptr_gen_i;
}

std::shared_ptr<Component> Player::CreateMenuBar(std::vector<std::shared_ptr<Component>>& to_drow) {
  sf::Font asim = Fonts::Get().asimov;

  std::shared_ptr<Composite> menu = std::make_shared<Composite>(sf::Vector2f(0, 0));
  std::shared_ptr<RectComponent> menu_bar = std::make_shared<RectComponent>(sf::Vector2<float>(-50, -50));
  menu_bar->SetSize(135, 900);
  menu_bar->SetFillColor(Color(110, 110, 110));
  menu_bar->SetOutlineColor(Color(61, 0, 0));
  menu_bar->SetOutlineWidth(5);
  menu->Add(menu_bar);

  auto tree_image = std::make_shared<ImageComponent>(sf::Vector2f(10, 10));
  tree_image->SetImage("./res/tree.png");
  tree_image->SetSize(65, 70);
  menu->Add(tree_image);

  std::shared_ptr<ButtonComponent> water_button = std::make_shared<ButtonComponent>(sf::Vector2f(20, 100));
  water_button->update = [water_button](sf::Event& e, sf::RenderWindow& window){
    water_button->GetContentComponent()->update(e, window);
  };
  std::shared_ptr<Composite> water_tab = std::make_shared<Composite>(water_button);
  water_button->GetContentComponent() = water_tab;
  std::shared_ptr<RectComponent> water_tab_rect = std::make_shared<RectComponent>(water_tab);
  water_tab->Add(water_tab_rect);
  water_tab_rect->SetOutlineColor(grey);
  water_tab_rect->SetOutlineWidth(3);
  water_tab_rect->SetSize(50, 50);
  water_tab_rect->SetFillColor(water_button_color);
  water_tab->update = [this, &to_drow, water_tab, water_tab_rect](sf::Event& e, sf::RenderWindow& window) {
    if (!water_tab->GetVisibility()) {
      return;
    }
    if (e.type == sf::Event::MouseButtonReleased) {
      auto i_pos = sf::Mouse::getPosition(window);
      float x = i_pos.x;
      float y = i_pos.y;
      sf::Vector2f pos = {x, y};
      if (water_tab_rect->Contains(pos)) {
        if (last_component != nullptr) {
          to_drow.erase(std::find(to_drow.begin(), to_drow.end(), last_component));
          last_component->SetVisibility(false);
        }
        to_drow.push_back(water_generators_component);
        last_component = water_generators_component;
        last_component->SetVisibility(true);
      }
    };
  };

  std::shared_ptr<ImageComponent> water_tab_image = std::make_shared<ImageComponent>(water_tab);
  water_tab->Add(water_tab_image);

  water_tab_image->SetMargin([]{return sf::Vector2f(15, 5); });
  water_tab_image->SetImage("./res/water.png");
  water_tab_image->SetSize(20, 40);

  menu->Add(water_button);

  std::shared_ptr<ButtonComponent> light_button = std::make_shared<ButtonComponent>(sf::Vector2f(20, 170));
  light_button->update = [light_button](sf::Event& e, sf::RenderWindow& window){
    light_button->GetContentComponent()->update(e, window);
  };
  std::shared_ptr<Composite> light_tab = std::make_shared<Composite>(light_button);
  light_button->GetContentComponent() = light_tab;
  std::shared_ptr<RectComponent> light_tab_rect = std::make_shared<RectComponent>(light_tab);
  light_tab->Add(light_tab_rect);
  light_tab_rect->SetOutlineColor(grey);
  light_tab_rect->SetOutlineWidth(3);
  light_tab_rect->SetSize(50, 50);
  light_tab_rect->SetFillColor(light_button_color);
  light_tab->update = [this, &to_drow, light_tab, light_tab_rect](sf::Event& e, sf::RenderWindow& window) {
    if (!light_tab->GetVisibility()) {
      return;
    }
    if (e.type == sf::Event::MouseButtonReleased) {
      auto i_pos = sf::Mouse::getPosition(window);
      float x = i_pos.x;
      float y = i_pos.y;
      sf::Vector2f pos = {x, y};
      if (light_tab_rect->Contains(pos)) {
        if (last_component != nullptr) {
          to_drow.erase(std::find(to_drow.begin(), to_drow.end(), last_component));
          last_component->SetVisibility(false);
        }
        to_drow.push_back(light_generators_component);
        last_component = light_generators_component;
        last_component->SetVisibility(true);
      }
    };
  };

  std::shared_ptr<ImageComponent> light_tab_image = std::make_shared<ImageComponent>(light_tab);
  light_tab->Add(light_tab_image);

  light_tab_image->SetImage("./res/light.png");
  light_tab_image->SetMargin([]{return sf::Vector2f(5, 5); });
  light_tab_image->SetSize(40, 40);

  menu->Add(light_button);

  std::shared_ptr<ButtonComponent> atmosphere_button = std::make_shared<ButtonComponent>(sf::Vector2f(20, 240));

  atmosphere_button->update = [atmosphere_button](sf::Event& e, sf::RenderWindow& window){
    atmosphere_button->GetContentComponent()->update(e, window);
  };
  std::shared_ptr<Composite> atmosphere_tab = std::make_shared<Composite>(atmosphere_button);
  atmosphere_button->GetContentComponent() = atmosphere_tab;
  std::shared_ptr<RectComponent> atmosphere_tab_rect = std::make_shared<RectComponent>(atmosphere_tab);
  atmosphere_tab->Add(atmosphere_tab_rect);
  atmosphere_tab_rect->SetOutlineColor(grey);
  atmosphere_tab_rect->SetOutlineWidth(3);
  atmosphere_tab_rect->SetSize(50, 50);
  atmosphere_tab_rect->SetFillColor(atmosphere_button_color);
  atmosphere_tab->update = [this, &to_drow, atmosphere_tab_rect, atmosphere_tab](sf::Event& e, sf::RenderWindow& window) {
    if (atmosphere_unlocked) {
      atmosphere_tab->SetVisibility(true);
    } else {
      atmosphere_tab->SetVisibility(false);
      return;
    }
    if (!atmosphere_tab_rect->GetVisibility()) {
      return;
    }
    if (e.type == sf::Event::MouseButtonReleased) {
      auto i_pos = sf::Mouse::getPosition(window);
      float x = i_pos.x;
      float y = i_pos.y;
      sf::Vector2f pos = {x, y};
      if (atmosphere_tab_rect->Contains(pos)) {
        if (last_component != nullptr) {
          to_drow.erase(std::find(to_drow.begin(), to_drow.end(), last_component));
          last_component->SetVisibility(false);
        }
        to_drow.push_back(atmosphere_component);
        last_component = atmosphere_component;
        last_component->SetVisibility(true);
      }
    };
  };

  std::shared_ptr<ImageComponent> atmosphere_tab_image = std::make_shared<ImageComponent>(atmosphere_tab);
  atmosphere_tab->Add(atmosphere_tab_image);

  atmosphere_tab_image->SetImage("./res/atmosphere.png");
  atmosphere_tab_image->SetMargin([]{return sf::Vector2f(5, 10); });
  atmosphere_tab_image->SetSize(40, 30);

  menu->Add(atmosphere_button);


  return menu;
}

std::shared_ptr<Component> Player::CreateHeader() {
  sf::Font asim = Fonts::Get().asimov;
  auto header = std::make_shared<Composite>(sf::Vector2f({-10, -10}));
  auto background = std::make_shared<RectComponent>(header);
  background->SetSize(1200, 70);
  background->SetFillColor(Color(155, 155, 155));
  background->SetOutlineColor(Color(61, 0, 0));
  background->SetOutlineWidth(5);
  header->Add(background);

  auto water_amount_text = std::make_shared<ObserverTextComponent>(water_text_color, 25, header, asim);
  water_amount_text->SetCalcNewString([this] { return water_amount.toString(); });
  water_amount_text->SetMargin([] { return Vector2f({200, 20}); });
  header->Add(water_amount_text);

  auto water_image = std::make_shared<ImageComponent>(water_amount_text);
  water_image->SetMargin([] { return sf::Vector2f(-20, -3); });
  water_image->SetImage("./res/water.png");
  water_image->SetSize(15, 30);
  header->Add(water_image);

  auto light_amount_text = std::make_shared<ObserverTextComponent>(light_text_color, 25, header, asim);
  light_amount_text->SetCalcNewString([this] { return light_amount.toString(); });
  light_amount_text->SetMargin([] { return Vector2f({450, 20}); });
  header->Add(light_amount_text);

  auto light_image = std::make_shared<ImageComponent>(light_amount_text);
  light_image->SetMargin([] { return sf::Vector2f(-35, 0); });
  light_image->SetImage("./res/light.png");
  light_image->SetSize(30, 30);
  header->Add(light_image);

  AddObserver(water_amount_text);
  AddObserver(light_amount_text);

  return header;
}

std::shared_ptr<Composite> Player::CreateAtmosphereComponent() {
  sf::Font asim = Fonts::Get().asimov;
  auto atmosphere_composite = std::make_shared<Composite>(sf::Vector2f(50, 50));

  auto water_composite = CreateAtmosphereResourceComposite("water");
  water_composite->SetPos(150, 120);
  atmosphere_composite->Add(water_composite);

  auto light_composite = CreateAtmosphereResourceComposite("light");
  light_composite->SetPos(620, 120);
  atmosphere_composite->Add(light_composite);

  return atmosphere_composite;
}

void Player::CalculateTick(int seconds) {
  for (int i = 3; i > 0; --i) {
    light_generators_owned[i - 1] += light_generators[i].production * light_generators_owned[i] * seconds * water_atmosphere_multipliers[i];
    water_generators_owned[i - 1] += water_generators[i].production * water_generators_owned[i] * seconds * water_atmosphere_multipliers[i];
  }
  light_amount += light_generators_owned[0] * light_generators[0].production * seconds * water_atmosphere_multipliers[0];
  water_amount += water_generators_owned[0] * water_generators[0].production * seconds * water_atmosphere_multipliers[0];
  for (int i = 3; i >= 0; --i) {
    if (light_generators_autobuyers[i] == True) {
      TryToBuyGenerator(light_generators[i], light_amount, light_generators_owned[i]);
    }
    if (water_generators_autobuyers[i] == True) {
      TryToBuyGenerator(water_generators[i], water_amount, water_generators_owned[i]);
    }
  }
  if (water_amount > atmosphere_unlock_requirement || light_amount > atmosphere_unlock_requirement) {
    atmosphere_unlocked = true;
  }
}

std::shared_ptr<Composite> Player::CreateAtmosphereResourceComposite(std::string res) {
  sf::Font asim = Fonts::Get().asimov;
  auto composite = std::make_shared<Composite>(sf::Vector2f(0, 0));

  auto rect = std::make_shared<RectComponent>(composite);
  composite->Add(rect);

  sf::Color text_color;
  sf::Color button_color;
  sf::Color bg_color;

  if (res == "water") {
    text_color = water_text_color;
    button_color = water_button_color;
    bg_color = water_generator_bg;
  }
  if (res == "light") {
    text_color = light_text_color;
    button_color = light_button_color;
    bg_color = light_generator_bg;
  }

  rect->SetSize(400, 565);
  rect->SetOutlineColor(text_color);
  rect->SetFillColor(button_color);
  rect->SetOutlineWidth(3);

  auto atmosphere_button = std::make_shared<ButtonComponent>(composite);
  atmosphere_button->update = [atmosphere_button](sf::Event& e, sf::RenderWindow& window){
    atmosphere_button->GetContentComponent()->update(e, window);
  };
  composite->Add(atmosphere_button);

  auto atmosphere_button_component = std::make_shared<Composite>(atmosphere_button);
  atmosphere_button->GetContentComponent() = atmosphere_button_component;

  auto atmosphere_button_component_rect = std::make_shared<RectComponent>(atmosphere_button_component);
  atmosphere_button_component_rect->SetSize(130, 50);
  atmosphere_button_component_rect->SetOutlineColor(text_color);
  atmosphere_button_component_rect->SetFillColor(bg_color);
  atmosphere_button_component_rect->SetMargin([]{return sf::Vector2f(130, 30); });
  atmosphere_button_component_rect->SetOutlineWidth(3);
  atmosphere_button_component_rect->update = [this, atmosphere_button_component_rect, res](sf::Event& e, sf::RenderWindow& window){
    if (!atmosphere_component->GetVisibility()) {
      return;
    }
    BigInteger res_amount;
    int resets_done;
    if (res == "water") {
      res_amount = water_amount;
      resets_done = atmosphere_water_resets;
    }
    if (res == "light") {
      res_amount = light_amount;
      resets_done = atmosphere_light_resets;
    }
    if (resets_done == 4) {
      atmosphere_button_component_rect->SetOutlineColor(grey);
      return;
    }
    if (res_amount >= atmosphere_requirements[resets_done]) {
      atmosphere_button_component_rect->SetOutlineColor(green);
    } else {
      atmosphere_button_component_rect->SetOutlineColor(red);
      return;
    }
    if (e.type == sf::Event::MouseButtonReleased) {
      auto i_pos = sf::Mouse::getPosition(window);
      float x = i_pos.x;
      float y = i_pos.y;
      sf::Vector2f pos = {x, y};
      if (atmosphere_button_component_rect->Contains(pos)) {
        AtmosphereReset(res);
      }
    };
  };
  atmosphere_button_component->Add(atmosphere_button_component_rect);


  auto atmosphere_button_component_text = std::make_shared<TextComponent>("Reset", text_color, 40, atmosphere_button_component, asim);
  atmosphere_button_component_text->SetMargin([]{return sf::Vector2f(140, 35); });
  atmosphere_button_component->Add(atmosphere_button_component_text);

  auto milestone_composite = std::make_shared<Composite>(composite);

  milestone_composite->SetMargin([]{ return sf::Vector2f(0, 100); });

  for (int i = 1; i <= 4; ++i) {
    auto milestone = CreateMilestone(res, i, milestone_composite);
    std::shared_ptr<TextComponent> milestone_tooltip = std::static_pointer_cast<TextComponent>(milestone->GetChildren(2));
    milestone_tooltip->SetString(Strings::milestone_tooltips[res][i - 1]);
    composite->Add(milestone);
  }

  return composite;
}

std::shared_ptr<Composite> Player::CreateMilestone(std::string res, int number, std::shared_ptr<Component> anch) {
  sf::Font asim = Fonts::Get().asimov;
  sf::Font bold = Fonts::Get().bold;
  BigInteger res_amount;
  int* resets_done;
  if (res == "water") {
    res_amount = water_amount;
    resets_done = &atmosphere_water_resets;
  }
  if (res == "light") {
    res_amount = light_amount;
    resets_done = &atmosphere_light_resets;
  }
  auto milestone = std::make_shared<Composite>(anch);
  milestone->SetMargin([number]{return sf::Vector2f(5, number * 120 - 120);});

  auto milestone_rect = std::make_shared<RectComponent>(milestone);
  milestone_rect->SetOutlineColor(grey);
  milestone_rect->SetOutlineWidth(3);
  milestone_rect->SetFillColor(light_grey);
  milestone_rect->SetSize(390, 100);
  milestone_rect->update = [resets_done, number, milestone_rect](sf::Event& e, sf::RenderWindow& window){
    if (*resets_done >= number) {
      milestone_rect->SetFillColor(light_green_milestone);
      milestone_rect->SetOutlineColor(green_milestone);
    } else {
      milestone_rect->SetOutlineColor(grey);
      milestone_rect->SetFillColor(light_grey);
    }
  };

  milestone->Add(milestone_rect);

  auto milestone_reset_text = std::make_shared<TextComponent>(milestone);
  milestone_reset_text->SetFont(bold);
  milestone_reset_text->SetTextColor(grey);
  milestone_reset_text->SetString(std::to_string(number) + " RESET");
  milestone_reset_text->SetSize(40);
  milestone_reset_text->SetMargin([]{ return sf::Vector2f(110, -5); });

  milestone->Add(milestone_reset_text);

  auto milestone_tooltip_text = std::make_shared<TextComponent>(milestone);
  milestone_tooltip_text->SetFont(asim);
  milestone_tooltip_text->SetSize(20);
  milestone_tooltip_text->SetTextColor(grey);
  milestone_tooltip_text->SetMargin([milestone_tooltip_text]{ return sf::Vector2f(195 - milestone_tooltip_text->GetGlobalBounds().width / 2, 40); });

  milestone->Add(milestone_tooltip_text);

  return milestone;
}

void Player::TryToBuyGenerator(Generator& generator, BigInteger& currency, BigInteger& generators_owned) {
  if (currency >= generator.cost) {
    currency -= generator.cost;
    generator.cost *= generator.cost_muliplier;
    if (generators_owned != 0) {
      generator.production *= generator.production_multiplier;
    }
    generators_owned += 1;
  };
}

void Player::AtmosphereReset(std::string res) {
  if (res == "water") {
    atmosphere_water_resets += 1;
    for (int i = 0; i < atmosphere_water_resets; ++i) {
      water_atmosphere_multipliers[i] *= 10;
    }
  }
  if (res == "light") {
    light_generators_autobuyers[atmosphere_light_resets] = False;
    water_generators_autobuyers[atmosphere_light_resets] = False;
    atmosphere_light_resets += 1;
  }
  water_amount = 1;
  light_amount = 1;
  light_generators = base_light_generators;
  water_generators = base_water_generators;
  light_generators_owned = base_light_generators_owned;
  water_generators_owned = base_water_generators_owned;
}


void Player::SaveGame() {
  auto current_path = std::filesystem::absolute(std::filesystem::current_path());
  current_path/="idle-save.txt";
  std::ofstream save(current_path);
  save << light_amount << '\n';
  save << water_amount << '\n';
  for (int i = 0; i < 4; ++i) {
    save << light_generators_owned[i] << ' ';
  }
  save << '\n';
  for (int i = 0; i < 4; ++i) {
    save << water_generators_owned[i] << ' ';
  }
  save << '\n';
  for (int i = 0; i < 4; ++i) {
    save << light_generators[i].cost << " "
         << light_generators[i].cost_muliplier << " "
         << light_generators[i].production << " "
         << light_generators[i].production_multiplier << "\n";
  }
  for (int i = 0; i < 4; ++i) {
    save << water_generators[i].cost << " "
         << water_generators[i].cost_muliplier << " "
         << water_generators[i].production << " "
         << water_generators[i].production_multiplier << "\n";
  }
  for (int i = 0; i < 4; ++i) {
    save << light_generators_autobuyers[i] << " ";
  }
  save << "\n";
  for (int i = 0; i < 4; ++i) {
    save << water_generators_autobuyers[i] << " ";
  }
  save << "\n";
  save << atmosphere_unlocked << "\n";
  save << atmosphere_light_resets << "\n";
  save << atmosphere_water_resets << "\n";
  for (int i = 0; i < 4; ++i) {
    save << water_atmosphere_multipliers[i] << " ";
  }
  save << "\n";
  save.close();
}

void Player::LoadGame() {
  auto current_path = std::filesystem::absolute(std::filesystem::current_path());
  current_path/="idle-save.txt";
  std::ifstream save(current_path);
  save >> light_amount;
  save >> water_amount;
  for (int i = 0; i < 4; ++i) {
    save >> light_generators_owned[i];
  }
  for (int i = 0; i < 4; ++i) {
    save >> water_generators_owned[i];
  }
  for (int i = 0; i < 4; ++i) {
    save >> light_generators[i].cost
         >> light_generators[i].cost_muliplier
         >> light_generators[i].production
         >> light_generators[i].production_multiplier;
  }
  for (int i = 0; i < 4; ++i) {
    save >> water_generators[i].cost
         >> water_generators[i].cost_muliplier
         >> water_generators[i].production
         >> water_generators[i].production_multiplier;
  }
  int state;
  for (int i = 0; i < 4; ++i) {
    save >> state;
    switch (state) {
      case 0:
        light_generators_autobuyers[i] = True;
        break;
      case 1:
        light_generators_autobuyers[i] = False;
        break;
      case 2:
        light_generators_autobuyers[i] = Locked;
        break;
    }
  }
  for (int i = 0; i < 4; ++i) {
    save >> state;
    switch (state) {
      case 0:
        water_generators_autobuyers[i] = True;
        break;
      case 1:
        water_generators_autobuyers[i] = False;
        break;
      case 2:
        water_generators_autobuyers[i] = Locked;
        break;
    }
  }
  save >> atmosphere_unlocked;
  save >> atmosphere_light_resets;
  save >> atmosphere_water_resets;
  for (int i = 0; i < 4; ++i) {
    save >> water_atmosphere_multipliers[i];
  }
  save.close();
}