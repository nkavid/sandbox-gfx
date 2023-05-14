function append()
{
  source scripts/logging.sh
  VERBOSE=0

  _usage()
  {
    echo -e "[USAGE]: append [env] -d [dir] "
    echo -e "Prepends path to environment variable\n"
    echo -e "flags"
    echo -e "\t-h | --help: prints this info"
    echo -e "\t-v | --verbose: prints even more info, or less"
    echo -e "\t-d | --directory: appended to environment variable"
    echo -e "\nenvironment variable option"
    echo -e "\tpythonpath: append PYTHONPATH"
    echo -e "\tpath: update PATH"
  }

  DIRECTORY=""
  ENVIRONMENT_VARIABLE=""

  while true; do
    case "$1" in
      -h | --help ) _usage; return 0 ;;
      -v | --verbose ) VERBOSE=1; shift ;;
      -d | --directory ) [[ ! -d $2 ]] \
        && error "${ITALIC}$2${RESET} not a directory" \
        && exit 1 \
        || DIRECTORY=$(realpath "$2");
      shift 2 ;;
      pythonpath ) ENVIRONMENT_VARIABLE="pythonpath"; shift ;;
      path ) ENVIRONMENT_VARIABLE="path"; shift ;;
      * ) break ;;
    esac
  done

  verbose_info "DIRECTORY = ${ITALIC}${DIRECTORY}${RESET}"
  verbose_info "ENVIRONMENT_VARIABLE = ${BOLD}${ENVIRONMENT_VARIABLE}${RESET}"

  if [[ ${ENVIRONMENT_VARIABLE} = "pythonpath" ]]; then
      export PYTHONPATH=${DIRECTORY}:${PYTHONPATH}
  elif [[ ${ENVIRONMENT_VARIABLE} = "path" ]]; then
      export PATH=${DIRECTORY}:${PATH}
  fi
}
