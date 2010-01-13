# declare additional include directories
INCLUDE_DIRECTORIES(${dcmjpls_SOURCE_DIR}/libcharls)

# create library from source files
ADD_LIBRARY(charls header interface jpegls)

# declare installation files
INSTALL_TARGETS(${INSTALL_LIBDIR} charls)
