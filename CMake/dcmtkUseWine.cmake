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
ENDFUNCTION(DCMTK_SETUP_WINE)

#
# Uses 'winepath' to translate a host path or filename to the location it can be
# accessed from within the emulated Windows environment (e. g. replaces '/' by '\').
# VAR - name of the variable to store the result in
# PATH - the path to be translated
# Will unset VAR if an error occurred.
# Additional arguments will be ignored.
#
FUNCTION(UNIX_TO_WINE_PATH VAR PATH)
    EXECUTE_PROCESS(
        COMMAND ${WINE_WINEPATH_PROGRAM} -w ${PATH}
        RESULT_VARIABLE ERROR
        OUTPUT_VARIABLE RESULT
        ERROR_VARIABLE STDERR
    )
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
    SET(${VAR} ${WINE_WINE_PROGRAM} cmd /c "PATH=${WINE_CPP_RUNTIME}\;%PATH%" \\& ${CMD} ${ARGS} PARENT_SCOPE)
ENDFUNCTION()
