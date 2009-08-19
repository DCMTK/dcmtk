# create library from source files
ADD_LIBRARY(oflog oflog appenderattachableimpl appender configurator consoleappender factory fileappender filter global-init hierarchy hierarchylocker layout logger loggerimpl loggingevent loglevel loglog logloguser ndc nteventlogappender nullappender objectregistry patternlayout pointer property rootlogger sleep socketappender socketbuffer socket stringhelper syncprims syslogappender threads timehelper socket-win32 win32debugappender)

# declare installation files
INSTALL_TARGETS(${INSTALL_LIBDIR} oflog)
