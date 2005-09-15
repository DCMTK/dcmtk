# create library from source files
ADD_LIBRARY(dcmdsig dcsignat siautopr sibrsapr sicert sicertvf sicreapr sidsa simaccon simd5 sinullpr siprivat siripemd sirsa sisha1 sisprof sitypes)

# declare installation files
INSTALL_TARGETS(${INSTALL_LIBDIR} dcmdsig)
