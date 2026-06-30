#include "util/draw.hpp"
#include "util/listenShutdownSignal.hpp"
#include "util/termEscape.hpp"
#include <iostream>
#include <sys/ioctl.h>
#include <sys/ttycom.h>
#include <thread>
#include <unistd.h>

#define DEFAULT_CHAR_RESERVE 4

void enterFullScreen() {
  std::cout << EC_ENTER_ALT_SCREEN << EC_HIDE_CURSOR << EC_ERASE_SCREEN
            << EC_CURSOR_HOME;
  std::cout.flush();
}
void exitFullScreen() {
  std::cout << EC_SHOW_CURSOR << EC_LEAVE_ALT_SCREEN << EC_RESET_STYLE;
  std::cout.flush();
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

void exitRenderer() { exitFullScreen(); }

void initRenderer(Draw *drawChar) {
  using namespace std::chrono_literals;
  enterFullScreen();
  listenShutdownSignal();
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

    std::string frameStr;
    frameStr.reserve((static_cast<unsigned long>(termSizeInfo.charSize.x) +
                      DEFAULT_CHAR_RESERVE) *
                     termSizeInfo.charSize.y);

    auto timeNow = std::chrono::steady_clock::now();
    auto msPassed = (timeNow - startTime) / 1ms;

    for (int iy = 0; iy < termSizeInfo.charSize.y; iy++) {
      for (auto ix = 0; ix < termSizeInfo.charSize.x; ix++) {
        frameStr +=
            drawChar({.x = ix, .y = iy},
                     SystemState{.size = nextTermSizeInfo.charSize,
                                 .pixelSize = nextTermSizeInfo.pixelSize,
                                 .timeMs = static_cast<double>(msPassed)});
      }
      if (iy < termSizeInfo.charSize.y - 1) {
        frameStr += "\n";
      }
    }

    std::cout << EC_CURSOR_HOME << ecBgRgbI(0, 0, 0) << frameStr;
    std::cout.flush();
    std::this_thread::sleep_for(16ms);
  }
  exitRenderer();
}
