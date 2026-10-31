set(BENCHMARK_ENABLE_TESTING OFF CACHE INTERNAL "")
FetchContent_Declare(
  GoogleBenchmark
  URL https://github.com/google/benchmark/archive/refs/tags/v1.10.0.tar.gz
  URL_HASH SHA256=acc5c7c0fbed855c4aaa2d7c3c36fb82b0c15c28f4e151e3f679c2fc0aa404af
  DOWNLOAD_EXTRACT_TIMESTAMP ON
)
FetchContent_MakeAvailable(GoogleBenchmark)