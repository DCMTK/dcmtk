#
# Register all tests for the current module
#
# DCMTK_ADD_TESTS - macro which registers all tests
# MODULE - name of the module that we are called for
#

MACRO(DCMTK_ADD_TESTS MODULE)
    FILE(STRINGS tests.cc AVAIL_TESTS REGEX "OFTEST_REGISTER\\([^)]*\\)")
    FOREACH(TEST_LINE ${AVAIL_TESTS})
        # TODO: How can we parse tests.cc in a saner way?
        STRING(REGEX MATCH "OFTEST_REGISTER\\([^)]*" TEST "${TEST_LINE}")
        STRING(REPLACE "OFTEST_REGISTER(" "" TEST ${TEST})
        # This assumes that test names are globally unique
        ADD_TEST("${TEST}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${MODULE}_tests" "${TEST}")
        SET_PROPERTY(TEST "${TEST}" PROPERTY LABELS "${MODULE}")
    ENDFOREACH(TEST_LINE)
ENDMACRO(DCMTK_ADD_TESTS)

#
# Setup an executable
#
# DCMTK_ADD_EXECUTABLE - macro which adds the needed setup for an executable
# PROGRAM - name of the executable that we are called for
#
MACRO(DCMTK_ADD_EXECUTABLE PROGRAM)
    ADD_EXECUTABLE(${PROGRAM} ${ARGN})

    # Make wildcard arguments work
    IF(WIN32 AND NOT MINGW)
        SET_TARGET_PROPERTIES(${PROGRAM} PROPERTIES LINK_FLAGS ${WIN32_STD_OBJECTS})
    ENDIF(WIN32 AND NOT MINGW)

    # declare installation files
    INSTALL(TARGETS ${PROGRAM}
            COMPONENT bin
            DESTINATION ${DCMTK_INSTALL_BINDIR})
ENDMACRO(DCMTK_ADD_EXECUTABLE)

#
# Setup an library
#
# DCMTK_ADD_LIBRARY - macro which adds the needed setup for a library
# LIBRARY - name of the library that we are called for
# extra arguments - names of the library's source files
#
MACRO(DCMTK_ADD_LIBRARY LIBRARY)
    # Actually add the library first
    ADD_LIBRARY(${LIBRARY} ${ARGN})

    # set proper version information for shared library
    IF(BUILD_SHARED_LIBS)
        SET_TARGET_PROPERTIES(${LIBRARY} PROPERTIES ${DCMTK_LIBRARY_PROPERTIES})
    ELSE(BUILD_SHARED_LIBS)
        # define the foo_EXPORTS-macro even when we are not building shared libs
        SET_TARGET_PROPERTIES(${LIBRARY} PROPERTIES COMPILE_DEFINITIONS "${LIBRARY}_EXPORTS")
    ENDIF(BUILD_SHARED_LIBS)

    # Declare installation files
    INSTALL(TARGETS ${LIBRARY}
            COMPONENT lib
            RUNTIME DESTINATION ${DCMTK_INSTALL_BINDIR}
            LIBRARY DESTINATION ${DCMTK_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${DCMTK_INSTALL_LIBDIR})
ENDMACRO(DCMTK_ADD_LIBRARY)
