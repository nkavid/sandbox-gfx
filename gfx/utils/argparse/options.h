#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void argparse_alloc_options(unsigned num_options);

void argparse_dealloc_options(void);

void argparse_add_option(unsigned index,
                         const char* long_opt,
                         char short_opt,
                         int has_arg,
                         const char* description);

const char* argparse_options_optstring(void);

const struct argparse_print_description* argparse_options_desc_opts(void);

const struct option* argparse_options_long_opts(void);

#ifdef __cplusplus
}
#endif
