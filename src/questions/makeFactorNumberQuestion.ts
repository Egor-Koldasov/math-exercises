import { getRandomInt } from "../util/getRandomInt"

type FactorNumberQuestionOpts = {
  min?: number
  max?: number
}

type FactorNumberQuestion = {
  questionNumber: number
  products: number[][]
}

export const makeFactorNumberQuestion = (
  opts: FactorNumberQuestionOpts = {},
): FactorNumberQuestion => {
  const { min = 1, max = 1000 } = opts
  const questionNumber = getRandomInt({ max, min })
  const question: FactorNumberQuestion = {
    questionNumber,
    products: [],
  }

  for (let i = 2; i * i <= questionNumber; i++) {
    // for (let i = questionNumber - 1; i * i >= questionNumber; i--) {
    if (questionNumber % i === 0) {
      question.products.push([i, questionNumber / i])
    }
  }

  return question
}
