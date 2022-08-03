#include "argparse/print.h"

#include "logging.h"

#include <getopt.h>
#include <stdio.h>

static const char* argparse_print_has_arg(int has_arg)
{
  switch (has_arg)
  {
  case no_argument:
    return "     ";
  case required_argument:
    return "[arg]";
  case optional_argument:
    return "[opt]";
  default:
    return "unexpected has_arg";
  }
}

void argparse_print_usage(void)
{
  printf("usage: %s [-h] %s\n",
         logging_get_executable_name(),
         "[options] [--] [positional]");
  putchar('\n');
}

void argparse_print_description(const char* description)
{
  printf("description: %s\n\n", description);
}

void argparse_print_options(const struct option* options,
                            const struct argparse_print_description* desc_options,
                            unsigned num_options)
{
  puts("options:");
  for (unsigned i = 0; i < (num_options + 1); ++i)
  {
    printf("  -%c, --%s\t %s\t %s\n",
           options[i].val,
           options[i].name,
           argparse_print_has_arg(options[i].has_arg),
           desc_options[i].string);
  }
  putchar('\n');
}

void argparse_print_positional(const char* positional)
{
  puts("positional:");
  printf("  %s\n", positional);
  putchar('\n');
}
