macro(gfx_pkgconfig_import namespace package version)
pkg_check_modules(${package} REQUIRED IMPORTED_TARGET ${package}>${version})
add_library(${namespace}::${package} ALIAS PkgConfig::${package})
endmacro()
