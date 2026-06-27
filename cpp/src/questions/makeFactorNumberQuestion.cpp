#include "questions/makeFactorNumberQuestion.h"
#include "util/getRandomInt.h"

FactorNumberQuestion makeFactorNumberQuestion(FactorNumberQuestionOpts opts)
{
  const int questionNumber = getRandomInt({
      .min = opts.min,
      .max = opts.max,
  });
  FactorNumberQuestion question{
      .questionNumber = questionNumber,
      .products = {},
  };

  for (int i = 2; i * i <= questionNumber; i++)
  {
    if (questionNumber % i == 0)
    {
      int secondProduct = questionNumber / i;
      question.products.push_back({i, secondProduct});
    }
  }

  return question;
}
