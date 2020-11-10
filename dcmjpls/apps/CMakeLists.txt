# declare additional include directories
include_directories("${dcmjpls_SOURCE_DIR}/include" "${ofstd_SOURCE_DIR}/include" "${oflog_SOURCE_DIR}/include" "${dcmdata_SOURCE_DIR}/include" "${dcmimgle_SOURCE_DIR}/include" "${dcmimage_SOURCE_DIR}/include" ${ZLIB_INCDIR} ${LIBTIFF_INCDIR} ${LIBPNG_INCDIR})

# declare executables
foreach(PROGRAM dcmcjpls dcmdjpls dcml2pnm)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
endforeach()

# make sure executables are linked to the corresponding libraries
foreach(PROGRAM dcmcjpls dcmdjpls dcml2pnm)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmjpls dcmtkcharls dcmimage dcmimgle dcmdata oflog ofstd ofstd)
endforeach()
