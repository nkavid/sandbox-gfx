#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
extern int verbose_flag;

void argparse_parse(int argc, char** argv);

#ifdef __cplusplus
}
#endif
