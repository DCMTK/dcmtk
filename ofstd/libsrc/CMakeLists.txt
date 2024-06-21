# create library from source files
DCMTK_ADD_LIBRARY(ofstd 
  ofchrenc.cc
  ofcmdln.cc
  ofconapp.cc
  ofcond.cc
  ofconfig.cc
  ofconsol.cc
  ofcrc32.cc
  ofdate.cc
  ofdatime.cc
  oferror.cc
  offile.cc
  offilsys.cc
  offname.cc
  ofipc.cc
  oflist.cc
  ofstd.cc
  ofstring.cc
  ofstrutl.cc
  ofthread.cc
  oftime.cc
  oftimer.cc
  oftempf.cc
  ofxml.cc
  ofuuid.cc
  ofmath.cc
  ofsockad.cc
  ofrand.cc
)

DCMTK_TARGET_LINK_LIBRARIES(ofstd config ${CHARSET_CONVERSION_LIBS} ${SOCKET_LIBS} ${THREAD_LIBS} ${WIN32_STD_LIBRARIES})
DCMTK_TARGET_LINK_MODULES(ofstd oficonv)
