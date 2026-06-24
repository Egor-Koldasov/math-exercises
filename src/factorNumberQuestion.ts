import { getRandomInt } from "./getRandomInt.ts"

type FactorNumberQuestionOpts = {
  min?: number
  max?: number
}

type FactorNumberQuestion = {
  questionNumber: number
  products: number[][]
}

export const factorNumberQuestion = (
  opts: FactorNumberQuestionOpts = {},
): FactorNumberQuestion => {
  const { min = 1, max = 1000 } = opts
  const questionNumber = getRandomInt({ max, min })
  const question: FactorNumberQuestion = {
    questionNumber,
    products: [[questionNumber, 1]],
  }

  for (let i = questionNumber - 1; i * i >= questionNumber; i--) {
    if (questionNumber % i === 0) {
      question.products.push([i, questionNumber / i])
    }
  }

  return question
}
