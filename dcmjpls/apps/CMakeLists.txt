# declare additional include directories
INCLUDE_DIRECTORIES("${dcmjpls_SOURCE_DIR}/include" "${ofstd_SOURCE_DIR}/include" "${oflog_SOURCE_DIR}/include" "${dcmdata_SOURCE_DIR}/include" "${dcmimgle_SOURCE_DIR}/include" "${dcmimage_SOURCE_DIR}/include" ${ZLIB_INCDIR} ${LIBTIFF_INCDIR} ${LIBPNG_INCDIR})

# declare executables
FOREACH(PROGRAM dcmcjpls dcmdjpls dcml2pnm)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dcmcjpls dcmdjpls dcml2pnm)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmjpls charls dcmimage dcmimgle dcmdata oflog ofstd ofstd)
ENDFOREACH(PROGRAM)
