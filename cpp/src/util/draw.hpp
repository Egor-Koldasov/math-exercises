#pragma once

#include <string>

struct IVec2 {
  int x;
  int y;
};
struct DVec2 {
  double x;
  double y;
};
struct DrawCharState {
  IVec2 size;
  IVec2 pixelSize;
  double timeMs;
};

void initRenderer(std::string (*drawChar)(IVec2 coord, DrawCharState state));
