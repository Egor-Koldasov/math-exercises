#pragma once

#include <algorithm>
#include <cmath>
#include <string>

#define EC_ESC "\033"
#define EC_ERASE_LINE EC_ESC "[2K"
#define EC_RESET_STYLE EC_ESC "[0m"
#define EC_GREEN_TEXT EC_ESC "[32m"
#define EC_CURSOR_HOME EC_ESC "[H"
#define EC_ERASE_SCREEN EC_ESC "[2J"
#define EC_CURSOR_DOWN EC_ESC "[1B"
#define EC_ENTER_ALT_SCREEN EC_ESC "[?1049h"
#define EC_LEAVE_ALT_SCREEN EC_ESC "[?1049l"
#define EC_HIDE_CURSOR EC_ESC "[?25l"
#define EC_SHOW_CURSOR EC_ESC "[?25h"
#define EC_HIDE_CURSOR EC_ESC "[?25l"
#define EC_RESET_TEXT_COLOR EC_ESC "[39m"
#define EC_RESET_BG_COLOR EC_ESC "[49m"

constexpr std::string ecRgbI(int r, int g, int b) {
  r = std::min(255, r);
  r = std::max(0, r);
  g = std::min(255, g);
  g = std::max(0, g);
  b = std::min(255, b);
  b = std::max(0, b);
  return "\033[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" +
         std::to_string(b) + "m";
}
constexpr std::string ecRgbD(double r, double g, double b) {
  return ecRgbI(static_cast<int>(round(r * 255)),
                static_cast<int>(round(g * 255)),
                static_cast<int>(round(b * 255)));
}

constexpr std::string ecBgRgbI(int r, int g, int b) {
  r = std::min(255, r);
  r = std::max(0, r);
  g = std::min(255, g);
  g = std::max(0, g);
  b = std::min(255, b);
  b = std::max(0, b);
  return "\033[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" +
         std::to_string(b) + "m";
}
constexpr std::string ecBgRgbD(double r, double g, double b) {
  return ecBgRgbI(static_cast<int>(round(r * 255)),
                  static_cast<int>(round(g * 255)),
                  static_cast<int>(round(b * 255)));
}
