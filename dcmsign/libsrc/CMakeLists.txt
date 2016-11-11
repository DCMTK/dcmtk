# create library from source files
DCMTK_ADD_LIBRARY(dcmdsig dcsignat siautopr sibrsapr sicert sicertvf sicreapr sidsa simaccon simd5 sinullpr siprivat siripemd sirsa sisha1 sisprof sitypes sisha256 sisha384 sisha512)

DCMTK_TARGET_LINK_MODULES(dcmdsig ofstd dcmdata)
DCMTK_TARGET_LINK_LIBRARIES(dcmdsig ${OPENSSL_LIBS})
