# Find PkgConfig file and create alias
macro(
  GFX_PKGCONFIG_IMPORT
  namespace
  package
  version
)
  pkg_check_modules(
    ${package}
    REQUIRED
    IMPORTED_TARGET
    ${package}>${version}
  )

  add_library(${namespace}::${package} ALIAS PkgConfig::${package})

endmacro()
