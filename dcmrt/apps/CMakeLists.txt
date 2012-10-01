# declare directories containing used libraries
LINK_DIRECTORIES(${dcmrt_BINARY_DIR} ${ofstd_BINARY_DIR} ${oflog_BINARY_DIR} ${dcmdata_BINARY_DIR} ${ZLIB_LIBDIR} ${LIBICONV_LIBDIR})

# declare executables
DCMTK_ADD_EXECUTABLE(drtdump drtdump)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(drtdump dcmrt dcmdata oflog ofstd)
DCMTK_TARGET_LINK_LIBRARIES(drtdump ${ZLIB_LIBS} ${LIBICONV_LIBS} ${WIN32_STD_LIBRARIES} ${THREAD_LIBS})
