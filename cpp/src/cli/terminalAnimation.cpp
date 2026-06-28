
#include "util/termEscape.hpp"
#include <chrono>
#include <cmath>
#include <iostream>

int main() {
  using namespace std::chrono_literals;
  auto startTime = std::chrono::steady_clock::now();
  auto lastAnimationTime = startTime;
  int x = 0;
  double xFloat = 0;
  int width = 10.0;
  while (true) {
    auto timeNow = std::chrono::steady_clock::now();
    auto msPassed = (timeNow - lastAnimationTime) / 1ms;
    xFloat += static_cast<double>(msPassed) * 0.01;
    xFloat = fmod(xFloat, width);
    lastAnimationTime = timeNow;
    // std::cout << xFloat << "\n";
    std::cout << EC_ERASE_LINE << "\r";
    for (auto i = 0; i < width; i++) {
      std::cout << (i == floor(xFloat) ? "x" : " ");
    }
    std::cout.flush();
  }
  return 0;
}