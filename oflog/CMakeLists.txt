# declare project
PROJECT(oflog)

# declare include directories which hold for all subdirectories
INCLUDE_DIRECTORIES(${oflog_SOURCE_DIR}/include ${ofstd_SOURCE_DIR}/include)

# recurse into subdirectories
SUBDIRS(libsrc include/dcmtk/oflog etc)
