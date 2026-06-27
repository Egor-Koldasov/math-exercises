#include "util/getRandomInt.h"
#include <cmath>
#include <ios>
#include <iostream>
#include <limits>

constexpr auto max_stream_size = std::numeric_limits<std::streamsize>::max();

int main() {
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

  return 0;
}