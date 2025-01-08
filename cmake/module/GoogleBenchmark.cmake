set(BENCHMARK_ENABLE_TESTING OFF CACHE INTERNAL "")
FetchContent_Declare(
  GoogleBenchmark
  URL https://github.com/google/benchmark/archive/refs/tags/v1.9.0.tar.gz
  URL_HASH MD5=21a2604efeded8b4cbabc72f3e1c7a2a
  DOWNLOAD_EXTRACT_TIMESTAMP ON
)
FetchContent_MakeAvailable(GoogleBenchmark)