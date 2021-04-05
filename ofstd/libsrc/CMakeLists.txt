# create library from source files
set(ofstd_src
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
    oflist.cc
    ofmath.cc
    ofrand.cc
    ofsockad.cc
    ofstd.cc
    ofstring.cc
    ofstrutl.cc
    oftempf.cc
    ofthread.cc
    oftime.cc
    oftimer.cc
    ofuuid.cc
    ofxml.cc
)

DCMTK_ADD_LIBRARY(ofstd ${ofstd_src})

DCMTK_TARGET_LINK_LIBRARIES(ofstd ${CHARSET_CONVERSION_LIBS} ${SOCKET_LIBS} ${THREAD_LIBS} ${WIN32_STD_LIBRARIES})
