#
# Functions for detection and usage of Wine
# Used when cross compiling
#

#
# Tries to detect the required executables and libraries automatically and emits an
# error that will halt the configuration process if that is not possible.
# All arguments will be ignored.
#
function(DCMTK_SETUP_WINE)
    find_program(WINE_WINE_PROGRAM wine)
    find_program(WINE_WINEPATH_PROGRAM winepath)
    if(NOT WINE_CPP_RUNTIME)
        set(v 1)
        unset(CMAKE_FIND_LIBRARY_SUFFIXES)
        while(${v} LESS 14)
            list(APPEND CMAKE_FIND_LIBRARY_SUFFIXES "-${v}.dll")
            math(EXPR v "${v} + 1")
        endwhile()
        find_library(CPP_RUNTIME stdc++ DOC "Location of the C++ runtime library used during target emulation")
        if(NOT CPP_RUNTIME)
            message(FATAL_ERROR "Please set the C++ runtime location (required for running applications targeting Windows).")
        else()
            get_filename_component(CPP_RUNTIME_LOCATION ${CPP_RUNTIME} PATH)
            UNIX_TO_WINE_PATH(RT ${CPP_RUNTIME_LOCATION})
            set(WINE_CPP_RUNTIME ${RT} CACHE INTERNAL "C++ RT in Wine path format")
        endif()
    endif()
    if(NOT WINE_WINE_PROGRAM OR NOT WINE_WINEPATH_PROGRAM)
        message(FATAL_ERROR
            "Some wine related executables could not be detected automatically. "
            "Please set WINE_WINE_PROGRAM and WINE_WINEPATH_PROGRAM appropriately."
        )
    endif()
    # prepare wine prefix for configure and unit tests
    if(NOT DCMTK_WINEPREFIX)
        set(DCMTK_WINEPREFIX "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/wineprefix" CACHE INTERNALE "the path of the wineprefix to use for configuration and unit tests")
        message(STATUS "Info: Preparing wine prefix for configuration and unit tests: ${DCMTK_WINEPREFIX}")
        string(REPLACE "\\" "\\\\" WINE_CPP_RUNTIME "${WINE_CPP_RUNTIME}")
        set(WINE_PATH_REG "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/dcmtkWinePath.reg")
        configure_file("${DCMTK_SOURCE_DIR}/CMake/dcmtkWinePath.reg.in" "${WINE_PATH_REG}" ESCAPE_QUOTES @ONLY)
        set(ENV{WINEPREFIX} "${DCMTK_WINEPREFIX}")
        execute_process(COMMAND "${WINE_WINE_PROGRAM}" "regedit" "${WINE_PATH_REG}"
            OUTPUT_FILE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/wineprefix.log"
            ERROR_FILE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/wineprefix.log"
            OUTPUT_VARIABLE RESULT
        )
        if(RESULT)
            set(DCMTK_WINEPREFIX CACHE INTERNAL "error, see ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/wineprefix.log")
            message(FATAL_ERROR "Failed to setup the wineprefix, see \"${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/wineprefix.log\"")
        endif()
    else()
        set(ENV{WINEPREFIX} "${DCMTK_WINEPREFIX}")
    endif()
endfunction()

#
# Helper function to detach the output and error streams from a wine process, so
# that CMake doesn't wait for the wineserver to quit before continuing execution.
# VAR - the variable that will hold the exit code of the launched process.
# OUTPUT_VAR - the variable that will hold the standard output of the launched process.
# ERROR_VAR - the variable that will hold the error output of the launched process.
# ARGN - the command to execute.
#
function(WINE_DETACHED VAR OUTPUT_VAR ERROR_VAR)
    # Prefix to prevent collision of output capturing files
    string(MD5 PREFIX "${ARGN}")
    set(OUTPUT_FILE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PREFIX}_output")
    if(OUTPUT_VAR STREQUAL ERROR_VAR)
        set(ERROR_FILE "${OUTPUT_FILE}")
    else()
        set(ERROR_FILE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PREFIX}_error")
    endif()
    execute_process(
        COMMAND ${ARGN}
        OUTPUT_FILE "${OUTPUT_FILE}"
        ERROR_FILE "${ERROR_FILE}"
        RESULT_VARIABLE RESULT
    )
    file(READ "${OUTPUT_FILE}" OUTPUT)
    set("${OUTPUT_VAR}" ${OUTPUT} PARENT_SCOPE)
    if(OUTPUT_VAR STREQUAL ERROR_VAR)
        file(REMOVE "${OUTPUT_FILE}")
    else()
        file(READ "${ERROR_FILE}" ERROR)
        set("${ERROR_VAR}" ${ERROR} PARENT_SCOPE)
        file(REMOVE "${OUTPUT_FILE}" "${ERROR_FILE}")
    endif()
    set("${VAR}" ${RESULT} PARENT_SCOPE)
endfunction()

#
# Uses 'winepath' to translate a host path or filename to the location it can be
# accessed from within the emulated Windows environment (e. g. replaces '/' by '\').
# VAR - name of the variable to store the result in
# PATH - the path to be translated
# Will unset VAR if an error occurred.
# Additional arguments will be ignored.
#
function(UNIX_TO_WINE_PATH VAR PATH)
    WINE_DETACHED(ERROR RESULT STDERR "${WINE_WINEPATH_PROGRAM}" "-w" "${PATH}")
    if(NOT ERROR)
        string(REPLACE "\n" "" RESULT ${RESULT})
        set(${VAR} ${RESULT} PARENT_SCOPE)
    else()
        unset(${VAR} PARENT_SCOPE)
        message(WARNING "Unable to convert path to Wine format: ${STDERR}")
    endif()
endfunction()

#
# Create a command (as string) to run a given command in the emulated Windows
# environment by using 'wine'.
# VAR - name of the variable to store the resulting command in
# COMMAND - the command to run, e. g. 'dir'
# Additional arguments will be appended to the command.
#
function(WINE_COMMAND VAR COMMAND)
    UNIX_TO_WINE_PATH(CMD ${COMMAND})
    separate_arguments(ARGS WINDOWS_COMMAND "${ARGN}")
    set(${VAR} "${CMD}" ${ARGS} PARENT_SCOPE)
endfunction()
