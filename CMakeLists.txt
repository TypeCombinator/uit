cmake_minimum_required(VERSION 3.20)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/module")

option(UIT_BUILD_TESTS "Build tests" OFF)
option(UIT_BUILD_EXAMPLES "Build examples" ON)

project(project_uit LANGUAGES C CXX)

if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
  set(UIT_IS_TOP_LEVEL_PROJECT ON)
else()
  set(UIT_IS_TOP_LEVEL_PROJECT OFF)
endif()

if(UIT_IS_TOP_LEVEL_PROJECT)
  include(FetchContent)

  # Set C++ standard.
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)

  # Add extra c/c++ flags.
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_COMPILER_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_COMPILER_FLAGS}")
endif()

add_library(uit INTERFACE)
target_include_directories(uit INTERFACE include)

if(UIT_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(UIT_BUILD_TESTS)
  include(GoogleTest)
  add_subdirectory(tests/unit)

  include(GoogleBenchmark)
  add_subdirectory(tests/bench)
endif()