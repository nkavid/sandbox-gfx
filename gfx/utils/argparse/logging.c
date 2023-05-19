#include "logging.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK_ERROR(VALUE, ERROR)                                                      \
  if ((VALUE) == (ERROR))                                                              \
  {                                                                                    \
    LOG_ERROR(VALUE)                                                                   \
  }

// NOLINTBEGIN(concurrency-mt-unsafe)
#define LOG_ERROR(VALUE)                                                               \
  if (fprintf(stderr,                                                                  \
              "%s:%d - Unexpected return value for '%s' with errno %s\n",              \
              __FILE__,                                                                \
              __LINE__,                                                                \
              #VALUE,                                                                  \
              strerror(errno))                                                         \
      <= 0)                                                                            \
  {                                                                                    \
    puts("failed LOG_ERROR");                                                          \
    puts(strerror(errno));                                                             \
    abort();                                                                           \
  }

// NOLINTEND(concurrency-mt-unsafe)

// NOLINTNEXTLINE(readability-function-size)
const char* logging_get_executable_name(void)
{
  // NOLINTNEXTLINE(clang-diagnostic-unsafe-buffer-usage)
  static char executable_name[16] = ""; // NOLINT(readability-magic-numbers)

  FILE* filePtr = fopen("/proc/self/comm", "r");

  CHECK_ERROR(filePtr, NULL)

  CHECK_ERROR(fscanf(filePtr, "%s", executable_name), EOF)

  CHECK_ERROR(fclose(filePtr), EOF)

  // NOLINTNEXTLINE(readability-magic-numbers)
  if (strlen(executable_name) >= 15)
  {
    // NOLINTBEGIN(readability-magic-numbers)
    executable_name[12] = '.';
    executable_name[13] = '.';
    executable_name[14] = '.';
    executable_name[15] = '\0';
    // NOLINTEND(readability-magic-numbers)
  }

  return executable_name;
}
