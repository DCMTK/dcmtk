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
    IF(BUILD_APPS)
        ADD_EXECUTABLE(${PROGRAM} ${ARGN})

        # Make wildcard arguments work
        IF(WIN32 AND NOT MINGW)
            SET_TARGET_PROPERTIES(${PROGRAM} PROPERTIES LINK_FLAGS ${WIN32_STD_OBJECTS})
        ENDIF(WIN32 AND NOT MINGW)

        # declare installation files
        INSTALL(TARGETS ${PROGRAM}
                COMPONENT bin
                DESTINATION ${DCMTK_INSTALL_BINDIR})
    ENDIF(BUILD_APPS)
ENDMACRO(DCMTK_ADD_EXECUTABLE)

#
# Setup a library
#
# DCMTK_ADD_LIBRARY - macro which adds the needed setup for a library
# LIBRARY - name of the library that we are called for
# extra arguments - names of the library's source files
#
MACRO(DCMTK_ADD_LIBRARY LIBRARY)
    # Actually add the library first
    ADD_LIBRARY(${LIBRARY}${DCMTK_LIBRARY_SUFFIX} ${DCMTK_LIBRARY_TYPE} ${ARGN})
    SET(DCMTK_ALL_LIBRARIES ${DCMTK_ALL_LIBRARIES} ${LIBRARY}${DCMTK_LIBRARY_SUFFIX} CACHE INTERNAL "List of all libraries in the DCMTK.")

    # set proper version information for shared library
    IF(BUILD_SHARED_LIBS AND NOT BUILD_SINGLE_SHARED_LIBRARY)
        SET_TARGET_PROPERTIES(${LIBRARY}${DCMTK_LIBRARY_SUFFIX} PROPERTIES ${DCMTK_LIBRARY_PROPERTIES})
    ELSE(BUILD_SHARED_LIBS AND NOT BUILD_SINGLE_SHARED_LIBRARY)
        # define the foo_EXPORTS-macro even when we are not building ("normal") shared libs
        SET_TARGET_PROPERTIES(${LIBRARY}${DCMTK_LIBRARY_SUFFIX} PROPERTIES COMPILE_DEFINITIONS "${LIBRARY}_EXPORTS")
    ENDIF(BUILD_SHARED_LIBS AND NOT BUILD_SINGLE_SHARED_LIBRARY)

    IF(NOT BUILD_SINGLE_SHARED_LIBRARY)
        # Declare installation files
        INSTALL(TARGETS ${LIBRARY}${DCMTK_LIBRARY_SUFFIX}
                COMPONENT lib
                RUNTIME DESTINATION ${DCMTK_INSTALL_BINDIR}
                LIBRARY DESTINATION ${DCMTK_INSTALL_LIBDIR}
                ARCHIVE DESTINATION ${DCMTK_INSTALL_LIBDIR})
    ENDIF(NOT BUILD_SINGLE_SHARED_LIBRARY)
ENDMACRO(DCMTK_ADD_LIBRARY)

#
# Specify a library's dependencies
#
# DCMTK_TARGET_LINK_LIBRARIES - macro for specifying a library's dependencies
# LIBRARY - name of the library that we are called for
# extra arguments - names of the library's dependencies
#
MACRO(DCMTK_TARGET_LINK_LIBRARIES LIBRARY)
    SET(DCMTK_LIBRARY_DEPENDENCIES ${DCMTK_LIBRARY_DEPENDENCIES} ${ARGN} CACHE INTERNAL "Dependencies of the DCMTK libraries.")
    # The name of this macro doesn't match, but it does just what we need here
    DCMTK_TARGET_LINK_MODULES(${LIBRARY} ${ARGN})
ENDMACRO(DCMTK_TARGET_LINK_LIBRARIES)

#
# Link a target against other DCMTK modules
#
# DCMTK_TARGET_LINK_MODULES - macro which adds DCMTK modules to a target's dependencies
# TARGET - name of the target that we are called for
# extra arguments - names of the modules that should be added
#
MACRO(DCMTK_TARGET_LINK_MODULES TARGET)
    # This macro is called for libraries and apps. Thus, we need some magic.
    # 1. During "normal builds", always call TARGET_LINK_LIBRARIES()
    # 2. When BUILD_SINGLE_SHARED_LIBRARY, never call it, because neither the
    #    libraries nor the apps really exist in this mode
    # 3. When "just" BUILD_APPS is disabled, only call it for libs, not for apps
    # These rules boil down to: If CMake knows the target (this handles all
    # cases for apps) and we aren't building a single library (case 2)
    IF(TARGET ${TARGET} AND NOT BUILD_SINGLE_SHARED_LIBRARY)
        TARGET_LINK_LIBRARIES(${TARGET} ${ARGN})
    ENDIF(TARGET ${TARGET} AND NOT BUILD_SINGLE_SHARED_LIBRARY)
ENDMACRO(DCMTK_TARGET_LINK_MODULES TARGET)

# This is an ugly hack to simulate global variables
SET(DCMTK_ALL_LIBRARIES CACHE INTERNAL "List of all libraries in the DCMTK.")
SET(DCMTK_LIBRARY_DEPENDENCIES CACHE INTERNAL "Dependencies of the DCMTK libraries.")
