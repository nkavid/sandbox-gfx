#include "argparse/parse.h"

#include "argparse/options.h"
#include "argparse/print.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_OPTIONS 2

int verbose_flag = 0;

static void argparse_add_options(void)
{
  argparse_alloc_options(NUM_OPTIONS);
  argparse_add_option(1, "verbose", 'v', no_argument, "");
  // NOLINTNEXTLINE(readability-magic-numbers)
  argparse_add_option(2, "dir", 'd', required_argument, "this is input");
}

void argparse_print_help(void)
{
  argparse_print_usage();
  argparse_print_description("this is a description");
  argparse_print_options(argparse_options_long_opts(),
                         argparse_options_desc_opts(),
                         NUM_OPTIONS);
  argparse_print_positional("space separated list of stuff");
}

static void argparse_parse_options(int argc, char** argv)
{
  if (argc == 1)
  {
    argparse_print_usage();
    exit(EXIT_FAILURE);
  }

  int character = 0;

  while (character != -1)
  {
    int option_index = 0;

    character = getopt_long(argc,
                            argv,
                            argparse_options_optstring(),
                            argparse_options_long_opts(),
                            &option_index);

    if (character == -1)
    {
      break;
    }

    switch (character)
    {
    case 'h':
      argparse_print_help();
      argparse_dealloc_options();
      exit(EXIT_SUCCESS);

    case 'v':
      puts("option -v");
      verbose_flag = 1;
      break;

    case 'd':
      printf("option -d with value `%s'\n", optarg);
      break;

    case '?':
      printf("option ? with value `%s'\n", optarg);
      break;

    default:
      printf("error, unexpected case\n");
      abort();
    }
  }
}

static void argparse_parse_positional(int argc, char** argv)
{
  if (optind < argc)
  {
    printf("non-option argv elements: ");

    while (optind < argc)
    {
      printf("%s ", argv[optind++]);
    }

    putchar('\n');
  }
}

void argparse_parse(int argc, char** argv)
{
  argparse_add_options();
  argparse_parse_options(argc, argv);
  argparse_parse_positional(argc, argv);
  argparse_dealloc_options();
}
