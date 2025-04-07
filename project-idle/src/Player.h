#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include "Generator.h"
#include "Observable.h"
#include "../views/Component.h"
#include "../views/Composite.h"
#include "../views/RectComponent.h"

namespace State {
  enum State {
    True,
    False,
    Locked
  };
}

class Player : public Observable {
private:

  using State::State::True;
  using State::State::False;
  using State::State::Locked;

  std::shared_ptr<Component> CreateGeneratorRow(BigInteger& generators_owned, Generator& generator,
                                                sf::Color text_color, int row, const sf::Vector2<float>& pos,
                                                sf::Color bg_color, char type);
  std::shared_ptr<Component> CreateMenuBar(std::vector<std::shared_ptr<Component>>& to_drow);
  std::shared_ptr<Component> CreateHeader();
  std::shared_ptr<Composite> CreateAtmosphereComponent();
  std::shared_ptr<Composite> CreateAtmosphereResourceComposite(std::string res);
  std::shared_ptr<Composite> CreateMilestone(std::string res, int number, std::shared_ptr<Component> anch);

  void AtmosphereReset(std::string res);

  std::shared_ptr<Composite> water_generators_component;
  std::shared_ptr<Composite> light_generators_component;
  std::shared_ptr<Composite> atmosphere_component;
  std::shared_ptr<Component> last_component = nullptr;

  bool atmosphere_unlocked = false;

  const BigInteger atmosphere_unlock_requirement = BigInteger("1.00e13");
  const std::vector<BigInteger> atmosphere_requirements = { BigInteger("1.00e13"), BigInteger("1.00e14"), BigInteger("1.00e16"), BigInteger("1.00e19") };


  BigInteger light_amount = 1;
  BigInteger water_amount = 1;
  std::vector<BigInteger> light_generators_owned = {0, 0, 0, 0};
  const std::vector<BigInteger> base_light_generators_owned = {0, 0, 0, 0};
  std::vector<BigInteger> water_generators_owned = {0, 0, 0, 0};
  const std::vector<BigInteger> base_water_generators_owned = {0, 0, 0, 0};
  std::vector<Generator> light_generators = {{1, 10, 1, 2}, {1'000, 100, 1, 2}, {100'000, 1000, 1, 2}, {10'000'000, 10000, 1, 2}}; // cost cost_multiplier prod_multiplier prod_multiplier_multiplier
  const std::vector<Generator> base_light_generators = {{1, 10, 1, 2}, {1'000, 100, 1, 2}, {100'000, 1000, 1, 2}, {10'000'000, 10000, 1, 2}};
  std::vector<Generator> water_generators = {{1, 10, 1, 2}, {1'000, 100, 1, 2}, {100'000, 1000, 1, 2}, {10'000'000, 10000, 1, 2}};
  const std::vector<Generator> base_water_generators = {{1, 10, 1, 2}, {1'000, 100, 1, 2}, {100'000, 1000, 1, 2}, {10'000'000, 10000, 1, 2}};
  std::vector<State::State> light_generators_autobuyers = {Locked, Locked, Locked, Locked};
  std::vector<State::State> water_generators_autobuyers = {Locked, Locked, Locked, Locked};
  int atmosphere_light_resets = 0;
  int atmosphere_water_resets = 0;
  std::vector<BigInteger> water_atmosphere_multipliers = {1, 1, 1, 1};

 public:

  void UIInit(std::vector<std::shared_ptr<Component>>& to_drow);
  void CalculateTick(int seconds);
  void TryToBuyGenerator(Generator& generator, BigInteger& currency, BigInteger& generators_owned);
  void LoadGame();
  void SaveGame();

};
