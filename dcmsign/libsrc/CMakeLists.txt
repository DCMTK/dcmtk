# create library from source files
ADD_LIBRARY(dcmdsig dcsignat siautopr sibrsapr sicert sicertvf sicreapr sidsa simaccon simd5 sinullpr siprivat siripemd sirsa sisha1 sisprof sitypes)

TARGET_LINK_LIBRARIES(dcmdsig ofstd dcmdata ${OPENSSL_LIBS})

# declare installation files
INSTALL(TARGETS dcmdsig
        COMPONENT lib
        RUNTIME DESTINATION ${DCMTK_INSTALL_BINDIR}
        LIBRARY DESTINATION ${DCMTK_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${DCMTK_INSTALL_LIBDIR})
