# declare project
PROJECT(dcmsign)

# declare include directories which hold for all subdirectories
INCLUDE_DIRECTORIES(${dcmsign_SOURCE_DIR}/include ${ofstd_SOURCE_DIR}/include ${dcmdata_SOURCE_DIR}/include)
IF(WITH_ZLIB)
  INCLUDE_DIRECTORIES(${WITH_ZLIBINC}/include)
ENDIF(WITH_ZLIB)
IF(WITH_OPENSSL)
  INCLUDE_DIRECTORIES(${WITH_OPENSSLINC}/include)
ENDIF(WITH_OPENSSL)

# recurse into subdirectories
SUBDIRS(libsrc apps)
