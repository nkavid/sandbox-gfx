set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)

set(CMAKE_CXX_FLAGS "-stdlib=libc++")

string(
  APPEND
  CMAKE_CXX_FLAGS
  " -Weverything"
  " -Wno-c++98-compat"
  " -Wno-c++98-compat-pedantic"
  " -Wno-switch-enum"
  " -Wno-padded"
)

string(
  APPEND
  CMAKE_C_FLAGS
  " -Wno-declaration-after-statement"
  " -Weverything"
  " -Wno-switch-enum"
  " -Wno-padded"
  " -Wno-unsafe-buffer-usage"
)

set(CUDA_HOST_COMPILER clang)
set(CMAKE_CUDA_COMPILER clang)

string(APPEND CMAKE_CUDA_FLAGS " -std=c++20")
