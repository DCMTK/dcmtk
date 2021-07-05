# Finish the setup. This should be run after all modules.

if(BUILD_SINGLE_SHARED_LIBRARY)
  # Collect the object files of all libraries in all modules.
  # The variable ${DCMTK_ALL_LIBRARIES} is set by DCMTK_ADD_LIBRARY().
  set(LIBS)
  foreach(LIB ${DCMTK_ALL_LIBRARIES})
    set(LIBS ${LIBS} $<TARGET_OBJECTS:${LIB}>)
  endforeach()

  # Build a single "everything-library".
  # The library dependencies are collected by DCMTK_TARGET_LINK_LIBRARIES().
  add_library(dcmtk SHARED ${LIBS})
  target_link_libraries(dcmtk ${DCMTK_LIBRARY_DEPENDENCIES})
  set_target_properties(dcmtk PROPERTIES ${DCMTK_LIBRARY_PROPERTIES})

  # Export target for build tree
  set_property(GLOBAL APPEND PROPERTY DCMTK_LIBRARY_TARGETS dcmtk)
  target_link_libraries(DCMTK INTERFACE dcmtk)

  # Declare installation files. Also export libs and executables to DCMTKTargets.cmake.
  install(TARGETS dcmtk
          EXPORT DCMTKTargets
          COMPONENT lib
          RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
          LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
endif()
