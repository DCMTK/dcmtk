# declare additional include directories
INCLUDE_DIRECTORIES(${dcmjpls_SOURCE_DIR}/libcharls ${ofstd_SOURCE_DIR}/include )

# create library from source files
ADD_LIBRARY(charls header intrface jpegls)

# declare installation files
INSTALL_TARGETS(${INSTALL_LIBDIR} charls)
