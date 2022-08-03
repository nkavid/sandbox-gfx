#include "logging.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

// NOLINTBEGIN(readability-magic-numbers)

const char* logging_get_executable_name(void)
{
  static char executable_name[16] = "";

  FILE* filePtr = NULL;
  filePtr       = fopen("/proc/self/comm", "r");
  assert(filePtr != NULL);

  int err = fscanf(filePtr, "%s", executable_name);
  assert(err != EOF);

  assert(fclose(filePtr) != 0);

  if (strlen(executable_name) >= 15)
  {
    executable_name[12] = '.';
    executable_name[13] = '.';
    executable_name[14] = '.';
    executable_name[15] = '\0';
  }

  return executable_name;
}

// NOLINTEND(readability-magic-numbers)
