# declare project
PROJECT(dcmsr)

# declare include directories which hold for all subdirectories
INCLUDE_DIRECTORIES(${dcmsr_SOURCE_DIR}/include ${ofstd_SOURCE_DIR}/include ${dcmdata_SOURCE_DIR}/include)
IF(WITH_ZLIB)
  INCLUDE_DIRECTORIES(${WITH_ZLIBINC}/include)
ENDIF(WITH_ZLIB)
IF(WITH_LIBXML)
  INCLUDE_DIRECTORIES(${WITH_LIBXMLINC}/include)
ENDIF(WITH_LIBXML)

# recurse into subdirectories
SUBDIRS(libsrc apps tests)
