# declare directories containing used libraries
LINK_DIRECTORIES(${dcmrt_BINARY_DIR} ${ofstd_BINARY_DIR} ${oflog_BINARY_DIR} ${dcmdata_BINARY_DIR} ${ZLIB_LIBDIR} ${LIBICONV_LIBDIR})

# declare executables
DCMTK_ADD_EXECUTABLE(drtdump drtdump)

# declare installation files
INSTALL(TARGETS drtdump DESTINATION ${DCMTK_INSTALL_BINDIR} COMPONENT bin)

# make sure executables are linked to the corresponding libraries
TARGET_LINK_LIBRARIES(drtdump dcmrt dcmdata oflog ofstd ${ZLIB_LIBS} ${LIBICONV_LIBS} ${WIN32_STD_LIBRARIES} ${THREAD_LIBS})
