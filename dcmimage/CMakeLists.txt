# declare project
PROJECT(dcmimage)

# declare include directories which hold for all subdirectories
INCLUDE_DIRECTORIES(${dcmimage_SOURCE_DIR}/include ${ofstd_SOURCE_DIR}/include ${dcmdata_SOURCE_DIR}/include ${dcmimgle_SOURCE_DIR}/include)
IF(WITH_ZLIB)
  INCLUDE_DIRECTORIES(${WITH_ZLIBINC}/include)
ENDIF(WITH_ZLIB)
IF(WITH_LIBTIFF)
  INCLUDE_DIRECTORIES(${WITH_LIBTIFFINC}/include)
ENDIF(WITH_LIBTIFF)
IF(WITH_LIBPNG)
  INCLUDE_DIRECTORIES(${WITH_LIBPNGINC}/include)
ENDIF(WITH_LIBPNG)

# recurse into subdirectories
SUBDIRS(libsrc apps)
