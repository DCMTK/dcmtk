#
# Check if the symbol exists in include files
#
# CHECK_FUNCTIONWITHHEADER_EXISTS - macro which checks the symbol exists in include files.
# SYMBOL - symbol
# FILES  - include files to check
# VARIABLE - variable to return result
# ... - additional arguments will be interpreted as additional libraries to link during testing.
#

MACRO(CHECK_FUNCTIONWITHHEADER_EXISTS SYMBOL FILES VARIABLE)
  IF(NOT DEFINED "${VARIABLE}")
    SET(CHECK_SYMBOL_EXISTS_CONTENT "/* */\n")
    SET(MACRO_CHECK_SYMBOL_EXISTS_FLAGS ${CMAKE_REQUIRED_FLAGS})
    IF(CMAKE_REQUIRED_LIBRARIES)
      SET(CHECK_SYMBOL_EXISTS_LIBS
        "-DLINK_LIBRARIES:STRING=${CMAKE_REQUIRED_LIBRARIES};${ARGN}")
    ENDIF(CMAKE_REQUIRED_LIBRARIES)
    FOREACH(FILE ${FILES})
      SET(CHECK_SYMBOL_EXISTS_CONTENT
        "${CHECK_SYMBOL_EXISTS_CONTENT}#include <${FILE}>\n")
    ENDFOREACH(FILE)
    SET(CHECK_SYMBOL_EXISTS_CONTENT
      "${CHECK_SYMBOL_EXISTS_CONTENT}\nint main()\n{\n${SYMBOL};return 0;\n}\n")

    FILE(WRITE ${CMAKE_BINARY_DIR}/CMakeTmp/CheckSymbolExists.cxx
      "${CHECK_SYMBOL_EXISTS_CONTENT}")

    MESSAGE(STATUS "Looking for prototype of ${SYMBOL}")
    TRY_COMPILE(${VARIABLE}
      "${CMAKE_BINARY_DIR}"
      "${CMAKE_BINARY_DIR}/CMakeTmp/CheckSymbolExists.cxx"
      CMAKE_FLAGS
      -DCOMPILE_DEFINITIONS:STRING=${MACRO_CHECK_SYMBOL_EXISTS_FLAGS}
      "${CHECK_SYMBOL_EXISTS_LIBS}"
      ${DCMTK_TRY_COMPILE_REQUIRED_CMAKE_FLAGS}
      OUTPUT_VARIABLE OUTPUT)
    IF(${VARIABLE})
      MESSAGE(STATUS "Looking for prototype of ${SYMBOL} - found")
      SET(${VARIABLE} 1 CACHE INTERNAL "Have symbol ${SYMBOL}")
      FILE(APPEND "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log"
        "Determining if the ${SYMBOL} "
        "exist passed with the following output:\n"
        "${OUTPUT}\nFile ${CMAKE_BINARY_DIR}/CMakeTmp/CheckSymbolExists.ccc:\n"
        "${CHECK_SYMBOL_EXISTS_CONTENT}\n")
    ELSE(${VARIABLE})
      MESSAGE(STATUS "Looking for prototype of ${SYMBOL} - not found.")
      SET(${VARIABLE} "" CACHE INTERNAL "Have symbol ${SYMBOL}")
      FILE(APPEND "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log"
        "Determining if the ${SYMBOL} "
        "exist failed with the following output:\n"
        "${OUTPUT}\nFile ${CMAKE_BINARY_DIR}/CMakeTmp/CheckSymbolExists.cxx:\n"
        "${CHECK_SYMBOL_EXISTS_CONTENT}\n")
    ENDIF(${VARIABLE})
  ENDIF(NOT DEFINED "${VARIABLE}")
ENDMACRO(CHECK_FUNCTIONWITHHEADER_EXISTS)
