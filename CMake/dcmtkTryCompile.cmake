#
# Check if a given C++ source code compiles
#
# VAR - Variable to set to 1 or 0, depending on the result
# MESSAGE - Description of the thing that we are checking for
# SOURCE - Code to compile
#
# All extra arguments are passed to try_compile().
#

macro(DCMTK_TRY_COMPILE VAR MESSAGE SOURCE)
    set(DCMTK_TRY_COMPILE_FILE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/src.cxx")

    # Do nothing if the variable is already set
    if(NOT DEFINED "${VAR}")
        message(STATUS "Checking whether ${MESSAGE}")
        file(WRITE "${DCMTK_TRY_COMPILE_FILE}" "${SOURCE}\n")
        if(DCMTK_TRY_COMPILE_REQUIRED_CMAKE_FLAGS)
            set(DCMTK_TRY_COMPILE_CMAKE_FLAGS CMAKE_FLAGS ${DCMTK_TRY_COMPILE_REQUIRED_CMAKE_FLAGS})
        else()
            DCMTK_UNSET(DCMTK_TRY_COMPILE_CMAKE_FLAGS)
        endif()
        try_compile(${VAR}
                    "${CMAKE_BINARY_DIR}"
                    "${DCMTK_TRY_COMPILE_FILE}"
                    ${DCMTK_TRY_COMPILE_CMAKE_FLAGS}
                    OUTPUT_VARIABLE OUTPUT
                    ${ARGN})
        if(${VAR})
            message(STATUS "Checking whether ${MESSAGE} -- yes")
            set(${VAR} 1 CACHE INTERNAL "${MESSAGE}")
            file(APPEND "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log"
                 "${MESSAGE} passed with the following output:\n"
                 "${OUTPUT}\n")
        else()
            message(STATUS "Checking whether ${MESSAGE} -- no")
            set(${VAR} 0 CACHE INTERNAL "${MESSAGE}")
            file(APPEND "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log"
                 "${MESSAGE} failed with the following output:\n"
                 "${OUTPUT}\n")
        endif()
    endif()
endmacro()
