# create library from source files
DCMTK_ADD_LIBRARY(dcmtls
  tlscond.cc
  tlslayer.cc
  tlsopt.cc
  tlstrans.cc
  tlsscu.cc
  tlsciphr.cc
)

DCMTK_TARGET_LINK_MODULES(dcmtls ofstd dcmdata dcmnet)
if(OPENSSL_MODULES)
  DCMTK_TARGET_LINK_MODULES(dcmtls ${OPENSSL_MODULES})
else()
  DCMTK_TARGET_LINK_LIBRARIES(dcmtls ${OPENSSL_LIBS})
endif()

if(TARGET dcmtls)
  set_property(TARGET dcmtls PROPERTY OUTPUT_NAME "dcmtls${DCMTK_TLS_LIBRARY_POSTFIX}")
endif()
