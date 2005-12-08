# declare project
PROJECT(ofstd)

# declare include directories which hold for all subdirectories
INCLUDE_DIRECTORIES(${ofstd_SOURCE_DIR}/include)

# recurse into subdirectories
SUBDIRS(libsrc include/dcmtk/ofstd)
