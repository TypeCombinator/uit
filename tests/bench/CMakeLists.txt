add_executable(bench
  isbt.cpp
  linux_irbt.cpp
  freebsd_irbt.cpp
)
target_link_libraries(bench PRIVATE
  uit
  benchmark::benchmark
  benchmark::benchmark_main
)