#include "questions/makeFactorNumberQuestion.h"
#include <algorithm>
#include <histedit.h>
#include <iostream>
#include <regex>
#include <string>

struct PromptState {
  int factorsFilled;
};

auto getPrompt(EditLine *elApp_) {
  PromptState *promptState = nullptr;
  el_get(elApp_, EL_CLIENTDATA, &promptState);
  if (promptState == nullptr) {
    promptState = new PromptState{.factorsFilled = 0};
  }
  switch (promptState->factorsFilled) {
  case 0:
    return "Enter two numbers: ";
  case 1:
    return "Enter one more number: ";
  default:
    return "I'm broken";
  }
}

std::regex getNumberRegex() {
  static const std::regex numberRegex(R"(\d+)");
  return numberRegex;
}

Products2 readProducts(EditLine *elApp) {
  PromptState promptState = {.factorsFilled = 0};
  el_set(elApp, EL_CLIENTDATA, &promptState);
  std::regex numberRegex = getNumberRegex();
  // Accumulate user input until it's valid
  Products2 products{};
  bool isInputValid = false;
  while (2 - promptState.factorsFilled > 0) {
    int numbersLeft = 2 - promptState.factorsFilled;
    // Read the line string
    int inputLength;
    std::string lineStr = el_gets(elApp, &inputLength);
    // Convert the string into the array on number strings
    std::sregex_iterator numberRi(lineStr.begin(), lineStr.end(), numberRegex);
    std::sregex_iterator riEnd;

    if (numberRi->size() > numbersLeft) {
      std::cout << "Too many numbers. Using the first two.\n";
      break;
    }

    for (int i = 0; i < numbersLeft; i++) {
      if (numberRi == riEnd) {
        break;
      }
      const std::smatch &numberSmatch = *numberRi;
      int number = std::stoi(numberSmatch.str());
      numberRi++;
      products[promptState.factorsFilled] = number;
      promptState.factorsFilled++;
      if (i == 1) {
        isInputValid = true;
      }
      el_set(elApp, EL_CLIENTDATA, &promptState);
    }
  }
  return products;
}

bool matchProducts2(Products2 a, Products2 b) {
  Products2 sortedA{a[0], a[1]};
  Products2 sortedB{b[0], b[1]};

  std::ranges::sort(sortedA);
  std::ranges::sort(sortedB);

  for (auto i = 0; i < 2; i++) {
    if (sortedA[i] != sortedB[i])
      return false;
  }

  return true;
}

int main() {
  auto elApp = el_init("FactorNumber", stdin, stdout, stderr);

  // Set the prompt
  auto res = el_set(elApp, EL_PROMPT, &getPrompt);
  if (res != 0) {
    std::cout << "Failed to set the prompt\n" << res;
  }

  while (true) {
    FactorNumberQuestion question = makeFactorNumberQuestion();
    std::cout << "Factor " << question.questionNumber << "\n";
    auto userInputProducts = readProducts(elApp);

    Products2 *biggestFactor = nullptr;
    if (question.products.size() > 0) {
      biggestFactor = &question.products[question.products.size() - 1];
    }
    bool isCorrectGuess = false;
    bool isBiggestFactorGuessed = false;
    if (biggestFactor == nullptr) {
      isCorrectGuess = matchProducts2(userInputProducts,
                                      Products2{1, question.questionNumber});
    } else {
      for (int i = 0; i < question.products.size(); i++) {
        if (matchProducts2(userInputProducts, question.products[i])) {
          isCorrectGuess = true;
          if (i == question.products.size() - 1) {
            isBiggestFactorGuessed = true;
          }
          break;
        }
      }
    }

    std::cout << (isCorrectGuess ? "Correct!" : "Wrong!") << "\n";
    if (biggestFactor == nullptr) {
      std::cout << "This is a prime number.\n";
    } else if (!isCorrectGuess) {
      std::cout << "The biggest factor is " << (*biggestFactor)[0] << " * "
                << (*biggestFactor)[1] << ".\n";
    } else if (!isBiggestFactorGuessed) {
      std::cout << "But the biggest factor is " << (*biggestFactor)[0] << " * "
                << (*biggestFactor)[1] << ".\n";
    } else {
      std::cout << "You named the biggest factor!\n";
    }
  }
}
