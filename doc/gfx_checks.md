# gfx-checks

## Description

Checks implemented specifically for this repository are prefixed "gfx" similar to other project specific checks in the llvm-project repository.

Partially functioning checks under development are prefixed "gfx-experimental" for easy filtering in `.clang-tidy` configuration file.

## Instructions

```bash
# called from 'llvm-project/' repo root
cmake -Sllvm -Bbuild \
-DCMAKE_CXX_STANDARD=17 \
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
-DCMAKE_BUILD_TYPE=Release \
-DLLVM_TARGETS_TO_BUILD=X86 \
-DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" \
-DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi" \
-DLLVM_EXTERNAL_PROJECTS=iwyu \
-DLLVM_EXTERNAL_IWYU_SOURCE_DIR=../include-what-you-use
```
