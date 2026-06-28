
#include "util/draw.hpp"
#include "util/termEscape.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <csignal>
#include <iostream>
#include <numbers>
#include <string>
#include <sys/ioctl.h>
#include <sys/ttycom.h>
#include <thread>
#include <unistd.h>

Draw draw;

namespace {
volatile std::sig_atomic_t shouldStop = 0;

void handleShutdownSignal(int) { shouldStop = 1; }

struct TerminalSession {
  TerminalSession() {
    std::cout << EC_ENTER_ALT_SCREEN << EC_HIDE_CURSOR << EC_ERASE_SCREEN
              << EC_CURSOR_HOME;
    std::cout.flush();
  }

  ~TerminalSession() {
    std::cout << EC_SHOW_CURSOR << EC_LEAVE_ALT_SCREEN << EC_RESET_STYLE;
    std::cout.flush();
  }
};
} // namespace

bool isInRange(double val, double target, double range) {
  return (val > target - range || val > target + range);
}

struct DrawState {
  double timeMs;
  IVec2 pixelSize;
};
std::string draw(IVec2 coord, IVec2 size, DrawState state) {
  DVec2 uv{static_cast<double>(coord.x) / size.x,
           static_cast<double>(coord.y) / size.y};
  uv.x = uv.x * 2 - 1;
  uv.y = uv.y * 2 - 1;
  uv.x *= static_cast<double>(size.x) / size.y;
  DVec2 cellSize = {static_cast<double>(state.pixelSize.x) / size.x,
                    static_cast<double>(state.pixelSize.y) / size.y};
  uv.x *= cellSize.x / cellSize.y;

  double timeSin = -cos(2 * std::numbers::pi * (state.timeMs / 3000));
  double sinAnimNorm = timeSin * 0.5 + 0.5;

  double circleRadius = 0.2;

  double distance = uv.x * uv.x + uv.y * uv.y;

  double glowMask = (1 - std::min(1., distance)) * sinAnimNorm;

  return ecRgbD(glowMask, glowMask, glowMask) + "•" + EC_RESET_TEXT_COLOR;
}

struct TermSizeInfo {
  IVec2 charSize;
  IVec2 pixelSize;
};

TermSizeInfo getTerminalSize() {
  winsize size;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
  return TermSizeInfo{IVec2{.x = size.ws_col, .y = size.ws_row},
                      IVec2{.x = size.ws_xpixel, .y = size.ws_ypixel}};
}

int main() {
  using namespace std::chrono_literals;
  std::signal(SIGINT, handleShutdownSignal);
  std::signal(SIGTERM, handleShutdownSignal);

  TerminalSession terminalSession;
  TermSizeInfo termSizeInfo;

  auto startTime = std::chrono::steady_clock::now();
  while (!shouldStop) {
    TermSizeInfo nextTermSizeInfo = getTerminalSize();
    bool isTermSizeChanged =
        termSizeInfo.charSize.x != nextTermSizeInfo.charSize.x ||
        termSizeInfo.charSize.y != nextTermSizeInfo.charSize.y;
    if (isTermSizeChanged) {
      std::cout << EC_ERASE_SCREEN;
    }
    termSizeInfo = nextTermSizeInfo;
    IVec2 termSize = nextTermSizeInfo.charSize;
    std::string frameStr;
    frameStr.reserve((termSize.x + 4) * termSize.y);
    auto timeNow = std::chrono::steady_clock::now();
    auto msPassed = (timeNow - startTime) / 1ms;
    for (int iy = 0; iy < termSize.y; iy++) {
      for (auto ix = 0; ix < termSize.x; ix++) {
        frameStr += draw({.x = ix, .y = iy}, nextTermSizeInfo.charSize,
                         {.timeMs = static_cast<double>(msPassed),
                          .pixelSize = nextTermSizeInfo.pixelSize});
      }
      if (iy < termSize.y - 1) {
        frameStr += "\n";
      }
    }
    std::cout << EC_CURSOR_HOME << ecBgRgbI(0, 0, 0) << frameStr;

    std::cout.flush();
    std::this_thread::sleep_for(16ms);
  }
  return 0;
}
