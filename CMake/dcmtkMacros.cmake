#
# Register all tests for the current module
#
# DCMTK_ADD_TESTS - macro which registers all tests
# MODULE - name of the module that we are called for
#
MACRO(DCMTK_ADD_TESTS MODULE)
    IF(BUILD_APPS)
        IF(CMAKE_CROSSCOMPILING)
            IF(WIN32)
                WINE_COMMAND(${MODULE}_TEST_CMD "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${MODULE}_tests")
                STRING(REPLACE "\\" "\\\\" ${MODULE}_TEST_CMD "${${MODULE}_TEST_CMD}")
            ELSEIF(ANDROID)
                SET(${MODULE}_TEST_CMD -P ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/dcmtkCTestRunAndroid.cmake)
            ELSE()
                IF(NOT DEFINED DCMTK_UNIT_TESTS_UNSUPPORTED_WARN_ONCE)
                    SET(DCMTK_UNIT_TESTS_UNSUPPORTED_WARN_ONCE CACHE INTERNAL "")
                    MESSAGE(WARNING "Emulation for your target platform is not available, unit tests will fail!")
                ENDIF()
                SET(${MODULE}_TEST_CMD "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${MODULE}_tests")
            ENDIF()
        ELSE(CMAKE_CROSSCOMPILING)
            SET(${MODULE}_TEST_CMD "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${MODULE}_tests")
        ENDIF(CMAKE_CROSSCOMPILING)
        LIST(APPEND DCMTK_TEST_EXECUTABLES "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${MODULE}_tests")
        LIST(REMOVE_DUPLICATES DCMTK_TEST_EXECUTABLES)
        SET(DCMTK_TEST_EXECUTABLES ${DCMTK_TEST_EXECUTABLES} CACHE INTERNAL "List of executables needed to run the unit tests")
        FILE(STRINGS tests.cc AVAIL_TESTS REGEX "OFTEST_REGISTER\\([^)]*\\)")
        FOREACH(TEST_LINE ${AVAIL_TESTS})
            # TODO: How can we parse tests.cc in a saner way?
            STRING(REGEX MATCH "OFTEST_REGISTER\\([^)]*" TEST "${TEST_LINE}")
            STRING(REPLACE "OFTEST_REGISTER(" "" TEST ${TEST})
            # This assumes that test names are globally unique
            IF(CMAKE_CROSSCOMPILING AND ANDROID)
                ADD_TEST("${TEST}" ${CMAKE_COMMAND} "-DDCMTK_CTEST_TESTCASE_COMMAND=${ANDROID_TEMPORARY_FILES_LOCATION}/${MODULE}_tests\;${TEST}" ${${MODULE}_TEST_CMD})
            ELSE()
                ADD_TEST("${TEST}" ${${MODULE}_TEST_CMD} "${TEST}")
            ENDIF()
            SET_PROPERTY(TEST "${TEST}" PROPERTY LABELS "${MODULE}")
        ENDFOREACH(TEST_LINE)
    ENDIF(BUILD_APPS)
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

        # Collect executable as part of global DCMTK_EXECUTABLE_TARGETS property
        SET_PROPERTY(GLOBAL APPEND PROPERTY DCMTK_EXECUTABLE_TARGETS ${PROGRAM})

        # declare installation files, also export DCMTKTargets.cmake
        INSTALL(TARGETS ${PROGRAM}
                EXPORT DCMTKTargets
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
        # Remember export target for writing it to build tree later
        SET_PROPERTY(GLOBAL APPEND PROPERTY DCMTK_LIBRARY_TARGETS ${LIBRARY}${DCMTK_LIBRARY_SUFFIX})

        # Declare installation files
        INSTALL(TARGETS ${LIBRARY}${DCMTK_LIBRARY_SUFFIX}
          EXPORT DCMTKTargets
          RUNTIME DESTINATION ${DCMTK_INSTALL_BINDIR} COMPONENT bin
          LIBRARY DESTINATION ${DCMTK_INSTALL_LIBDIR} COMPONENT shlib
          ARCHIVE DESTINATION ${DCMTK_INSTALL_LIBDIR} COMPONENT lib)
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

# Failsafe implementation of UNSET for old CMake versions
IF(CMAKE_VERSION VERSION_LESS 2.6.3)
  MACRO(DCMTK_UNSET VAR)
    SET(${VAR})
  ENDMACRO(DCMTK_UNSET)
ELSE(CMAKE_VERSION VERSION_LESS 2.6.3)
  MACRO(DCMTK_UNSET VAR)
    UNSET(${VAR})
  ENDMACRO(DCMTK_UNSET)
ENDIF(CMAKE_VERSION VERSION_LESS 2.6.3)

# Failsafe implementation of UNSET(... CACHE) for old CMake versions
IF(CMAKE_VERSION VERSION_LESS 2.6.3)
  MACRO(DCMTK_UNSET_CACHE VAR)
    SET(${VAR} CACHE INTERNAL "")
  ENDMACRO(DCMTK_UNSET_CACHE)
ELSE(CMAKE_VERSION VERSION_LESS 2.6.3)
  MACRO(DCMTK_UNSET_CACHE VAR)
    UNSET(${VAR} CACHE)
  ENDMACRO(DCMTK_UNSET_CACHE)
ENDIF(CMAKE_VERSION VERSION_LESS 2.6.3)

# Failsafe implementation of UNSET(... PARENT_SCOPE) for old CMake versions
IF(CMAKE_VERSION VERSION_LESS 3.0)
  MACRO(DCMTK_UNSET_PARENT_SCOPE VAR)
    SET(${VAR} PARENT_SCOPE)
  ENDMACRO(DCMTK_UNSET_PARENT_SCOPE)
ELSE(CMAKE_VERSION VERSION_LESS 3.0)
  MACRO(DCMTK_UNSET_PARENT_SCOPE VAR)
    UNSET(${VAR} PARENT_SCOPE)
  ENDMACRO(DCMTK_UNSET_PARENT_SCOPE)
ENDIF(CMAKE_VERSION VERSION_LESS 3.0)

# C style atexit for CMake
FUNCTION(DCMTK_ATEXIT_DISPATCHER)
    # this will be called at every scope change, but CMAKE_PARENT_LIST_FILE will only
    # be empty at exit.
    IF(NOT CMAKE_PARENT_LIST_FILE)
        FILE(WRITE ${CMAKE_BINARY_DIR}/CMakeTmp/atexit.cmake "${DCMTK_ATEXIT_HOOK}")
        DCMTK_UNSET_CACHE(DCMTK_ATEXIT_HOOK)
        INCLUDE(${CMAKE_BINARY_DIR}/CMakeTmp/atexit.cmake NO_POLICY_SCOPE)
        FILE(REMOVE ${CMAKE_BINARY_DIR}/CMakeTmp/atexit.cmake)
    ENDIF()
ENDFUNCTION(DCMTK_ATEXIT_DISPATCHER)

# Necessary since it's sometimes not correctly cleaned up
DCMTK_UNSET_CACHE(DCMTK_ATEXIT_HOOK)

MACRO(DCMTK_ATEXIT FUNCTION)
    IF(NOT DCMTK_ATEXIT_HOOK)
        VARIABLE_WATCH(CMAKE_PARENT_LIST_FILE DCMTK_ATEXIT_DISPATCHER)
    ENDIF()
    SET(DCMTK_ATEXIT_HOOK "${FUNCTION}(${ARGN})" CACHE INTERNAL "")
ENDMACRO(DCMTK_ATEXIT)
