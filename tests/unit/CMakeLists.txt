add_executable(uit_tests
  intrusive.cpp
  islist.cpp
  isdlist.cpp
  idslist.cpp
  idlist.cpp
  idlist_proxy.cpp
)
target_include_directories(uit_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(uit_tests PRIVATE
  uit
  Catch2::Catch2WithMain
)

# add_subdirectory(experiment)

enable_testing()

# list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

catch_discover_tests(uit_tests
  DL_PATHS
    # Set by cmake/module/asan.cmake， it's non-empty only when building
    # with clang-cl and using ASan.
    "${UIT_CLANG_CL_RT_DIR}"
)

# ctest --test-dir build/release/tests/unit --output-on-failure