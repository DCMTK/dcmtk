# create library from source files

IF(WIN32 AND NOT CYGWIN)
  SET(OFLOG_PLATFORM_LIBRARIES winsock windebap winconap)
ELSE(WIN32 AND NOT CYGWIN)
  SET(OFLOG_PLATFORM_LIBRARIES unixsock)
ENDIF(WIN32 AND NOT CYGWIN)

ADD_LIBRARY(oflog oflog apndimpl appender config consap factory fileap filter globinit hierarchy hierlock layout logger logimpl logevent loglevel loglog lloguser ndc ntelogap nullap objreg patlay pointer property rootlog sleep socketap sockbuff socket strhelp syncprims syslogap threads timehelp ${OFLOG_PLATFORM_LIBRARIES})

# declare installation files
INSTALL_TARGETS(${INSTALL_LIBDIR} oflog)
