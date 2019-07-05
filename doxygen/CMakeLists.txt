# declare project
project(doxygen)

# doxygen support
if(DCMTK_WITH_DOXYGEN)

  if(DOXYGEN_EXECUTABLE)
    add_custom_target(DOXYGEN)
    add_custom_target(html "${DOXYGEN_EXECUTABLE}" htmldocs.tmp)
    add_dependencies(DOXYGEN html)
    if(NOT WIN32) # For Unix systems, also build manpages
      add_custom_target(man COMMAND "${DOXYGEN_EXECUTABLE}" manpages.tmp
                            COMMAND rm -f "${CMAKE_CURRENT_BINARY_DIR}/manpages/man1/*.man.1"
                            COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/patchman.sh")
      add_dependencies(DOXYGEN man)
      add_custom_target(man2text COMMAND "${CMAKE_COMMAND}" -E make_directory man2text
                                 COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/man2text.sh")
      add_dependencies(man2text man)
    endif()
  endif()

  # generate a tag file in order to link to this documentation from external projects
  if(DCMTK_GENERATE_DOXYGEN_TAGFILE)
    set(DOXYGEN_TAGFILE "dcmtk.tag")
  endif()

  # create configuration files (expand variables)
  set(DCMTK_VERSION "${DCMTK_PACKAGE_VERSION}${DCMTK_PACKAGE_VERSION_SUFFIX}")
  set(DOXYGEN_INPUT_DIR "${CMAKE_SOURCE_DIR}")
  set(DOXYGEN_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/htmldocs.cfg" "${CMAKE_CURRENT_BINARY_DIR}/htmldocs.tmp" ESCAPE_QUOTES @ONLY)
  if(NOT WIN32)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/manpages.cfg" "${CMAKE_CURRENT_BINARY_DIR}/manpages.tmp" ESCAPE_QUOTES @ONLY)
  endif()

  # Make sure these directories exist when we try to install them
  install(CODE "file(MAKE_DIRECTORY \"${CMAKE_CURRENT_BINARY_DIR}/htmldocs\")" COMPONENT html)
  if(NOT WIN32)
    install(CODE "file(MAKE_DIRECTORY \"${CMAKE_CURRENT_BINARY_DIR}/manpages\")" COMPONENT man)
  endif()

  # install html docs and manpages
  install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/htmldocs/" DESTINATION "${CMAKE_INSTALL_DOCDIR}/html" COMPONENT html PATTERN "*.md5" EXCLUDE)
  if(DCMTK_GENERATE_DOXYGEN_TAGFILE)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${DOXYGEN_TAGFILE}" DESTINATION "${CMAKE_INSTALL_DOCDIR}" COMPONENT html OPTIONAL)
  endif()
  if(NOT WIN32)
    file(GLOB_RECURSE MANPAGES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/manpages/" "${CMAKE_CURRENT_SOURCE_DIR}/manpages/*.1")
    foreach(MANPAGE ${MANPAGES})
        if("${CMAKE_CURRENT_SOURCE_DIR}/manpages/${MANPAGE}" IS_NEWER_THAN "${CMAKE_CURRENT_BINARY_DIR}/manpages/${MANPAGE}")
            get_filename_component(DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/manpages/${MANPAGE}" PATH)
            file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/manpages/${MANPAGE}" DESTINATION "${DESTINATION}")
        endif()
    endforeach()
    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/manpages/" DESTINATION "${CMAKE_INSTALL_MANDIR}" COMPONENT man PATTERN "*_.1" EXCLUDE)
  endif()

  # the files in manpages/ and htmldocs/ should be removed by "make clean".
  set(make_clean_files "${CMAKE_CURRENT_BINARY_DIR}/htmldocs" "${CMAKE_CURRENT_BINARY_DIR}/manpages" "${CMAKE_CURRENT_BINARY_DIR}/man2text")
  set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${make_clean_files}")

  # the created files "htmldocs.tmp" and "manpages.tmp" should be removed with "make distclean",
  # which will (hopefully) be introduced in a future version, because they are only created during
  # the configure/generate process

else()

  # by default, install the pre-defined manpages, i.e. the ones shipped with this package
  if(NOT WIN32)
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/manpages/" DESTINATION "${CMAKE_INSTALL_MANDIR}" COMPONENT man)
  endif()

endif()
