#include "util/draw.hpp"
#include "util/listenShutdownSignal.hpp"
#include "util/termEscape.hpp"
#include <iostream>
#include <sys/ioctl.h>
#include <sys/ttycom.h>
#include <termios.h>
#include <thread>
#include <unistd.h>

#define DEFAULT_CHAR_RESERVE 4

namespace RawTerm {
termios oldTermios;

void enableRawInput() {
  tcgetattr(STDIN_FILENO, &oldTermios);

  termios raw = oldTermios;
  raw.c_lflag &= ~(ICANON | ECHO);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 0;

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disableRawInput() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldTermios); }

std::string inputBuffer;
std::string submittedBuffer;
char ch;
void readInput() {
  while (read(STDIN_FILENO, &ch, 1) == 1) {
    if (ch == '\n' || ch == '\r') {
      submittedBuffer = inputBuffer;
      inputBuffer.clear();
    } else if (ch == 127 || ch == '\b') {
      if (!inputBuffer.empty())
        inputBuffer.pop_back();
    } else {
      inputBuffer += ch;
    }
  }
}
} // namespace RawTerm

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

void initRenderer(std::string (*drawChar)(IVec2 coord, DrawCharState state),
                  void (*afterFrame)()) {
  using namespace std::chrono_literals;
  enterFullScreen();
  listenShutdownSignal();
  RawTerm::enableRawInput();
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

    IVec2 canvasSize = termSizeInfo.charSize;
    canvasSize.y = canvasSize.y - 1;

    std::string frameStr;
    frameStr.reserve(
        (static_cast<unsigned long>(canvasSize.x) + DEFAULT_CHAR_RESERVE) *
        canvasSize.y);

    auto timeNow = std::chrono::steady_clock::now();
    auto msPassed = (timeNow - startTime) / 1ms;

    for (int iy = 0; iy < canvasSize.y; iy++) {
      for (auto ix = 0; ix < canvasSize.x; ix++) {
        frameStr +=
            drawChar({.x = ix, .y = iy},
                     DrawCharState{.size = canvasSize,
                                   .pixelSize = termSizeInfo.pixelSize,
                                   .timeMs = static_cast<double>(msPassed)});
      }
      if (iy < canvasSize.y - 1) {
        frameStr += "\n";
      }
    }

    std::cout << EC_CURSOR_HOME << ecBgRgbI(0, 0, 0) << frameStr << "\n";
    std::cout.flush();
    RawTerm::readInput();
    std::cout << RawTerm::inputBuffer << EC_ERASE_LINE_END;
    std::cout.flush();
    if (afterFrame != nullptr) {
      afterFrame();
    }
    std::this_thread::sleep_for(16ms);
  }
  RawTerm::disableRawInput();
  exitRenderer();
}
