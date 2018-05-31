#
# Check if the symbol exists in include files
#
# CHECK_FUNCTIONWITHHEADER_EXISTS - macro which checks the symbol exists in include files.
# SYMBOL - symbol
# FILES  - include files to check
# VARIABLE - variable to return result
# ... - additional arguments will be interpreted as additional libraries to link during testing.
#

macro(CHECK_FUNCTIONWITHHEADER_EXISTS SYMBOL FILES VARIABLE)
  if(NOT DEFINED "${VARIABLE}")
    set(CHECK_SYMBOL_EXISTS_CONTENT "/* */\n")
    set(MACRO_CHECK_SYMBOL_EXISTS_FLAGS ${CMAKE_REQUIRED_FLAGS})
    if(CMAKE_REQUIRED_LIBRARIES)
      set(CHECK_SYMBOL_EXISTS_LIBS
        "-DLINK_LIBRARIES:STRING=${CMAKE_REQUIRED_LIBRARIES};${ARGN}")
    endif()
    foreach(FILE ${FILES})
      set(CHECK_SYMBOL_EXISTS_CONTENT
        "${CHECK_SYMBOL_EXISTS_CONTENT}#include <${FILE}>\n")
    endforeach()
    set(CHECK_SYMBOL_EXISTS_CONTENT
      "${CHECK_SYMBOL_EXISTS_CONTENT}\nint main()\n{\n${SYMBOL};return 0;\n}\n")

    file(WRITE ${CMAKE_BINARY_DIR}/CMakeTmp/CheckSymbolExists.cxx
      "${CHECK_SYMBOL_EXISTS_CONTENT}")

    message(STATUS "Looking for prototype of ${SYMBOL}")
    try_compile(${VARIABLE}
      "${CMAKE_BINARY_DIR}"
      "${CMAKE_BINARY_DIR}/CMakeTmp/CheckSymbolExists.cxx"
      CMAKE_FLAGS
      -DCOMPILE_DEFINITIONS:STRING=${MACRO_CHECK_SYMBOL_EXISTS_FLAGS}
      "-DINCLUDE_DIRECTORIES:STRING=${CMAKE_REQUIRED_INCLUDES}"
      "${CHECK_SYMBOL_EXISTS_LIBS}"
      ${DCMTK_TRY_COMPILE_REQUIRED_CMAKE_FLAGS}
      OUTPUT_VARIABLE OUTPUT)
    if(${VARIABLE})
      message(STATUS "Looking for prototype of ${SYMBOL} - found")
      set(${VARIABLE} 1 CACHE INTERNAL "Have symbol ${SYMBOL}")
      file(APPEND "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log"
        "Determining if the ${SYMBOL} "
        "exist passed with the following output:\n"
        "${OUTPUT}\nFile ${CMAKE_BINARY_DIR}/CMakeTmp/CheckSymbolExists.ccc:\n"
        "${CHECK_SYMBOL_EXISTS_CONTENT}\n")
    else()
      message(STATUS "Looking for prototype of ${SYMBOL} - not found.")
      set(${VARIABLE} "" CACHE INTERNAL "Have symbol ${SYMBOL}")
      file(APPEND "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log"
        "Determining if the ${SYMBOL} "
        "exist failed with the following output:\n"
        "${OUTPUT}\nFile ${CMAKE_BINARY_DIR}/CMakeTmp/CheckSymbolExists.cxx:\n"
        "${CHECK_SYMBOL_EXISTS_CONTENT}\n")
    endif()
  endif()
endmacro()
