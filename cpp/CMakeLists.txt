cmake_minimum_required(VERSION 3.20)

project(math_exercises_cpp VERSION 0.1.0 LANGUAGES CXX)

add_executable(factor_number
  src/cli/factorNumber.cpp
  src/questions/makeFactorNumberQuestion.cpp
  src/util/getRandomInt.cpp
)
add_executable(find_closest_root
  src/cli/findClosestRoot.cpp
  src/util/getRandomInt.cpp
)

find_library(EDITLINE_LIBRARY
  NAMES edit
  REQUIRED
)

set(EXECUTABLES factor_number find_closest_root)

foreach (executable IN LISTS EXECUTABLES)
  target_link_libraries(${executable} PRIVATE ${EDITLINE_LIBRARY})

  target_compile_features(${executable} PRIVATE cxx_std_20)
  target_include_directories(${executable} PRIVATE src)

  install(TARGETS ${executable} RUNTIME DESTINATION bin)
endforeach()

