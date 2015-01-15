#
# Functions for detection and usage of Wine
# Used when cross compiling
#

FUNCTION(UNIX_TO_WINE_PATH VAR PATH)
    IF(NOT WINE_WINEPATH_PROGRAM)
        FIND_PROGRAM(WINE_WINEPATH_PROGRAM winepath)
        MESSAGE(FATAL_ERROR "The program 'winepath' is required for running applications targetting Windows.")
    ELSE()
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
    ENDIF()
ENDFUNCTION()

FUNCTION(WINE_COMMAND VAR COMMAND)
    IF(NOT WINE_WINE_PROGRAM)
        FIND_PROGRAM(WINE_WINE_PROGRAM wine)
        MESSAGE(FATAL_ERROR "The program 'wine' is required for running applications targetting Windows.")
    ELSE()
        IF(NOT WINE_CPP_RUNTIME)
            SET(v 1)
            UNSET(CMAKE_FIND_LIBRARY_SUFFIXES)
            WHILE(${v} LESS 14)
                LIST(APPEND CMAKE_FIND_LIBRARY_SUFFIXES "-${v}.dll")
                MATH(EXPR v "${v} + 1")
            ENDWHILE()
            FIND_LIBRARY(CPP_RUNTIME stdc++ DOC "Location of the C++ runtime library used during target emulation")
            IF(NOT CPP_RUNTIME)
                MESSAGE(FATAL_ERROR "Please set the C++ runtime location (required for running applications targetting Windows).")
            ELSE()
                GET_FILENAME_COMPONENT(CPP_RUNTIME_LOCATION ${CPP_RUNTIME} PATH)
                UNIX_TO_WINE_PATH(RT ${CPP_RUNTIME_LOCATION})
                SET(WINE_CPP_RUNTIME ${RT} CACHE INTERNAL "C++ RT in Wine path format")
            ENDIF()
        ELSE()
            UNIX_TO_WINE_PATH(CMD ${COMMAND})
            SET(${VAR} ${WINE_WINE_PROGRAM} cmd /c "PATH=${WINE_CPP_RUNTIME}\;%PATH%" \\& ${CMD} ${ARGN} PARENT_SCOPE)
        ENDIF()
    ENDIF()
ENDFUNCTION()
