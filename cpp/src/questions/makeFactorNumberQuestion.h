#pragma once

#include <vector>

typedef std::array<int, 2> Products2;

struct FactorNumberQuestionOpts {
  int min = 1;
  int max = 1000;
};

struct FactorNumberQuestion {
  int questionNumber;
  std::vector<Products2> products;
};

FactorNumberQuestion
makeFactorNumberQuestion(FactorNumberQuestionOpts opts = {});
