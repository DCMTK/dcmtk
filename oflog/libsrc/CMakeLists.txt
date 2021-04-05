# create library from source files

if(WIN32 AND NOT CYGWIN)
  # clfs needs extra libraries and thus is ignored here
  set(OFLOG_PLATFORM_LIBRARIES
      winsock.cc
      windebap.cc
      winconap.cc
      )
else()
  set(OFLOG_PLATFORM_LIBRARIES unixsock.cc)
endif()

set(oflog_src
  apndimpl.cc
  appender.cc
  asyncap.cc
  clogger.cc
  config.cc
  consap.cc
  cygwin32.cc
  env.cc
  factory.cc
  fileap.cc
  fileinfo.cc
  filter.cc
  globinit.cc
  hierarchy.cc
  hierlock.cc
  layout.cc
  lloguser.cc
  lockfile.cc
  log4judp.cc
  logevent.cc
  logger.cc
  logimpl.cc
  loglevel.cc
  loglog.cc
  logmacro.cc
  mdc.cc
  ndc.cc
  ntelogap.cc
  nullap.cc
  objreg.cc
  oflog.cc
  patlay.cc
  pointer.cc
  property.cc
  queue.cc
  rootlog.cc
  sleep.cc
  snprintf.cc
  sockbuff.cc
  socketap.cc
  socket.cc
  strccloc.cc
  strcloc.cc
  strhelp.cc
  striconv.cc
  syncprims.cc
  syslogap.cc
  threads.cc
  timehelp.cc
  tls.cc
  version.cc
)

DCMTK_ADD_LIBRARY(oflog
  ${oflog_src}
  ${OFLOG_PLATFORM_LIBRARIES}
)

DCMTK_TARGET_LINK_MODULES(oflog ofstd)
