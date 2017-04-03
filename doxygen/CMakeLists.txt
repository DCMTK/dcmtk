# declare project
PROJECT(doxygen)

# doxygen support
IF(DCMTK_WITH_DOXYGEN)

  IF(DOXYGEN_EXECUTABLE)
    ADD_CUSTOM_TARGET(DOXYGEN)
    ADD_CUSTOM_TARGET(html "${DOXYGEN_EXECUTABLE}" htmldocs.tmp)
    ADD_DEPENDENCIES(DOXYGEN html)
    IF(NOT WIN32) # For Unix systems, also build manpages
      ADD_CUSTOM_TARGET(man COMMAND "${DOXYGEN_EXECUTABLE}" manpages.tmp
                            COMMAND rm -f "${CMAKE_CURRENT_BINARY_DIR}/manpages/man1/*.man.1"
                            COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/patchman.sh")
      ADD_DEPENDENCIES(DOXYGEN man)
      ADD_CUSTOM_TARGET(man2text COMMAND "${CMAKE_COMMAND}" -E make_directory man2text
                                 COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/man2text.sh")
      ADD_DEPENDENCIES(man2text man)
    ENDIF(NOT WIN32)
  ENDIF(DOXYGEN_EXECUTABLE)

  # generate a tag file in order to link to this documentation from external projects
  IF(DCMTK_GENERATE_DOXYGEN_TAGFILE)
    SET(DOXYGEN_TAGFILE "dcmtk.tag")
  ENDIF(DCMTK_GENERATE_DOXYGEN_TAGFILE)

  # create configuration files (expand variables)
  SET(DCMTK_VERSION "${DCMTK_PACKAGE_VERSION}${PACKAGE_VERSION_SUFFIX}")
  SET(DOXYGEN_INPUT_DIR "${CMAKE_SOURCE_DIR}")
  SET(DOXYGEN_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
  CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/htmldocs.cfg" "${CMAKE_CURRENT_BINARY_DIR}/htmldocs.tmp" ESCAPE_QUOTES @ONLY)
  IF(NOT WIN32)
    CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/manpages.cfg" "${CMAKE_CURRENT_BINARY_DIR}/manpages.tmp" ESCAPE_QUOTES @ONLY)
  ENDIF(NOT WIN32)

  # Make sure these directories exist when we try to install them
  INSTALL(CODE "FILE(MAKE_DIRECTORY \"${CMAKE_CURRENT_BINARY_DIR}/htmldocs\")" COMPONENT html)
  IF(NOT WIN32)
    INSTALL(CODE "FILE(MAKE_DIRECTORY \"${CMAKE_CURRENT_BINARY_DIR}/manpages\")" COMPONENT man)
  ENDIF(NOT WIN32)

  # install html docs and manpages
  INSTALL(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/htmldocs/" DESTINATION "${DCMTK_INSTALL_HTMDIR}" COMPONENT html PATTERN "*.md5" EXCLUDE)
  IF(DCMTK_GENERATE_DOXYGEN_TAGFILE)
    INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/${DOXYGEN_TAGFILE}" DESTINATION "${DCMTK_INSTALL_DOCDIR}" COMPONENT html OPTIONAL)
  ENDIF(DCMTK_GENERATE_DOXYGEN_TAGFILE)
  IF(NOT WIN32)
    FILE(GLOB_RECURSE MANPAGES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/manpages/" "${CMAKE_CURRENT_SOURCE_DIR}/manpages/*.1")
    FOREACH(MANPAGE ${MANPAGES})
        IF("${CMAKE_CURRENT_SOURCE_DIR}/manpages/${MANPAGE}" IS_NEWER_THAN "${CMAKE_CURRENT_BINARY_DIR}/manpages/${MANPAGE}")
            GET_FILENAME_COMPONENT(DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/manpages/${MANPAGE}" PATH)
            FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/manpages/${MANPAGE}" DESTINATION "${DESTINATION}")
        ENDIF()
    ENDFOREACH()
    INSTALL(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/manpages/" DESTINATION "${DCMTK_INSTALL_MANDIR}" COMPONENT man PATTERN "*_.1" EXCLUDE)
  ENDIF(NOT WIN32)

  # the files in manpages/ and htmldocs/ should be removed by "make clean".
  SET(make_clean_files "${CMAKE_CURRENT_BINARY_DIR}/htmldocs" "${CMAKE_CURRENT_BINARY_DIR}/manpages" "${CMAKE_CURRENT_BINARY_DIR}/man2text")
  SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${make_clean_files}")

  # the created files "htmldocs.tmp" and "manpages.tmp" should be removed with "make distclean",
  # which will (hopefully) be introduced in a future version, because they are only created during
  # the configure/generate process

ELSE(DCMTK_WITH_DOXYGEN)

  # by default, install the pre-defined manpages, i.e. the ones shipped with this package
  IF(NOT WIN32)
    INSTALL(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/manpages/" DESTINATION "${DCMTK_INSTALL_MANDIR}" COMPONENT man)
  ENDIF(NOT WIN32)

ENDIF(DCMTK_WITH_DOXYGEN)
