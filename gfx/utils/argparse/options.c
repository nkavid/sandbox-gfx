#include "argparse/options.h"

#include "argparse/print.h"

#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
// NOLINTNEXTLINE(clang-diagnostic-unsafe-buffer-usage)
static struct option* g_long_options = NULL;

// NOLINTNEXTLINE(clang-diagnostic-unsafe-buffer-usage)
static struct argparse_print_description* g_desc_options = NULL;

static char* g_optstring = NULL;

static unsigned* g_num_options = NULL;

// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

void argparse_alloc_options(unsigned num_options)
{
  g_num_options                  = malloc(sizeof(unsigned));
  *g_num_options                 = num_options;
  static const unsigned num_help = 1;
  size_t size_all_desc_options   = (num_help + num_options)
                               * sizeof(struct argparse_print_description);

  g_desc_options = malloc(size_all_desc_options);
  memset(g_desc_options, 0, size_all_desc_options);

  static const unsigned help_and_null   = num_help + 1;
  const unsigned long total_num_options = num_options + help_and_null;

  const unsigned long size_long_options = total_num_options * sizeof(struct option);

  g_long_options = malloc(size_long_options);
  memset(g_long_options, 0, size_long_options);

  struct option help = {.name = "help", .has_arg = no_argument, .flag = 0, .val = 'h'};

  g_long_options[0] = help;

  struct argparse_print_description desc = {.string = "print this message and exit",
                                            .num_tabs_needed = 0};

  g_desc_options[0] = desc;

  // NOLINTNEXTLINE(readability-magic-numbers)
  unsigned size_g_optstring = 1 + *g_num_options * 2 + 1;

  g_optstring = malloc(size_g_optstring);
  memset(g_optstring, '\0', size_g_optstring);
  g_optstring[0] = 'h';
}

void argparse_dealloc_options(void)
{
  free(g_long_options);
  free(g_desc_options);
  free(g_optstring);
  free(g_num_options);
}

void argparse_add_option(unsigned index,
                         const char* long_opt,
                         char short_opt,
                         int has_arg,
                         const char* description)
{
  assert((index - 1) < *g_num_options);
  struct option verbose = {.name    = long_opt,
                           .has_arg = has_arg,
                           .flag    = 0,
                           .val     = short_opt};
  g_long_options[index] = verbose;

  int num_tabs = 0;
  // NOLINTNEXTLINE(readability-magic-numbers)
  if (strlen(long_opt) > 6)
  {
    num_tabs = 1;
  }

  struct argparse_print_description desc = {.string          = description,
                                            .num_tabs_needed = num_tabs};

  g_desc_options[index] = desc;

  strncat(g_optstring, &short_opt, 1);
  if (has_arg != no_argument)
  {
    // NOLINTNEXTLINE(readability-magic-numbers)
    strncat(g_optstring, ":", 2);
  }
}

const char* argparse_options_optstring(void)
{
  return g_optstring;
}

const struct argparse_print_description* argparse_options_desc_opts(void)
{
  return g_desc_options;
}

const struct option* argparse_options_long_opts(void)
{
  return g_long_options;
}
