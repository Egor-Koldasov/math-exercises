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

typedef std::string Draw(IVec2 coord, IVec2 size);
