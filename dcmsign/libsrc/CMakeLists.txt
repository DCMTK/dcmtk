# create library from source files
DCMTK_ADD_LIBRARY(
  dcmdsig dcsighlp dcsignat siautopr sibrsapr sicert sicertvf sicreapr
  simaccon sinullpr siprivat sipurpos sipkey simdmac sisprof sisrpr
  sisrvpr sitstamp sitypes sitsfs)

DCMTK_TARGET_LINK_MODULES(dcmdsig ofstd dcmdata)
DCMTK_TARGET_LINK_LIBRARIES(dcmdsig ${OPENSSL_LIBS})
set_property(TARGET dcmdsig PROPERTY OUTPUT_NAME "dcmdsig${DCMTK_TLS_LIBRARY_POSTFIX}")
