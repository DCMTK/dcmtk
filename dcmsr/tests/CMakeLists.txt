# declare directories containing used libraries
LINK_DIRECTORIES(${dcmsr_BINARY_DIR} ${ofstd_BINARY_DIR} ${dcmdata_BINARY_DIR})
IF(WITH_ZLIB)
  LINK_DIRECTORIES(${WITH_ZLIBINC}/lib)
ENDIF(WITH_ZLIB)
IF(WITH_LIBXML)
  LINK_DIRECTORIES(${WITH_LIBXMLINC}/lib)
ENDIF(WITH_LIBXML)

# declare executables
ADD_EXECUTABLE(mkreport mkreport)

# make sure executables are linked to the corresponding libraries
# mkreport
IF(WITH_ZLIB)
  IF(WITH_LIBXML)
    TARGET_LINK_LIBRARIES(mkreport netapi32 wsock32 ofstd dcmdata dcmsr debug zlib_d optimized zlib_o debug libxml2_d optimized libxml2_o debug iconv_d optimized iconv_o)
  ELSE(WITH_LIBXML)
    TARGET_LINK_LIBRARIES(mkreport netapi32 wsock32 ofstd dcmdata dcmsr debug zlib_d optimized zlib_o)
  ENDIF(WITH_LIBXML)
ELSE(WITH_ZLIB)
  IF(WITH_LIBXML)
    TARGET_LINK_LIBRARIES(mkreport netapi32 wsock32 ofstd dcmdata dcmsr debug libxml2_d optimized libxml2_o debug iconv_d optimized iconv_o)
  ELSE(WITH_LIBXML)
    TARGET_LINK_LIBRARIES(mkreport netapi32 wsock32 ofstd dcmdata dcmsr)
  ENDIF(WITH_LIBXML)
ENDIF(WITH_ZLIB)
