# declare directories containing used libraries
LINK_DIRECTORIES(${dcmimgle_BINARY_DIR} ${ofstd_BINARY_DIR} ${dcmdata_BINARY_DIR})
IF(WITH_ZLIB)
  LINK_DIRECTORIES(${WITH_ZLIBINC}/lib)
ENDIF(WITH_ZLIB)

# declare executables
ADD_EXECUTABLE(dcmdspfn dcmdspfn)
ADD_EXECUTABLE(dcod2lum dcod2lum)
ADD_EXECUTABLE(dconvlum dconvlum)

# declare link flags
SET_TARGET_PROPERTIES(dcmdspfn PROPERTIES LINK_FLAGS "/incremental:no")
SET_TARGET_PROPERTIES(dcod2lum PROPERTIES LINK_FLAGS "/incremental:no")
SET_TARGET_PROPERTIES(dconvlum PROPERTIES LINK_FLAGS "/incremental:no")

# make sure executables are linked to the corresponding libraries
# dcmdspfn
IF(WITH_ZLIB)
  TARGET_LINK_LIBRARIES(dcmdspfn ofstd dcmdata dcmimgle debug zlib_d optimized zlib_o)
ELSE(WITH_ZLIB)
  TARGET_LINK_LIBRARIES(dcmdspfn ofstd dcmdata dcmimgle)
ENDIF(WITH_ZLIB)
# dcod2lum
IF(WITH_ZLIB)
  TARGET_LINK_LIBRARIES(dcod2lum ofstd dcmimgle debug zlib_d optimized zlib_o)
ELSE(WITH_ZLIB)
  TARGET_LINK_LIBRARIES(dcod2lum ofstd dcmimgle)
ENDIF(WITH_ZLIB)
# dconvlum
IF(WITH_ZLIB)
  TARGET_LINK_LIBRARIES(dconvlum ofstd dcmimgle debug zlib_d optimized zlib_o)
ELSE(WITH_ZLIB)
  TARGET_LINK_LIBRARIES(dconvlum ofstd dcmimgle)
ENDIF(WITH_ZLIB)
