import { createInterface } from "node:readline/promises"
import { makeFactorNumberQuestion } from "../questions/makeFactorNumberQuestion"

const printInputError = () => {
  console.log("Invalid input")
  printQuestionEndGap()
}
const printQuestionEndGap = () => {
  console.log("\n".repeat(3))
}
const printCorrect = (isCorrect: boolean) =>
  console.log(isCorrect ? `Correct!` : `Wrong.`)

const matchProducts = (a: number[], b: number[]) => {
  const aSorted = [...a].sort()
  const bSorted = [...b].sort()
  for (let i = 0; i < aSorted.length; i++) {
    if (aSorted[i] !== bSorted[i]) {
      return false
    }
  }
  return true
}

export const factorNumber = async () => {
  const cli = createInterface({
    input: process.stdin,
    output: process.stdout,
  })
  const controller = new AbortController()
  const signal = controller.signal

  while (!signal.aborted) {
    try {
      const factorQuestion = makeFactorNumberQuestion()
      const userInput = await cli.question(
        `Factor ${factorQuestion.questionNumber}\nEnter two numbers: `,
        { signal },
      )
      const userInputNumberStrs = userInput.match(/\d+/g)
      if (userInputNumberStrs?.length !== 2) {
        printInputError()
        continue
      }
      const userInputNumbers: number[] = []
      let isParsingValid = true
      for (let numberStr of userInputNumberStrs) {
        const inputNumber = parseInt(numberStr)
        if (!isFinite(inputNumber)) {
          isParsingValid = false
          break
        }
        userInputNumbers.push(inputNumber)
      }
      // TODO: try again
      if (!isParsingValid) {
        printInputError()
        continue
      }

      if (factorQuestion.products.length === 0) {
        const isCorrect = matchProducts(userInputNumbers, [
          1,
          factorQuestion.questionNumber,
        ])
        printCorrect(isCorrect)
        console.log(`This is a prime number.`)
        printQuestionEndGap()
        continue
      }

      const biggestFactorProducts =
        factorQuestion.products[factorQuestion.products.length - 1]!
      const isBiggestFactorGuessed = matchProducts(
        userInputNumbers,
        biggestFactorProducts,
      )
      if (isBiggestFactorGuessed) {
        printCorrect(true)
        console.log(`You found the biggest factor!`)
        printQuestionEndGap()
        continue
      }

      if (!isBiggestFactorGuessed) {
        const factorFound = factorQuestion.products.find((p) =>
          matchProducts(userInputNumbers, p),
        )
        printCorrect(!!factorFound)
        // TODO: try again
        if (!!factorFound) {
          console.log(
            `But the biggest factor is ${biggestFactorProducts[0]!} * ${biggestFactorProducts[1]!}`,
          )
        } else {
          console.log(
            `The biggest factor is ${biggestFactorProducts[0]!} * ${biggestFactorProducts[1]!}`,
          )
        }
        printQuestionEndGap()
        continue
      }
    } catch (err) {
      controller.abort(err)
    }
  }
}

factorNumber()
