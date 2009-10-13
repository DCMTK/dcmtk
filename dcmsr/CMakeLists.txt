# declare project
PROJECT(dcmsr)

# declare include directories which hold for all subdirectories
INCLUDE_DIRECTORIES(${dcmsr_SOURCE_DIR}/include ${ofstd_SOURCE_DIR}/include ${oflog_SOURCE_DIR}/include ${dcmdata_SOURCE_DIR}/include ${ZLIB_INCDIR} ${LIBXML_INCDIR})

# recurse into subdirectories
SUBDIRS(libsrc apps tests include/dcmtk/dcmsr data)
