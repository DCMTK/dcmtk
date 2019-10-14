#
#  Copyright (C) 2019, Jan Schlamelcher, OFFIS e.V.
#  All rights reserved.  See COPYRIGHT file for details.
#
#  Purpose: Fix some details in Doxygen generated manpages that can not be
#           configured via Doxygen itself directly.
#

file(GLOB MANPAGES "${CMAKE_CURRENT_BINARY_DIR}/manpages/man1/*.1")
foreach(MANPAGE ${MANPAGES})
  if(MANPAGE MATCHES "[.]man[.]1$" OR MANPAGE MATCHES "_([^/]*)_[.]1$")
    # The file is some Doxygen generated manpage index that we don't want.
    file(REMOVE "${MANPAGE}")
  else()
    # The file is a useful manpage, but it might contain a ".ad l" statement
    # at the beginning that messes with text justification.
    file(READ "${MANPAGE}" CONTENT)
    string(REGEX REPLACE "(\r|\n|\r\n)[.]ad l" "" CONTENT "${CONTENT}")
    file(WRITE "${MANPAGE}" "${CONTENT}")
  endif()
endforeach()
