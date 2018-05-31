#
# this file is used to execute the unit tests with the -x option injected
#

set(ENV{DCMTK_CTEST_EXTRA_ARGUMENTS} "-x")
execute_process(COMMAND ${CMAKE_CTEST_COMMAND} -C "${CONFIG}")
