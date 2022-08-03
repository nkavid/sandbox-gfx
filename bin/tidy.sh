#!/bin/bash

source scripts/logging.sh

CLANG_TIDY="../llvm-project/build/bin/clang-tidy"
CLANG_TIDY_OPTIONS=(-p build \
--extra-arg=-Wno-error=unknown-warning-option \
--extra-arg=-Weverything \
--extra-arg=-Wno-c++98-compat \
--extra-arg=-Wno-c++98-compat-pedantic \
--extra-arg=-Wno-switch-enum \
--extra-arg=-Wno-padded \
--extra-arg=-isystem/usr/lib/llvm-15/include/c++/v1/ \
--checks=-gfx-class-cohesion \
--quiet \
--use-color)

function tidy()
{
  [[ ! -d $1 ]] && error "did not find directory '$1'..." && exit 1
  verbose_info "run clang-tidy for headers in '$1'"

  HEADERS=$(find "$1" \( -name "*.h" -o -name "*.hpp" \))
  NUM_HEADERS=$(echo "${HEADERS}" | wc -l)
  PROCESSED=0

  for HEADER in ${HEADERS}; do
    INCR="${BOLD}[$((++PROCESSED)) of ${NUM_HEADERS}]${RESET}"

    [[ ${HEADER} =~ /shaders/ ]] \
      && verbose_info "skip ${ITALIC}${HEADER}${RESET} ${INCR}" \
      && continue

    verbose_info "checking ${ITALIC}${HEADER}${RESET} ${INCR}"
    ${CLANG_TIDY} "${CLANG_TIDY_OPTIONS[@]}" "${HEADER}"
  done
}

tidy "$1"
