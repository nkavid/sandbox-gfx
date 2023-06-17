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

set(CMAKE_CUDA_HOST_COMPILER clang)
set(CMAKE_CUDA_COMPILER clang)

string(APPEND CMAKE_CUDA_FLAGS "-nostdinc++ -std=c++20")

string(
  APPEND
  GFX_CLANG_ASAN
  " -fsanitize=address"
  " -fsanitize=leak"
  " -fno-omit-frame-pointer"
  " -fsanitize-address-use-after-scope"
  " -fno-optimize-sibling-calls"
  " -fno-common"
)

if(GFX_ASAN)
  string(ASCII 27 Esc)
  set(RED "${Esc}[31m")
  set(YELLOW "${Esc}[93m")
  set(RESET "${Esc}[m")

  if(NOT DEFINED ENV{ASAN_OPTIONS})
    message(
      NOTICE
      "${RESET}${YELLOW}"
      "export ASAN_OPTIONS=protect_shadow_gap=0,fast_unwind_on_malloc=0"
      "\n"
      "export LSAN_OPTIONS=suppressions=${CMAKE_SOURCE_DIR}/tools/lsan.supp"
      "${RESET}"
    )

    string(
      APPEND
      ASAN_ENV_MSG
      "${RESET}${YELLOW}"
      "gfx::Set ASAN environment variable for run-time flags:"
      "${RESET}${RED}\n"
    )

    message(FATAL_ERROR "${ASAN_ENV_MSG}")
  endif()
  string(APPEND CMAKE_C_FLAGS ${GFX_CLANG_ASAN})
  string(APPEND CMAKE_CXX_FLAGS ${GFX_CLANG_ASAN})
endif()

# Call in project listing after configuring build to log in terminal
function(gfx_print_asan_info)
  if(GFX_ASAN)
    message(NOTICE "${RESET}${YELLOW}ASAN_OPTIONS=$ENV{ASAN_OPTIONS}${RESET}")
    message(NOTICE "${RESET}${YELLOW}LSAN_OPTIONS=$ENV{LSAN_OPTIONS}${RESET}")
    message(NOTICE "Disable unloading to not confuse sanitizers:")
    set(fake_dl_close "./libdlclose.so")
    message(
      NOTICE
      "${RESET}${YELLOW}clang --shared "
      "../tools/fake/dlclose.c"
      " -o libdlclose.so${RESET}"
    )
    message(
      NOTICE
      "${RESET}${YELLOW}LD_PRELOAD=${fake_dl_close} <executable>${RESET}"
    )
  endif()
endfunction()
