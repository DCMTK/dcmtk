# create library from source files

IF(WIN32 AND NOT CYGWIN)
  SET(OFLOG_PLATFORM_LIBRARIES winsock windebap winconap)
ELSE(WIN32 AND NOT CYGWIN)
  SET(OFLOG_PLATFORM_LIBRARIES unixsock)
ENDIF(WIN32 AND NOT CYGWIN)

ADD_LIBRARY(oflog oflog apndimpl appender config consap factory fileap filter globinit hierarchy hierlock layout logger logimpl logevent loglevel loglog lloguser ndc ntelogap nullap objreg patlay pointer property rootlog sleep socketap sockbuff socket strhelp syncprims syslogap threads timehelp ${OFLOG_PLATFORM_LIBRARIES})

TARGET_LINK_LIBRARIES(oflog ofstd ${WIN32_STD_LIBRARIES})

# set proper version information for shared library
IF(BUILD_SHARED_LIBS)
  SET_TARGET_PROPERTIES(oflog PROPERTIES ${DCMTK_LIBRARY_PROPERTIES})
ENDIF(BUILD_SHARED_LIBS)

# declare installation files
INSTALL(TARGETS oflog
        COMPONENT lib
        RUNTIME DESTINATION ${DCMTK_INSTALL_BINDIR}
        LIBRARY DESTINATION ${DCMTK_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${DCMTK_INSTALL_LIBDIR})
