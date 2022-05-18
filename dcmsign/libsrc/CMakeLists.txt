# create library from source files
DCMTK_ADD_LIBRARY(dcmdsig
  dcsighlp.cc
  dcsignat.cc
  siautopr.cc
  sibrsapr.cc
  sicert.cc
  sicertvf.cc
  sicreapr.cc
  simaccon.cc
  simdmac.cc
  sinullpr.cc
  sipkey.cc
  siprivat.cc
  sipurpos.cc
  sisprof.cc
  sisrpr.cc
  sisrvpr.cc
  sitsfs.cc
  sitstamp.cc
  sitypes.cc
)

DCMTK_TARGET_LINK_MODULES(dcmdsig ofstd dcmdata)
DCMTK_TARGET_LINK_LIBRARIES(dcmdsig ${OPENSSL_LIBS})

if (TARGET dcmdsig)
  set_property(TARGET dcmdsig PROPERTY OUTPUT_NAME "dcmdsig${DCMTK_TLS_LIBRARY_POSTFIX}")
endif()
