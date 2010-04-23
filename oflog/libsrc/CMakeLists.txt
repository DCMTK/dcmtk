# create library from source files
ADD_LIBRARY(oflog oflog apndimpl appender config consap factory fileap filter globinit hierarchy hierlock layout logger logimpl logevent loglevel loglog lloguser ndc ntelogap nullap objreg patlay pointer property rootlog sleep socketap sockbuff socket strhelp syncprims syslogap threads timehelp winsock windebap winconap)

# declare installation files
INSTALL_TARGETS(${INSTALL_LIBDIR} oflog)
