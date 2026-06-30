#include "util/getRandomInt.h"
#include "util/joinStringVector.hpp"
#include <cmath>
#include <getopt.h>
#include <ios>
#include <iostream>
#include <limits>

constexpr auto max_stream_size = std::numeric_limits<std::streamsize>::max();

namespace CliAppOpts {

enum class GraphicsMode { FULL, TEXT };

std::unordered_map<std::string, GraphicsMode> argvToGraphicsModeMap{
    {"full", GraphicsMode::FULL}, {"text", GraphicsMode::TEXT}};
struct CliAppOpts {
  GraphicsMode graphicsMode;
};
static option optionConfList[] = {
    option{"graphics-mode", required_argument, nullptr, 'g'},
    option{nullptr, 0, nullptr, 0}};

struct InitCliAppRes {
  std::vector<std::string> errorMessages;
  CliAppOpts opts;
};

InitCliAppRes initCliApp(int argc, char *argv[]) {
  int opt;
  InitCliAppRes res{};
  while ((opt = getopt_long(argc, argv, "g:", optionConfList, nullptr)) != -1) {
    switch (opt) {
    case 'g': {
      auto matchI = argvToGraphicsModeMap.find(optarg);
      if (matchI == argvToGraphicsModeMap.end()) {
        std::string validValues = "";
        for (const auto &validValuePair : argvToGraphicsModeMap) {
          if (validValues != "") {
            validValues += ", ";
          }
          validValues += validValuePair.first;
        }
        res.errorMessages.emplace_back(
            "Invalid value for the option graphics-mode. Valid values are: " +
            validValues + ".");
      } else {
        res.opts.graphicsMode = matchI->second;
      }
      break;
    }

    case '?': {
      res.errorMessages.emplace_back("Ivalid option name.");
      break;
    }
    default: {
      res.errorMessages.emplace_back("Unexpected arguments.");
    }
    }
  }
  return res;
}
} // namespace CliAppOpts

void runTextMode() {
  while (true) {
    int randomNumber = getRandomInt({.min = 1, .max = 1000});
    std::cout << "Find the closes roots for the number: " << randomNumber
              << "\n";
    std::cin.ignore(max_stream_size, '\n');
    double root = std::sqrt(randomNumber);
    int intBefore = floor(root);
    int intAfter = ceil(root);
    std::cout << "The root: " << intBefore << "^2=" << intBefore * intBefore
              << " - " << intAfter << "^2=" << intAfter * intAfter << "   | "
              << root << "\n";
    std::cout << "\n\n";
  }
}

int main(int argc, char *argv[]) {
  auto cliAppOptsRes = CliAppOpts::initCliApp(argc, argv);
  if (cliAppOptsRes.errorMessages.size() > 0) {
    std::cerr << joinStringVector(cliAppOptsRes.errorMessages, "\n") << "\n";
    return 1;
  }
  if (cliAppOptsRes.opts.graphicsMode == CliAppOpts::GraphicsMode::TEXT) {
    runTextMode();
  }

  return 0;
}