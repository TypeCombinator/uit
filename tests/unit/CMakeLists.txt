add_executable(uit_tests
  idlist.cpp
  islist.cpp
  isdlist.cpp
  idslist.cpp
  irsbt.cpp
  irwbt.cpp
)
target_include_directories(uit_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(uit_tests PRIVATE
  uit
  gtest_main
)