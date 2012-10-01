# declare directories containing used libraries
LINK_DIRECTORIES(${dcmsign_BINARY_DIR} ${dcmdata_BINARY_DIR} ${oflog_BINARY_DIR} ${ofstd_BINARY_DIR} ${ZLIB_LIBDIR} ${LIBICONV_LIBDIR} ${OPENSSL_LIBDIR})

# declare executables
DCMTK_ADD_EXECUTABLE(dcmsign dcmsign)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(dcmsign dcmdsig dcmdata oflog ofstd)
DCMTK_TARGET_LINK_LIBRARIES(dcmsign ${ZLIB_LIBS} ${LIBICONV_LIBS} ${OPENSSL_LIBS} ${WIN32_STD_LIBRARIES} ${THREAD_LIBS})
