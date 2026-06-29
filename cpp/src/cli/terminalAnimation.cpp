
#include "util/draw.hpp"
#include "util/termEscape.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <csignal>
#include <iostream>
#include <numbers>
#include <random>
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

double fract(double x) { return x - std::floor(x); }

double rand0(double x) { return fract(sin(x) * 1000000.); }

double rand1(double x) {
  std::uint64_t xBits;
  std::memcpy(&xBits, &x, sizeof(xBits));
  std::mt19937_64 randomGenerator(xBits);
  std::uniform_real_distribution<double> makeRandomNorm(0, 1);
  return makeRandomNorm(randomGenerator);
}

double angleRand(double angleStep) { return rand0((10. + angleStep * 67.423)); }

double smoothstep(double edge0, double edge1, double x) {
  double t = std::clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
  return t * t * (3.0 - 2.0 * t);
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

  double angle = atan2(uv.y, uv.x);
  double angleNormalized =
      std::min(0.9999999, (angle + std::numbers::pi) / (2 * std::numbers::pi));
  // angleNormalized = std::min(angleNormalized, 0.99999999999);
  // angleNormalized = std::max(angleNormalized, 0.00000000001);
  double angleCount = 7;
  double angleStep = floor(angleNormalized * angleCount) + 1;

  double angleRandSharpMask =
      std::lerp(angleRand(angleStep / angleCount),
                angleRand((fmod(angleStep, angleCount) + 1.) / angleCount),
                smoothstep(0., 1., fract((angleNormalized * angleCount))));

  double animatedAngleRandSharpMask =
      -cos(2 * std::numbers::pi * (state.timeMs / 3000 + angleRandSharpMask)) *
          0.5 +
      0.5;

  double glowMask = (1 - std::min(1., distance));

  double outMask = std::max(0., glowMask - animatedAngleRandSharpMask * 0.3);

  // return ecBgRgbD(outMask * rand1((angleStep / angleCount)),
  //                 outMask * rand1((angleStep / angleCount) + 1000),
  //                 outMask * rand1((angleStep / angleCount) + 99999)) +
  //        " " + EC_RESET_BG_COLOR;
  // return ecBgRgbD(outMask, outMask, outMask) + " " + EC_RESET_BG_COLOR;
  return ecRgbD(outMask, outMask, outMask) + "•" + EC_RESET_TEXT_COLOR;
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
