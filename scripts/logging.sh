BEGIN="\x1B["
END="m"
SEPARATOR=";"

BOLD="\x1B[1m"
ITALIC="\x1B[3m"

RED="\x1B[31m"
GREEN="\x1B[32m"
YELLOW="\x1B[33m"
BLUE="\x1B[34m"
MAGENTA="\x1B[35m"
CYAN="\x1B[36m"

LIGHT_RED="\x1B[91m"
LIGHT_GREEN="\x1B[92m"
LIGHT_YELLOW="\x1B[93m"
LIGHT_BLUE="\x1B[94m"
LIGHT_MAGENTA="\x1B[95m"
LIGHT_CYAN="\x1B[96m"

LIGHT_GRAY="\x1B[37m"
GRAY="\x1B[90m"
BLACK="\x1B[30m"
WHITE="\x1B[97m"

DEFAULT="\x1B[39m"
RESET="\x1B[0m"

DEBUG="\x1B[1;30;105m"
DEV_MSG="\x1B[1;95m"

BASENAME=$(basename $0)
VERBOSE=1

function error()
{
  echo -e "${BOLD}gfx::[${RED}error${DEFAULT}]::${BASENAME}:${RESET} $1"
}

function warning()
{
  echo -e "${BOLD}gfx::[${YELLOW}warning${DEFAULT}]::${BASENAME}:${RESET} $1"
}

function success()
{
  echo -e "${BOLD}gfx::[${GREEN}success${DEFAULT}]::${BASENAME}:${RESET} $1"
}

function verbose_info()
{
  [[ ${VERBOSE} = 1 ]] \
    && echo -e "${BOLD}gfx::[${LIGHT_GRAY}info${DEFAULT}]::${BASENAME}:${RESET} $1" \
    || true
}

function debug()
{
  echo -e "${DEBUG}::DEBUG::${RESET}\t${DEV_MSG}$1${RESET}"
}
