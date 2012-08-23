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
# DCMTK_SETUP_EXECUTABLE - macro which adds the needed setup for an executable
# PROGRAM - name of the executable that we are called for
#
MACRO(DCMTK_SETUP_EXECUTABLE PROGRAM)
    # Make wildcard arguments work
    IF(WIN32 AND NOT MINGW)
        SET_TARGET_PROPERTIES(${PROGRAM} PROPERTIES LINK_FLAGS ${WIN32_STD_OBJECTS})
    ENDIF(WIN32 AND NOT MINGW)
ENDMACRO(DCMTK_SETUP_EXECUTABLE)

#
# Setup an library
#
# DCMTK_SETUP_LIBRARY - macro which adds the needed setup for a library
# LIBRARY - name of the library that we are called for
#
MACRO(DCMTK_SETUP_LIBRARY LIBRARY)
    # set proper version information for shared library
    IF(BUILD_SHARED_LIBS)
        SET_TARGET_PROPERTIES(${LIBRARY} PROPERTIES ${DCMTK_LIBRARY_PROPERTIES})
    ELSE(BUILD_SHARED_LIBS)
        # define the foo_EXPORTS-macro even when we are not building shared libs
        SET_TARGET_PROPERTIES(${LIBRARY} PROPERTIES COMPILE_DEFINITIONS "${LIBRARY}_EXPORTS")
    ENDIF(BUILD_SHARED_LIBS)
ENDMACRO(DCMTK_SETUP_LIBRARY)
