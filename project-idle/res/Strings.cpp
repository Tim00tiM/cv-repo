#include <unordered_map>
#include <string>
#include <vector>

namespace Strings {
  static std::unordered_map<std::string, std::vector<std::string>> milestone_tooltips = {
          {"water", {"Multiple 1st generator by 10", "Multiple 1-2 generators by 10", "Multiple 1-3 generators by 10", "Multiple 1-4 generators by 10"}},
          {"light", {"Unlock 1st generator autobuyer", "Unlock 2nd generator autobuyer", "Unlock 3rd generator autobuyer", "Unlock 4th generator autobuyer"}}
  };
}