#include "argparse/parse.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
  argparse_parse(argc, argv);

  if (g_verbose_flag == 1)
  {
    puts("verbose flag is set");
  }

  return EXIT_SUCCESS;
}
