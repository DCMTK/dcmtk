# Finish the setup. This should be run after all modules.

IF(BUILD_SINGLE_SHARED_LIBRARY)
  # Collect the object files of all libraries in all modules.
  # The variable ${DCMTK_ALL_LIBRARIES} is set by DCMTK_ADD_LIBRARY().
  SET(LIBS)
  FOREACH(LIB ${DCMTK_ALL_LIBRARIES})
    SET(LIBS ${LIBS} $<TARGET_OBJECTS:${LIB}>)
  ENDFOREACH(LIB)

  # Build a single "everything-library".
  # The library dependencies are collected by DCMTK_TARGET_LINK_LIBRARIES().
  ADD_LIBRARY(dcmtk SHARED ${LIBS})
  TARGET_LINK_LIBRARIES(dcmtk ${DCMTK_LIBRARY_DEPENDENCIES})
  SET_TARGET_PROPERTIES(dcmtk PROPERTIES ${DCMTK_LIBRARY_PROPERTIES})

  # Export target for build tree
  SET_PROPERTY(GLOBAL APPEND PROPERTY DCMTK_LIBRARY_TARGETS dcmtk)

  # Declare installation files. Also export libs and executables to DCMTKTargets.cmake.
  INSTALL(TARGETS dcmtk
          EXPORT DCMTKTargets
          COMPONENT lib
          RUNTIME DESTINATION "${DCMTK_INSTALL_BINDIR}"
          LIBRARY DESTINATION "${DCMTK_INSTALL_LIBDIR}"
          ARCHIVE DESTINATION "${DCMTK_INSTALL_LIBDIR}")
ENDIF(BUILD_SINGLE_SHARED_LIBRARY)
