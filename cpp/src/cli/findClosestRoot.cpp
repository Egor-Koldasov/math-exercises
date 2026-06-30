#include "util/draw.hpp"
#include "util/getRandomInt.h"
#include "util/joinStringVector.hpp"
#include "util/termEscape.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <getopt.h>
#include <ios>
#include <iostream>
#include <limits>
#include <numbers>
#include <string>
#include <vector>

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

std::string drawBackground(IVec2 pos, DrawCharState state) {
  DVec2 uv{static_cast<double>(pos.x) / state.size.x,
           static_cast<double>(pos.y) / state.size.y};
  uv.x = uv.x * 2 - 1;
  uv.y = uv.y * 2 - 1;
  uv.x *= static_cast<double>(state.size.x) / state.size.y;
  DVec2 cellSize = {static_cast<double>(state.pixelSize.x) / state.size.x,
                    static_cast<double>(state.pixelSize.y) / state.size.y};
  uv.x *= cellSize.x / cellSize.y;

  double distSqr = uv.x * uv.x + uv.y * uv.y;

  double animNorm =
      -sin(2 * std::numbers::pi * (state.timeMs / 12000)) * 0.5 + 0.5;

  double mask = std::ranges::max(0., exp(-distSqr) * 0.3) * animNorm;

  return ecBgRgbD(mask, mask, mask) + " " + EC_RESET_BG_COLOR;
}

struct UiBoxRender {
  std::vector<std::string> rows;
  size_t width;
};

std::vector<std::string> uiTextRow = {"H", "e", "l", "l", "o"};

constexpr int uiTextRowLimit = 6;
constexpr IVec2 gap = {4, 2};

UiBoxRender makeUiBox(const std::string &text) {
  UiBoxRender uiBox;
  std::vector<std::string> rows = {""};
  std::vector<std::string> renderString;
  size_t maxWidth = 0;
  std::string color = ecBgRgbI(33, 33, 33);

  for (auto i = 0; i < text.size(); i++) {
    auto &charItem = text[i];
    if (charItem == '\n' || i == text.size() - 1) {
      size_t currentWidth = rows.back().size();
      if (charItem != '\n') {
        currentWidth += 1;
      }
      if (currentWidth > maxWidth) {
        maxWidth = currentWidth;
      }
    }

    if (charItem == '\n' && i < text.size() - 1) {
      rows.emplace_back("");
    }

    if (charItem != '\n') {
      if (rows.back() == "") {
        rows.back() = charItem;
      } else {
        rows.back() += charItem;
      }
    }
  }

  for (auto gapI = 0; gapI < gap.y; gapI++) {
    for (auto colI = 0; colI < maxWidth + static_cast<size_t>(gap.x) * 2;
         colI++) {
      if (colI == 0) {
        renderString.emplace_back(color + " ");
      } else {
        renderString.emplace_back(" ");
      }
    }
    renderString.back().append(EC_RESET_BG_COLOR);
  }

  for (auto &row : rows) {

    for (auto gapI = 0; gapI < gap.x; gapI++) {
      if (gapI == 0) {
        renderString.emplace_back(color + " ");
      } else {
        renderString.emplace_back(" ");
      }
    }

    for (auto &charItem : row) {
      renderString.emplace_back(1, charItem);
    }
    if (row.size() < maxWidth) {
      size_t emptySize = maxWidth - row.size();
      for (auto i = 0; i < emptySize; i++) {
        renderString.emplace_back(" ");
      }
    }

    for (auto gapI = 0; gapI < gap.x; gapI++) {
      renderString.emplace_back(" ");
    }

    renderString.back().append(EC_RESET_BG_COLOR);
  }

  for (auto gapI = 0; gapI < gap.y; gapI++) {
    for (auto colI = 0; colI < maxWidth + static_cast<size_t>(gap.x) * 2;
         colI++) {
      if (colI == 0) {
        renderString.emplace_back(color + " ");
      } else {
        renderString.emplace_back(" ");
      }
    }
    renderString.back().append(EC_RESET_BG_COLOR);
  }

  uiBox.rows = renderString;
  uiBox.width = static_cast<size_t>(gap.x * 2) + maxWidth;

  return uiBox;
}

UiBoxRender ui = makeUiBox("Hello\nworld!!!");

std::string draw(IVec2 pos, DrawCharState state) {
  IVec2 uiSize = {static_cast<int>(ui.width),
                  static_cast<int>(ui.rows.size() / ui.width)};
  IVec2 uiPos;

  // Center ui
  uiPos.x = (state.size.x - uiSize.x) / 2;
  uiPos.y = (state.size.y - uiSize.y) / 2;

  IVec2 relUiPos = {
      pos.x - uiPos.x,
      pos.y - uiPos.y,
  };

  bool isWithinUiBox = relUiPos.x >= 0 && relUiPos.x < uiSize.x &&
                       relUiPos.y >= 0 && relUiPos.y < uiSize.y;

  if (isWithinUiBox) {
    std::string uiChar =
        ui.rows.at(static_cast<size_t>(relUiPos.x) + (relUiPos.y * ui.width));
    return uiChar;
  } else {
    return drawBackground(pos, state);
  }
}

void runTextMode() {
  while (true) {
    int randomNumber = getRandomInt({.min = 1, .max = 1000});
    std::cout << "Find the closest roots for the number: " << randomNumber
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

int lastRandomNumber = -1;
int randomNumber = -1;
std::string lastAnswer = "";

void afterFrame() {
  std::string uiText = "";
  if (lastRandomNumber != -1) {
    double root = std::sqrt(lastRandomNumber);
    int intBefore = floor(root);
    int intAfter = ceil(root);
    double lastAnswerNumber = std::stod(lastAnswer);
    bool isCorrect = abs(lastAnswerNumber - root) < 1;
    uiText += "Last number: " + std::to_string(lastRandomNumber) + ". ";
    uiText += "Your answer is ";
    // uiText += (isCorrect ? (ecRgbI(70, 140, 70) + "correct.")
    //                      : (ecRgbI(140, 70, 70) + "wrong!"));
    uiText += (isCorrect ? "correct." : "wrong!");
    // uiText += EC_RESET_TEXT_COLOR;
    uiText += "\n";
    uiText += "The root: " + std::to_string(intBefore) +
              "^2=" + std::to_string(intBefore * intBefore) + " - " +
              std::to_string(intAfter) +
              "^2=" + std::to_string(intAfter * intAfter) + "   | " +
              std::to_string(root) + "\n\n";
  }

  bool didAnswerChange = RawTerm::submittedBuffer != lastAnswer;

  if (randomNumber == -1 || didAnswerChange) {
    lastRandomNumber = randomNumber;
    randomNumber = getRandomInt({1, 1000});
    lastAnswer = RawTerm::submittedBuffer;
  }

  uiText +=
      "Find the closest roots for the number: " + std::to_string(randomNumber);

  ui = makeUiBox(uiText);
}

void runGraphicsMode() { initRenderer(draw, &afterFrame); }

int main(int argc, char *argv[]) {
  auto cliAppOptsRes = CliAppOpts::initCliApp(argc, argv);
  if (cliAppOptsRes.errorMessages.size() > 0) {
    std::cerr << joinStringVector(cliAppOptsRes.errorMessages, "\n") << "\n";
    return 1;
  }
  if (cliAppOptsRes.opts.graphicsMode == CliAppOpts::GraphicsMode::TEXT) {
    runTextMode();
  } else {
    runGraphicsMode();
  }

  return 0;
}