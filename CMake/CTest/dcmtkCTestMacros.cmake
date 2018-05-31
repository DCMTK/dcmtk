#
# This file "re-implements" some of CMake's functionality
# that is missing inside CTest.
#

macro(MESSAGE_COMMAND VAR MODE)
    if(${MODE} STREQUAL "STATUS")
        set(${VAR} ${CMAKE_COMMAND} -E echo -- ${ARGN})
    elseif(${MODE} STREQUAL "WARNING")
        set(${VAR} ${CMAKE_COMMAND} -E echo WARNING: ${ARGN})
    elseif(${MODE} STREQUAL "AUTHOR_WARNING")
        set(${VAR} ${CMAKE_COMMAND} -E echo Warning: ${ARGN})
    elseif(${MODE} STREQUAL "SEND_ERROR")
        set(${VAR} ${CMAKE_COMMAND} -E echo Error: ${ARGN})
    elseif(${MODE} STREQUAL "FATAL_ERROR")
        set(${VAR} ${CMAKE_COMMAND} -E echo ERROR: ${ARGN})
    elseif(${MODE} STREQUAL "DEPRECATION")
        set(${VAR} ${CMAKE_COMMAND} -E echo ${ARGN})
    else()
        set(${VAR} ${CMAKE_COMMAND} -E echo ${MODE} ${ARGN})
    endif()
endmacro()

function(MESSAGE)
    MESSAGE_COMMAND(COMMAND ${ARGN})
    execute_process(COMMAND ${COMMAND})
endfunction()
