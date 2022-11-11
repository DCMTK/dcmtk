#
# Register all tests for the current module
#
# DCMTK_ADD_TESTS - function which registers all tests for a module
# MODULE - name of the module that we are called for
#
function(DCMTK_ADD_TESTS MODULE)
    if(BUILD_APPS AND DCMTK_RUN_CTEST_SCRIPT)
        set(TEST_EXECUTABLE "$<TARGET_FILE:${MODULE}_tests>")
        if(CMAKE_CROSSCOMPILING)
            if(WIN32)
                WINE_COMMAND(TEST_COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${MODULE}_tests${CMAKE_EXECUTABLE_SUFFIX}")
                string(REPLACE "\\" "\\\\" TEST_COMMAND "${TEST_COMMAND}")
            elseif(ANDROID)
                set(TEST_COMMAND "${ANDROID_TEMPORARY_FILES_LOCATION}/${MODULE}_tests")
            else()
                # not reachable, handled by not defining DCMTK_RUN_CTEST_SCRIPT
            endif()
        else()
            set(TEST_COMMAND "${TEST_EXECUTABLE}")
        endif()
        list(APPEND DCMTK_TEST_EXECUTABLES "${TEST_EXECUTABLE}")
        list(REMOVE_DUPLICATES DCMTK_TEST_EXECUTABLES)
        set(DCMTK_TEST_EXECUTABLES ${DCMTK_TEST_EXECUTABLES} CACHE INTERNAL "List of executables needed to run the unit tests")
        file(STRINGS tests.cc AVAIL_TESTS REGEX "OFTEST_REGISTER\\([^)]*\\)")
        foreach(TEST_LINE ${AVAIL_TESTS})
            # TODO: How can we parse tests.cc in a saner way?
            string(REGEX MATCH "OFTEST_REGISTER\\([^)]*" TEST "${TEST_LINE}")
            string(REPLACE "OFTEST_REGISTER(" "" TEST ${TEST})
            # This assumes that test names are globally unique
            add_test(NAME "${TEST}" COMMAND "${CMAKE_COMMAND}" "-DDCMTK_CTEST_TESTCASE_COMMAND=${TEST_COMMAND}" "-DDCMTK_CTEST_TEST_NAME=${TEST}" "-P" "${DCMTK_RUN_CTEST_SCRIPT}")
            set_property(TEST "${TEST}" PROPERTY LABELS "${MODULE}")
            set_property(TEST "${TEST}" PROPERTY REQUIRED_FILES "${TEST_EXECUTABLE}")
        endforeach()
        add_custom_target("${MODULE}-test-exhaustive"
            COMMAND "${CMAKE_COMMAND}" "-DCONFIG=${DCMTK_CONFIG_GENERATOR_EXPRESSION}" "-P" "${DCMTK_SOURCE_DIR}/CMake/CTest/dcmtkCTestRunExhaustive.cmake"
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        )
    endif()
endfunction()

#
# Setup an executable
#
# DCMTK_ADD_EXECUTABLE - macro which adds the needed setup for an executable
# PROGRAM - name of the executable that we are called for
#
macro(DCMTK_ADD_EXECUTABLE PROGRAM)
    if(BUILD_APPS)
        add_executable(${PROGRAM} ${ARGN})

        # Make wildcard arguments work
        if(WIN32 AND NOT MINGW)
            set_target_properties(${PROGRAM} PROPERTIES LINK_FLAGS ${WIN32_STD_OBJECTS})
        endif()

        # Collect executable as part of global DCMTK_EXECUTABLE_TARGETS property
        set_property(GLOBAL APPEND PROPERTY DCMTK_EXECUTABLE_TARGETS ${PROGRAM})

        # declare installation files, also export DCMTKTargets.cmake
        install(TARGETS ${PROGRAM}
                EXPORT DCMTKTargets
                COMPONENT bin
                DESTINATION ${CMAKE_INSTALL_BINDIR})
    endif()
endmacro()

#
# Setup a library
#
# DCMTK_ADD_LIBRARY - macro which adds the needed setup for a library
# LIBRARY - name of the library that we are called for
# extra arguments - names of the library's source files
#
macro(DCMTK_ADD_LIBRARY LIBRARY)
    # Actually add the library first
    add_library(${LIBRARY}${DCMTK_LIBRARY_SUFFIX} ${DCMTK_LIBRARY_TYPE} ${ARGN})
    set(DCMTK_ALL_LIBRARIES ${DCMTK_ALL_LIBRARIES} ${LIBRARY}${DCMTK_LIBRARY_SUFFIX} CACHE INTERNAL "List of all libraries in the DCMTK.")

    # set proper version information for shared library
    if(BUILD_SHARED_LIBS AND NOT BUILD_SINGLE_SHARED_LIBRARY)
        set_target_properties(${LIBRARY}${DCMTK_LIBRARY_SUFFIX} PROPERTIES ${DCMTK_LIBRARY_PROPERTIES})
    else()
        # define the foo_EXPORTS-macro even when we are not building ("normal") shared libs
        set_target_properties(${LIBRARY}${DCMTK_LIBRARY_SUFFIX} PROPERTIES COMPILE_DEFINITIONS "${LIBRARY}_EXPORTS")
    endif()

    get_filename_component(DCMTK_ADD_LIBRARY_PARENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" PATH)
    target_include_directories(${LIBRARY}${DCMTK_LIBRARY_SUFFIX} PUBLIC
      $<BUILD_INTERFACE:${DCMTK_ADD_LIBRARY_PARENT_DIR}/include>
      $<INSTALL_INTERFACE:include>
    )

    if(NOT BUILD_SINGLE_SHARED_LIBRARY)
        # Remember export target for writing it to build tree later
        set_property(GLOBAL APPEND PROPERTY DCMTK_LIBRARY_TARGETS ${LIBRARY}${DCMTK_LIBRARY_SUFFIX})
        target_link_libraries(DCMTK INTERFACE ${LIBRARY}${DCMTK_LIBRARY_SUFFIX})

        # Declare installation files
        install(TARGETS ${LIBRARY}${DCMTK_LIBRARY_SUFFIX}
          EXPORT DCMTKTargets
          RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT bin
          LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib
          ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT lib
        )
    endif()
endmacro()

#
# Specify a library's dependencies
#
# DCMTK_TARGET_LINK_LIBRARIES - macro for specifying a library's dependencies
# LIBRARY - name of the library that we are called for
# extra arguments - names of the library's dependencies
#
macro(DCMTK_TARGET_LINK_LIBRARIES LIBRARY)
    set(DCMTK_LIBRARY_DEPENDENCIES ${DCMTK_LIBRARY_DEPENDENCIES} ${ARGN} CACHE INTERNAL "Dependencies of the DCMTK libraries.")
    # The name of this macro doesn't match, but it does just what we need here
    DCMTK_TARGET_LINK_MODULES(${LIBRARY} ${ARGN})
endmacro()

#
# Link a target against other DCMTK modules
#
# DCMTK_TARGET_LINK_MODULES - macro which adds DCMTK modules to a target's dependencies
# TARGET - name of the target that we are called for
# extra arguments - names of the modules that should be added
#
macro(DCMTK_TARGET_LINK_MODULES TARGET)
    # This macro is called for libraries and apps. Thus, we need some magic.
    # 1. During "normal builds", always call target_link_libraries()
    # 2. When BUILD_SINGLE_SHARED_LIBRARY, never call it, because neither the
    #    libraries nor the apps really exist in this mode
    # 3. When "just" BUILD_APPS is disabled, only call it for libs, not for apps
    # These rules boil down to: If CMake knows the target (this handles all
    # cases for apps) and we aren't building a single library (case 2)
    if(TARGET ${TARGET} AND NOT BUILD_SINGLE_SHARED_LIBRARY)
        target_link_libraries(${TARGET} ${ARGN})
    endif()
endmacro()

# This is an ugly hack to simulate global variables
set(DCMTK_ALL_LIBRARIES CACHE INTERNAL "List of all libraries in the DCMTK.")
set(DCMTK_LIBRARY_DEPENDENCIES CACHE INTERNAL "Dependencies of the DCMTK libraries.")

# implementation of UNSET
macro(DCMTK_UNSET VAR)
  unset(${VAR})
endmacro()

# implementation of unset(... CACHE)
macro(DCMTK_UNSET_CACHE VAR)
  unset(${VAR} CACHE)
endmacro()

# implementation of unset(... PARENT_SCOPE)
macro(DCMTK_UNSET_PARENT_SCOPE VAR)
  unset(${VAR} PARENT_SCOPE)
endmacro()

# A C++ STL style upper_bound function for CMake ';' lists
function(DCMTK_UPPER_BOUND LIST COMP VAR)
  list(LENGTH "${LIST}" N)
  math(EXPR LAST "${N}-1")
  foreach(INDEX RANGE ${LAST})
    list(GET "${LIST}" ${INDEX} E)
    if(COMP LESS E)
      set("${VAR}" ${INDEX} PARENT_SCOPE)
      return()
    endif()
  endforeach()
  set("${VAR}" ${N} PARENT_SCOPE)
endfunction()

function(DCMTK_CREATE_INSTALL_EXPORTS)
    # DCMTKTargets.cmake will contain list of executables and libraries produced
    # DCMTKConfigVersion.cmake will contain DCMTK version information
    # DCMTKConfig.cmake will contain options used to build DCMTK
    #
    # All three files are created within the build tree's main directory (handled in
    # CMake/GenerateCMakeExports.cmake, and are installed to locations (OS-specific
    # under the main install dir (handled directly below).

    # Only create fully-fledged CMake export files if we have the related commands
    include("${DCMTK_MACROS_DIR}/CheckCMakeCommandExists.cmake")
    include(CMakePackageConfigHelpers OPTIONAL)
    CHECK_CMAKE_COMMAND_EXISTS("CONFIGURE_PACKAGE_CONFIG_FILE")
    CHECK_CMAKE_COMMAND_EXISTS("WRITE_BASIC_PACKAGE_VERSION_FILE")

    if(HAVE_CONFIGURE_PACKAGE_CONFIG_FILE AND HAVE_WRITE_BASIC_PACKAGE_VERSION_FILE)

      # Create and configure CMake export files
      include("${DCMTK_MACROS_DIR}/GenerateCMakeExports.cmake")

      # ${DCMTK_INSTALL_CONFIG} and ${DCMTK_CONFIG_VERSION} are
      # defined within CMake/GenerateCMakeExports.cmake.
      # Install DCMTKTargets.cmake to install tree
      install(EXPORT DCMTKTargets FILE DCMTKTargets.cmake NAMESPACE DCMTK::
              DESTINATION "${DCMTK_INSTALL_CMKDIR}" COMPONENT cmake)

      # Install DCMTKConfig.cmake and DCMTKConfigVersion.cmake
      install(FILES "${DCMTK_INSTALL_CONFIG}" "${DCMTK_CONFIG_VERSION}"
              DESTINATION "${DCMTK_INSTALL_CMKDIR}" COMPONENT cmake)

    else()

      # Warning that we use old "configure_file" command
      message(STATUS "Warning: Using old configure_file() mechanism to produce DCMTKConfig.cmake")

      # Actually configure file
      configure_file("${DCMTK_MACROS_DIR}/DCMTKConfig.old_cmake.in"
                     "${DCMTK_BINARY_DIR}/DCMTKConfig.cmake" @ONLY)

      # Install DCMTKConfig.cmake and DCMTKConfigVersion.cmake
      install(FILES "${DCMTK_BINARY_DIR}/DCMTKConfig.cmake" "${DCMTK_BINARY_DIR}/DCMTKConfigVersion.cmake"
              DESTINATION "${DCMTK_INSTALL_CMKDIR}"
              COMPONENT cmake)

    endif()
endfunction()

set(DCMTK_MACROS_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE INTERNAL "")
