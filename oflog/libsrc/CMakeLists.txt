# create library from source files

if(WIN32 AND NOT CYGWIN)
  # clfs needs extra libraries and thus is ignored here
  set(OFLOG_PLATFORM_LIBRARIES winsock windebap winconap)
else()
  set(OFLOG_PLATFORM_LIBRARIES unixsock)
endif()

DCMTK_ADD_LIBRARY(oflog oflog apndimpl appender config consap factory fileap filter globinit hierarchy hierlock layout logger logimpl logevent loglevel loglog lloguser ndc ntelogap nullap objreg patlay pointer property rootlog sleep socketap sockbuff socket strhelp syncprims syslogap threads timehelp clogger env fileinfo lockfile mdc queue snprintf tls version log4judp logmacro asyncap cygwin32 striconv strcloc strccloc ${OFLOG_PLATFORM_LIBRARIES})

DCMTK_TARGET_LINK_MODULES(oflog ofstd)
