#pragma once

struct GetRandomIntOpts {
  int min = 0;
  int max;
};

int getRandomInt(GetRandomIntOpts opts);
