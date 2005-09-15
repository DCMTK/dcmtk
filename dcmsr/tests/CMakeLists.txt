# declare directories containing used libraries
LINK_DIRECTORIES(${dcmsr_BINARY_DIR} ${ofstd_BINARY_DIR} ${dcmdata_BINARY_DIR} ${ZLIB_LIBDIR} ${LIBXML_LIBDIR})

# declare executables
ADD_EXECUTABLE(mkreport mkreport)

# declare installation files
INSTALL_TARGETS(${INSTALL_BINDIR} mkreport)

# make sure executables are linked to the corresponding libraries
TARGET_LINK_LIBRARIES(mkreport netapi32 wsock32 ofstd dcmdata dcmsr ${ZLIB_LIBS} ${LIBXML_LIBS})
