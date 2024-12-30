add_executable(uit_tests
  idlist.cpp
  islist.cpp
  isdlist.cpp
  idslist.cpp
  isbt.cpp
)
target_link_libraries(uit_tests PRIVATE
  uit
  gtest_main
)