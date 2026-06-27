cmake_minimum_required(VERSION 3.20)

project(math_exercises_cpp VERSION 0.1.0 LANGUAGES CXX)

add_executable(factor_number
  src/cli/factorNumber.cpp
  src/questions/makeFactorNumberQuestion.cpp
  src/util/getRandomInt.cpp
)

find_library(EDITLINE_LIBRARY
  NAMES edit
  REQUIRED
)

target_link_libraries(factor_number PRIVATE ${EDITLINE_LIBRARY})

target_compile_features(factor_number PRIVATE cxx_std_20)
target_include_directories(factor_number PRIVATE src)

install(TARGETS factor_number RUNTIME DESTINATION bin)