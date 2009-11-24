# declare project
PROJECT(dcmqrdb)

# declare include directories which hold for all subdirectories
INCLUDE_DIRECTORIES(${dcmqrdb_SOURCE_DIR}/include ${ofstd_SOURCE_DIR}/include ${oflog_SOURCE_DIR}/include ${oflog_SOURCE_DIR}/include ${dcmdata_SOURCE_DIR}/include ${dcmnet_SOURCE_DIR}/include ${ZLIB_INCDIR})

# recurse into subdirectories
SUBDIRS(libsrc apps include/dcmtk/dcmqrdb docs etc)
