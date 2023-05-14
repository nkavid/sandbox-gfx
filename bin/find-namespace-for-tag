#!/bin/bash

source scripts/logging.sh
export VERBOSE=1

function check()
{
  TEST_IMPLEMENTATION=$1
  verbose_info "checking '${TEST_IMPLEMENTATION}'"

  TEST_CASES=$(grep -E "SCENARIO|TEST_CASE" "${TEST_IMPLEMENTATION}")
  [[ -z ${TEST_CASES} ]] && verbose_info "no catch2 macro" || true && return

  FIRST_INCLUDE_DECLARATION=$(grep -m 1 "#include " "${TEST_IMPLEMENTATION}")

  INCLUDED_FILE=$(echo "${FIRST_INCLUDE_DECLARATION}" | sed "s/.*include \"//g" | sed "s/hpp\"/hpp/g" || true)
  verbose_info "'${INCLUDED_FILE}' Discovered SUT include"

  NAMESPACES=$(find gfx/ -wholename "*${INCLUDED_FILE}" -exec grep namespace {} + \
    | sed "s/namespace//" \
    | sed "s/::/ /" || true)

  FILE_BASENAME=$(basename -s ".hpp" "${INCLUDED_FILE}")

  echo "${TEST_CASES}" | while read -r TEST_CASE; do
    verbose_info "Catch2 macro found: '${TEST_CASE}'"
    CATCH2_TAGS=$(echo "${TEST_CASE}" | grep -o "\[[A-Za-z0-9_]*\]")
    # shellcheck disable=SC2086 # "CATCH2_TAGS" space separated
    verbose_info "'${TEST_IMPLEMENTATION}' Discovered tags:" ${CATCH2_TAGS}

    if [[ -z "${CATCH2_TAGS}" ]]; then
      error "Found test case in '${TEST_IMPLEMENTATION}' without tags"
      grep -HnF "${TEST_CASE}" "${TEST_IMPLEMENTATION}"
    fi

    for namespace in ${NAMESPACES}; do
      if [[ -z "${CATCH2_TAGS##*"${namespace}"*}" ]]; then
        verbose_info "found namespace tag '${namespace}'!"
      else
        error "did not find namespace tag '${namespace}'"
        grep -HnF "${TEST_CASE}" "${TEST_IMPLEMENTATION}"
      fi
    done

    if [[ -z "${CATCH2_TAGS##*"${FILE_BASENAME}"*}" ]]; then
      verbose_info "found include basename '${FILE_BASENAME}'!"
    else
      error "did not find include basename '${FILE_BASENAME}'"
      grep -HnF "${TEST_CASE}" "${TEST_IMPLEMENTATION}"
    fi
  done && success "${TEST_IMPLEMENTATION}"
}

INPUT_FILES=""

usage()
{
  echo -e "[USAGE]: $(basename "$0") [option]\n"
  echo -e "\tChecks Catch2 test macros for project specific tags"
  echo -e "\tSCENARIO(<desc>,<tags>) and TEST_CASE(<desc>,<tags>)"
  echo -e "\tThe string <tags> shall contain:"
  echo -e "\t\t* Namespaces opened in SUT header (except detail?)"
  echo -e "\t\t* Include file basename (component name)"
  echo -e "\tThe tags are in the form [tag][some_other_tag]..."
  echo -e "\n[OPTIONS]"
  echo -e "\t-d <directory>\tExpecting path to a directory"
  echo -e "\t-f <file>\tExpecting path to a file"
  echo -e "\t-v\t\tVerbose (optional)"
}

while getopts "hf:vd:vv" flag; do
  case "${flag}" in
    h)
      usage
      exit 0
      ;;
    f)
      [[ ! -f "${OPTARG}" ]] && error "'${OPTARG}' is not a file" && exit 1
      INPUT_FILES="${OPTARG}"
      ;;
    d)
      [[ ! -d "${OPTARG}" ]] && error "'${OPTARG}' is not a directory" && exit 1
      INPUT_FILES="$(find "${OPTARG}" -name "*.cpp")"
      ;;
    v)
      VERBOSE=1
      ;;
    *)
      error "unrecognized flag: ${flag}"
      usage
      exit 1
  esac
done

if [[ -z "${INPUT_FILES}" ]]; then
    error "no input files"
    usage
fi

for INPUT_FILE in ${INPUT_FILES}; do
  check "${INPUT_FILE}"
done

success "" && exit 0

