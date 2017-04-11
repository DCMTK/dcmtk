#
# Functions for detection and usage of Wine
# Used when cross compiling
#

#
# Tries to detect the required executables and libraries automatically and emits an
# error that will halt the configuration process if that is not possible.
# All arguments will be ignored.
#
FUNCTION(DCMTK_SETUP_WINE)
    FIND_PROGRAM(WINE_WINE_PROGRAM wine)
    FIND_PROGRAM(WINE_WINEPATH_PROGRAM winepath)
    IF(NOT WINE_CPP_RUNTIME)
        SET(v 1)
        UNSET(CMAKE_FIND_LIBRARY_SUFFIXES)
        WHILE(${v} LESS 14)
            LIST(APPEND CMAKE_FIND_LIBRARY_SUFFIXES "-${v}.dll")
            MATH(EXPR v "${v} + 1")
        ENDWHILE()
        FIND_LIBRARY(CPP_RUNTIME stdc++ DOC "Location of the C++ runtime library used during target emulation")
        IF(NOT CPP_RUNTIME)
            MESSAGE(FATAL_ERROR "Please set the C++ runtime location (required for running applications targeting Windows).")
        ELSE()
            GET_FILENAME_COMPONENT(CPP_RUNTIME_LOCATION ${CPP_RUNTIME} PATH)
            UNIX_TO_WINE_PATH(RT ${CPP_RUNTIME_LOCATION})
            SET(WINE_CPP_RUNTIME ${RT} CACHE INTERNAL "C++ RT in Wine path format")
        ENDIF()
    ENDIF()
    IF(NOT WINE_WINE_PROGRAM OR NOT WINE_WINEPATH_PROGRAM)
        MESSAGE(FATAL_ERROR
            "Some wine related executables could not be detected automatically. "
            "Please set WINE_WINE_PROGRAM and WINE_WINEPATH_PROGRAM appropriately."
        )
    ENDIF()
    # prepare wine prefix for configure and unit tests
    IF(NOT DCMTK_WINEPREFIX)
        SET(DCMTK_WINEPREFIX "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/wineprefix" CACHE INTERNALE "the path of the wineprefix to use for configuration and unit tests")
        MESSAGE(STATUS "Info: Preparing wine prefix for configuration and unit tests: ${DCMTK_WINEPREFIX}")
        STRING(REPLACE "\\" "\\\\" WINE_CPP_RUNTIME "${WINE_CPP_RUNTIME}")
        SET(WINE_PATH_REG "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/dcmtkWinePath.reg")
        CONFIGURE_FILE("${DCMTK_SOURCE_DIR}/CMake/dcmtkWinePath.reg.in" "${WINE_PATH_REG}" ESCAPE_QUOTES @ONLY)
        SET(ENV{WINEPREFIX} "${DCMTK_WINEPREFIX}")
        EXECUTE_PROCESS(COMMAND "${WINE_WINE_PROGRAM}" "regedit" "${WINE_PATH_REG}"
            OUTPUT_FILE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/wineprefix.log"
            ERROR_FILE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/wineprefix.log"
            OUTPUT_VARIABLE RESULT
        )
        IF(RESULT)
            SET(DCMTK_WINEPREFIX CACHE INTERNAL "error, see ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/wineprefix.log")
            MESSAGE(FATAL_ERROR "Failed to setup the wineprefix, see \"${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/wineprefix.log\"")
        ENDIF()
    ELSE()
        SET(ENV{WINEPREFIX} "${DCMTK_WINEPREFIX}")
    ENDIF()
ENDFUNCTION(DCMTK_SETUP_WINE)

#
# Helper function to detach the output and error streams from a wine process, so
# that CMake doesn't wait for the wineserver to quit before continuing execution.
# VAR - the variable that will hold the exit code of the launched process.
# OUTPUT_VAR - the variable that will hold the standard output of the launched process.
# ERROR_VAR - the variable that will hold the error output of the launched process.
# ARGN - the command to execute.
#
FUNCTION(WINE_DETACHED VAR OUTPUT_VAR ERROR_VAR)
    # Prefix to prevent collision of output capturing files
    IF(CMAKE_VERSION VERSION_LESS 2.8.7)
        STRING(RANDOM LENGTH 20 ALPHABET "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" PREFIX)
    ELSE()
        STRING(MD5 PREFIX "${ARGN}")
    ENDIF()
    SET(OUTPUT_FILE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PREFIX}_output")
    IF(OUTPUT_VAR STREQUAL ERROR_VAR)
        SET(ERROR_FILE "${OUTPUT_FILE}")
    ELSE()
        SET(ERROR_FILE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PREFIX}_error")
    ENDIF()
    EXECUTE_PROCESS(
        COMMAND ${ARGN}
        OUTPUT_FILE "${OUTPUT_FILE}"
        ERROR_FILE "${ERROR_FILE}"
        RESULT_VARIABLE RESULT
    )
    FILE(READ "${OUTPUT_FILE}" OUTPUT)
    SET("${OUTPUT_VAR}" ${OUTPUT} PARENT_SCOPE)
    IF(OUTPUT_VAR STREQUAL ERROR_VAR)
        FILE(REMOVE "${OUTPUT_FILE}")
    ELSE()
        FILE(READ "${ERROR_FILE}" ERROR)
        SET("${ERROR_VAR}" ${ERROR} PARENT_SCOPE)
        FILE(REMOVE "${OUTPUT_FILE}" "${ERROR_FILE}")
    ENDIF()
    SET("${VAR}" ${RESULT} PARENT_SCOPE)
ENDFUNCTION(WINE_DETACHED)

#
# Uses 'winepath' to translate a host path or filename to the location it can be
# accessed from within the emulated Windows environment (e. g. replaces '/' by '\').
# VAR - name of the variable to store the result in
# PATH - the path to be translated
# Will unset VAR if an error occurred.
# Additional arguments will be ignored.
#
FUNCTION(UNIX_TO_WINE_PATH VAR PATH)
    WINE_DETACHED(ERROR RESULT STDERR "${WINE_WINEPATH_PROGRAM}" "-w" "${PATH}")
    IF(NOT ERROR)
        STRING(REPLACE "\n" "" RESULT ${RESULT})
        SET(${VAR} ${RESULT} PARENT_SCOPE)
    ELSE()
        UNSET(${VAR} PARENT_SCOPE)
        MESSAGE(WARNING "Unable to convert path to Wine format: ${STDERR}")
    ENDIF()
ENDFUNCTION()

#
# Create a command (as string) to run a given command in the emulated Windows
# environment by using 'wine'.
# VAR - name of the variable to store the resulting command in
# COMMAND - the command to run, e. g. 'dir'
# Additional arguments will be appended to the command.
#
FUNCTION(WINE_COMMAND VAR COMMAND)
    UNIX_TO_WINE_PATH(CMD ${COMMAND})
    SEPARATE_ARGUMENTS(ARGS WINDOWS_COMMAND "${ARGN}")
    SET(${VAR} "${CMD}" ${ARGS} PARENT_SCOPE)
ENDFUNCTION()
