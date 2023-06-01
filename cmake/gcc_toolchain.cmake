set(CMAKE_CXX_COMPILER g++)
set(CMAKE_C_COMPILER gcc)

string(APPEND GFX_GCC_FLAGS " -Werror" # " -O3"
)

string(
  APPEND
  GFX_GCC_WARNINGS
  " -Wall"
  " -Wextra"
  " -Wshadow"
  " -Wpedantic"
  " -Wunused"
  " -Wunused-macros"
  " -Wmissing-declarations"
  " -Wredundant-decls"
  " -Wcast-align"
  " -Wconversion"
  " -Wsign-conversion"
  " -Wint-in-bool-context"
  " -Warith-conversion"
  " -Wdouble-promotion"
  " -Wfloat-equal"
  " -Wmisleading-indentation"
  " -Wduplicated-cond"
  " -Wduplicated-branches"
  " -Wlogical-op"
  " -Wnull-dereference"
  " -Walloc-zero"
  " -Wwrite-strings"
  " -Wformat=2"
  " -Wundef"
  " -Wsuggest-attribute=cold"
  " -Wsuggest-attribute=malloc"
  " -Wsuggest-attribute=noreturn"
  " -Wsuggest-final-types"
  " -Wsuggest-final-methods"
  " -Wbidi-chars=any,ucn"
  " -Warray-compare"
  " -Wcast-qual")

string(
  APPEND
  GFX_CXX_WARNING_FLAGS
  " -Wnon-virtual-dtor"
  " -Wold-style-cast"
  " -Woverloaded-virtual"
  " -Wuseless-cast"
  " -Weffc++"
  " -Wvexing-parse"
  " -Wsuggest-override"
  " -Wc++20-compat"
  " -Wterminate"
  " -Wsubobject-linkage"
  " -Wrange-loop-construct"
  " -Wpessimizing-move"
  " -Wextra-semi"
  " -Wconversion-null"
  " -Wredundant-move"
  " -Wredundant-tags"
  " -Wreorder"
  " -Wsign-promo"
  " -Wuninitialized")

string(APPEND GFX_C_WARNING_FLAGS
       " -Wno-declaration-after-statement" # C90 compatibility
       " -Wmissing-prototypes" " -Wc++-compat")

string(APPEND CMAKE_CXX_FLAGS ${GFX_GCC_FLAGS} ${GFX_GCC_WARNINGS}
       ${GFX_CXX_WARNING_FLAGS})

string(APPEND CMAKE_C_FLAGS ${GFX_GCC_FLAGS} ${GFX_GCC_WARNINGS}
       ${GFX_C_WARNING_FLAGS})
