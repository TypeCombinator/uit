cmake_minimum_required(VERSION 3.20)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/module")

option(UIT_BUILD_TESTING "Build testing" OFF)

project (project_uit LANGUAGES C CXX)

include(FetchContent)

# Set C++ standard.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add extra c/c++ flags.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_COMPILER_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_COMPILER_FLAGS}")

add_library(uit INTERFACE)
target_include_directories(uit INTERFACE include)

add_subdirectory(examples)

if(UIT_BUILD_TESTING)
  include(GoogleTest)
  add_subdirectory(tests/unit)

  # include(GoogleBenchmark)
  # add_subdirectory(tests/bench)
  
endif()