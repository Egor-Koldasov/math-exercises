
#include "util/draw.hpp"
#include "util/termEscape.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>
#include <random>
#include <string>
#include <sys/ioctl.h>
#include <sys/ttycom.h>
#include <unistd.h>

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

std::string draw(IVec2 coord, DrawCharState state) {
  DVec2 uv{static_cast<double>(coord.x) / state.size.x,
           static_cast<double>(coord.y) / state.size.y};
  uv.x = uv.x * 2 - 1;
  uv.y = uv.y * 2 - 1;
  uv.x *= static_cast<double>(state.size.x) / state.size.y;
  DVec2 cellSize = {static_cast<double>(state.pixelSize.x) / state.size.x,
                    static_cast<double>(state.pixelSize.y) / state.size.y};
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

int main() {
  initRenderer(&draw);
  return 0;
}
