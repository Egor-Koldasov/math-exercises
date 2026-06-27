#include "getRandomInt.h"
#include <random>

int getRandomInt(GetRandomIntOpts opts) {
  static std::random_device randomDevice;
  static std::mt19937 mtGen(randomDevice());

  std::uniform_int_distribution<int> uniformDist(opts.min, opts.max);
  auto number = uniformDist(mtGen);
  return number;
}
