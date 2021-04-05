# create library from source files
set(dcmdsig_src 
  dcsighlp.cc
  dcsignat.cc
  siautopr.cc
  sibrsapr.cc
  sicert.cc
  sicertvf.cc
  sicreapr.cc
  simaccon.cc
  sinullpr.cc
  siprivat.cc
  sipurpos.cc
  sipkey.cc
  simdmac.cc
  sisprof.cc
  sisrpr.cc
  sisrvpr.cc
  sitstamp.cc
  sitypes.cc
  sitsfs.cc
)

DCMTK_ADD_LIBRARY(dcmdsig ${dcmdsig_src})

DCMTK_TARGET_LINK_MODULES(dcmdsig ofstd dcmdata)
DCMTK_TARGET_LINK_LIBRARIES(dcmdsig ${OPENSSL_LIBS})
set_property(TARGET dcmdsig PROPERTY OUTPUT_NAME "dcmdsig${DCMTK_TLS_LIBRARY_POSTFIX}")
