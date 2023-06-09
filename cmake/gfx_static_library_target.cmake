# Create GFX library target
function(gfx_static_library_target)
  set(options)
  set(one_value_args TARGET NAMESPACE INTERFACE_HEADERS)
  set(multi_value_args SOURCES DIRECTORIES DEPENDENCIES)
  cmake_parse_arguments(
    GFX_LIBRARY
    "${options}"
    "${one_value_args}"
    "${multi_value_args}"
    ${ARGN}
  )

  set(target_name ${GFX_LIBRARY_NAMESPACE}_${GFX_LIBRARY_TARGET})

  add_library(${target_name} STATIC)

  target_sources(${target_name} PRIVATE ${GFX_LIBRARY_SOURCES})

  target_include_directories(${target_name} PRIVATE ${GFX_LIBRARY_DIRECTORIES})

  target_link_libraries(${target_name} ${GFX_LIBRARY_DEPENDENCIES})

  add_library(
    ${GFX_LIBRARY_NAMESPACE}::${GFX_LIBRARY_TARGET} ALIAS ${target_name}
  )

  install(TARGETS ${target_name} EXPORT gfxTargets)

  install(
    FILES ${GFX_LIBRARY_INTERFACE_HEADERS}
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/gfx/${GFX_LIBRARY_NAMESPACE}"
  )
endfunction()
