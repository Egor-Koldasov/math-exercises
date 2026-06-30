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
struct SystemState {
  IVec2 size;
  IVec2 pixelSize;
  double timeMs;
};

typedef std::string Draw(IVec2 coord, SystemState state);

void initRenderer(Draw *drawChar);
