# declare directories containing used libraries
LINK_DIRECTORIES(${imagectn_BINARY_DIR} ${ofstd_BINARY_DIR} ${dcmdata_BINARY_DIR} ${dcmnet_BINARY_DIR})
IF(WITH_ZLIB)
  LINK_DIRECTORIES(${WITH_ZLIBINC}/lib)
ENDIF(WITH_ZLIB)

# declare executables
ADD_EXECUTABLE(dcmqrdb dcmqrdb)
ADD_EXECUTABLE(dcmqridx dcmqridx)
ADD_EXECUTABLE(dcmqrti dcmqrti)

# make sure executables are linked to the corresponding libraries
# imagectn
IF(WITH_ZLIB)
  TARGET_LINK_LIBRARIES(dcmqrdb netapi32 wsock32 ofstd dcmdata dcmnet dcmqrdb debug zlib_d optimized zlib_o)
ELSE(WITH_ZLIB)
  TARGET_LINK_LIBRARIES(dcmqrdb netapi32 wsock32 ofstd dcmdata dcmnet dcmqrdb)
ENDIF(WITH_ZLIB)
# ti
IF(WITH_ZLIB)
  TARGET_LINK_LIBRARIES(dcmqrti netapi32 wsock32 ofstd dcmdata dcmnet dcmqrdb debug zlib_d optimized zlib_o)
ELSE(WITH_ZLIB)
  TARGET_LINK_LIBRARIES(dcmqrti netapi32 wsock32 ofstd dcmdata dcmnet dcmqrdb)
ENDIF(WITH_ZLIB)
# dbregimg
IF(WITH_ZLIB)
  TARGET_LINK_LIBRARIES(dcmqridx netapi32 wsock32 ofstd dcmdata dcmnet dcmqrdb debug zlib_d optimized zlib_o)
ELSE(WITH_ZLIB)
  TARGET_LINK_LIBRARIES(dcmqridx netapi32 wsock32 ofstd dcmdata dcmnet dcmqrdb)
ENDIF(WITH_ZLIB)
