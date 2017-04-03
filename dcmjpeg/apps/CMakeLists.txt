# declare additional include directories
INCLUDE_DIRECTORIES("${dcmjpeg_SOURCE_DIR}/include" "${ofstd_SOURCE_DIR}/include" "${oflog_SOURCE_DIR}/include" "${dcmdata_SOURCE_DIR}/include" "${dcmimgle_SOURCE_DIR}/include" "${dcmimage_SOURCE_DIR}/include" ${ZLIB_INCDIR} ${LIBTIFF_INCDIR} ${LIBPNG_INCDIR})

# declare executables
FOREACH(PROGRAM dcmcjpeg dcmdjpeg dcmj2pnm dcmmkdir)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dcmcjpeg dcmdjpeg dcmj2pnm dcmmkdir)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmjpeg ijg8 ijg12 ijg16 dcmimage dcmimgle dcmdata oflog ofstd)
ENDFOREACH(PROGRAM)
