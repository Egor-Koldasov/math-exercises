type GetRandomIntOpts = {
  min?: number
  max: number
}
export const getRandomInt = (opts: GetRandomIntOpts) => {
  const { min = 0, max } = opts

  const byteSize = 16

  const range = max - min

  const a = new Uint16Array(1)
  crypto.getRandomValues(a)

  const randomNorm = a[0]! / (Math.pow(2, byteSize) + 1)

  // Inclusive
  const randomInter = Math.round(randomNorm * range + min)

  return randomInter
}
