add_executable(uit_tests
  islist.cpp
  isdlist.cpp
  idslist.cpp
  idlist.cpp
)
target_include_directories(uit_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(uit_tests PRIVATE
  uit
  Catch2::Catch2WithMain
)

add_subdirectory(experiment)