#
# this file is used to execute the unit tests with the -x option injected
#

SET(ENV{DCMTK_CTEST_EXTRA_ARGUMENTS} "-x")
EXECUTE_PROCESS(COMMAND ${CMAKE_CTEST_COMMAND})
