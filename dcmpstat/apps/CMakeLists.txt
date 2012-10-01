# declare additional include directories
INCLUDE_DIRECTORIES(${dcmtls_SOURCE_DIR}/include ${LIBXML_INCDIR})

# declare directories containing used libraries
LINK_DIRECTORIES(${dcmpstat_BINARY_DIR} ${ofstd_BINARY_DIR} ${oflog_BINARY_DIR} ${dcmdata_BINARY_DIR} ${dcmtls_BINARY_DIR} ${dcmnet_BINARY_DIR} ${dcmqrdb_BINARY_DIR} ${dcmimgle_BINARY_DIR} ${dcmimage_BINARY_DIR} ${dcmsr_BINARY_DIR} ${dcmsign_BINARY_DIR} ${ZLIB_LIBDIR} ${LIBICONV_LIBDIR} ${LIBXML_LIBDIR} ${OPENSSL_LIBDIR} ${LIBTIFF_LIBDIR} ${LIBPNG_LIBDIR})

# declare executables
FOREACH(PROGRAM dcmmkcrv dcmmklut dcmp2pgm dcmprscp dcmprscu dcmpschk dcmpsmk dcmpsprt dcmpsrcv dcmpssnd)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dcmmkcrv dcmmklut dcmp2pgm dcmprscp dcmprscu dcmpsmk dcmpschk dcmpsprt dcmpsrcv dcmpssnd)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmpstat dcmdsig dcmsr dcmimage dcmimgle dcmqrdb dcmnet dcmtls dcmdata oflog ofstd)
  DCMTK_TARGET_LINK_LIBRARIES(${PROGRAM} ${LIBTIFF_LIBS} ${LIBPNG_LIBS} ${LIBXML_LIBS} ${OPENSSL_LIBS} ${ZLIB_LIBS} ${LIBICONV_LIBS} ${WIN32_STD_LIBRARIES} ${THREAD_LIBS})
ENDFOREACH(PROGRAM)
