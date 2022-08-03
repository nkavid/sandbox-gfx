#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct option;

struct argparse_print_description
{
    long num_tabs_needed;
    const char* string;
};

void argparse_print_usage(void);

void argparse_print_description(const char* description);

void argparse_print_options(const struct option* options,
                            const struct argparse_print_description* desc_options,
                            unsigned num_options);

void argparse_print_positional(const char* positional);

void argparse_print_help(void);

#ifdef __cplusplus
}
#endif
