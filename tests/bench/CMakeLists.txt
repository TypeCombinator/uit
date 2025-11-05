add_executable(bench
  irsbt.cpp
  linux_irbt.cpp
  freebsd_irbt.cpp
)
target_include_directories(bench PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(bench PRIVATE
  uit
  benchmark::benchmark
  benchmark::benchmark_main
)