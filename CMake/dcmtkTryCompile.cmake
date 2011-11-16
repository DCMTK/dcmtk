#
# Check if a given C++ source code compiles
#
# VAR - Variable to set to 1 or 0, depending on the result
# MESSAGE - Description of the thing that we are checking for
# SOURCE - Code to compile
#
# All extra arguments are passed to TRY_COMPILE().
#

MACRO(DCMTK_TRY_COMPILE VAR MESSAGE SOURCE)
    SET(DCMTK_TRY_COMPILE_FILE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/src.cxx")

    # Do nothing if the variable is already set
    IF("${VAR}" MATCHES "^${VAR}$")
        MESSAGE(STATUS "Checking whether ${MESSAGE}")
        FILE(WRITE "${DCMTK_TRY_COMPILE_FILE}" "${SOURCE}\n")
        TRY_COMPILE(${VAR}
                    ${CMAKE_BINARY_DIR}
                    ${DCMTK_TRY_COMPILE_FILE}
                    OUTPUT_VARIABLE OUTPUT
                    ${ARGN})
        IF(${VAR})
            MESSAGE(STATUS "Checking whether ${MESSAGE} -- yes")
            SET(${VAR} 1 CACHE INTERNAL "${MESSAGE}")
            FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
                 "${MESSAGE} passed with the following output:\n"
                 "${OUTPUT}\n")
        ELSE(${VAR})
            MESSAGE(STATUS "Checking whether ${MESSAGE} -- no")
            SET(${VAR} 0 CACHE INTERNAL "${MESSAGE}")
            FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
                 "${MESSAGE} failed with the following output:\n"
                 "${OUTPUT}\n")
        ENDIF(${VAR})
    ENDIF("${VAR}" MATCHES "^${VAR}$")
ENDMACRO(DCMTK_TRY_COMPILE)
