add_executable(uit_experiment_tests
  islist.cpp
  isdlist.cpp
  idslist.cpp
  idlist.cpp
)
target_include_directories(uit_experiment_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(uit_experiment_tests PRIVATE
  uit
  gtest_main
  # fmt::fmt
)