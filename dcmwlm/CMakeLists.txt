# declare project
PROJECT(dcmwlm)

# declare include directories which hold for all subdirectories
INCLUDE_DIRECTORIES(${dcmwlm_SOURCE_DIR}/include ${ofstd_SOURCE_DIR}/include ${dcmdata_SOURCE_DIR}/include ${dcmnet_SOURCE_DIR}/include)
IF(WITH_ZLIB)
  INCLUDE_DIRECTORIES(${WITH_ZLIBINC}/include)
ENDIF(WITH_ZLIB)

# recurse into subdirectories
SUBDIRS(libsrc apps)
