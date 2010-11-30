IF(WIN32)

  # For Windows, we don't used FIND_PACKAGE because DCMTK usually is used with its
  # own set of 3rd-party support libraries that can be downloaded from DCMTK's
  # website (pre-built).

  # libxml support: find out whether user has library
  GET_FILENAME_COMPONENT(LIBXML_DIR "${DCMTK_SOURCE_DIR}/../libxml2-2.7.7" ABSOLUTE)
  FIND_PATH(WITH_LIBXMLINC "/include/libxml/parser.h" "${LIBXML_DIR}"/ NO_DEFAULT_PATH)

  # libpng support: find out whether user has library
  GET_FILENAME_COMPONENT(LIBPNG_DIR "${DCMTK_SOURCE_DIR}/../libpng-1.4.2" ABSOLUTE)
  FIND_PATH(WITH_LIBPNGINC "include/png.h" "${LIBPNG_DIR}" NO_DEFAULT_PATH)

  # libtiff support: find out whether user has library
  GET_FILENAME_COMPONENT(LIBTIFF_DIR "${DCMTK_SOURCE_DIR}/../tiff-3.9.2" ABSOLUTE)
  FIND_PATH(WITH_LIBTIFFINC "include/tiff.h" "${LIBTIFF_DIR}" NO_DEFAULT_PATH)

  # OpenSSL support: find out whether user has library
  GET_FILENAME_COMPONENT(OPENSSL_DIR "${DCMTK_SOURCE_DIR}/../openssl-1.0.0" ABSOLUTE)
  FIND_PATH(WITH_OPENSSLINC "include/openssl/ssl.h" "${OPENSSL_DIR}" NO_DEFAULT_PATH)

  # zlib support: find out whether user has library
  GET_FILENAME_COMPONENT(ZLIB_DIR "${DCMTK_SOURCE_DIR}/../zlib-1.2.5" ABSOLUTE)
  FIND_PATH(WITH_ZLIBINC "include/zlib.h" "${ZLIB_DIR}" NO_DEFAULT_PATH)

  # sndfile support: find out whether user has library. Needed for module dcmwave (not in public DCMTK yet, marked as advanced)
  GET_FILENAME_COMPONENT(SNDFILE_DIR "${DCMTK_SOURCE_DIR}/../libsndfile-1.0.17" ABSOLUTE)
  FIND_PATH(WITH_SNDFILEINC "sndfile.h" "${SNDFILE_DIR}" NO_DEFAULT_PATH)
  MARK_AS_ADVANCED(SNDFILE_DIR WITH_SNDFILEINC)

  # libxml support: configure compiler
  IF(DCMTK_WITH_XML)
    IF(WITH_LIBXMLINC)
      SET(LIBXML_INCDIR ${WITH_LIBXMLINC}/include)
      SET(LIBXML_LIBDIR ${WITH_LIBXMLINC}/lib)
      SET(LIBXML_LIBS debug libxml2_d optimized libxml2_o debug iconv_d optimized iconv_o)
      MESSAGE(STATUS "Info: DCMTK XML support will be enabled")
      SET(WITH_LIBXML 1)
    ELSE(WITH_LIBXMLINC) # turn off library if library path not set
      MESSAGE(STATUS "Warning: XML support will be disabled because libxml2 directory is not specified. Correct path and re-enable DCMTK_WITH_XML.")
      SET(DCMTK_WITH_XML OFF CACHE BOOL "" FORCE)
      SET(WITH_LIBXML "")
    ENDIF(WITH_LIBXMLINC)
  ENDIF(DCMTK_WITH_XML)

  # libpng support: configure compiler
  IF(DCMTK_WITH_PNG)
    IF(WITH_LIBPNGINC)
      SET(LIBPNG_INCDIR ${WITH_LIBPNGINC}/include)
      SET(LIBPNG_LIBDIR ${WITH_LIBPNGINC}/lib)
      SET(LIBPNG_LIBS debug libpng_d optimized libpng_o)
      MESSAGE(STATUS "Info: DCMTK PNG support will be enabled")
      SET(WITH_LIBPNG 1)
    ELSE(WITH_LIBPNGINC) # turn off library if library path not set
      MESSAGE(STATUS "Warning: PNG support will be disabled because libpng directory was not specified. Correct path and re-enable DCMTK_WITH_PNG.")
      SET(DCMTK_WITH_PNG OFF CACHE BOOL "" FORCE)
      SET(WITH_LIBPNG "")
    ENDIF(WITH_LIBPNGINC)
  ENDIF(DCMTK_WITH_PNG)

  # libtiff support: configure compiler
  IF(DCMTK_WITH_TIFF)
    IF (WITH_LIBTIFFINC)
      SET(LIBTIFF_INCDIR ${WITH_LIBTIFFINC}/include)
      SET(LIBTIFF_LIBDIR ${WITH_LIBTIFFINC}/lib)
      SET(LIBTIFF_LIBS debug libtiff_d optimized libtiff_o)
      MESSAGE(STATUS "Info: DCMTK TIFF support will be enabled")
      SET(WITH_LIBTIFF 1)
    ELSE(WITH_LIBTIFFINC) # turn off library if library path not set
      MESSAGE(STATUS "Warning: TIFF support will be disabled because libtiff directory was not specified. Correct path and re-enable DCMTK_WITH_TIFF.")
      SET(DCMTK_WITH_TIFF OFF CACHE BOOL "" FORCE)
      SET(WITH_LIBTIFF "")
    ENDIF(WITH_LIBTIFFINC)
  ENDIF(DCMTK_WITH_TIFF)

  # OpenSSL support: configure compiler
  IF(DCMTK_WITH_OPENSSL)
    IF(WITH_OPENSSLINC)
      SET(OPENSSL_BINDIR ${WITH_OPENSSLINC}/bin)
      SET(OPENSSL_INCDIR ${WITH_OPENSSLINC}/include)
      SET(OPENSSL_LIBDIR ${WITH_OPENSSLINC}/lib)
      SET(OPENSSL_LIBS debug dcmtkssl_d.lib optimized dcmtkssl_o.lib debug dcmtkeay_d.lib optimized dcmtkeay_o.lib)
      MESSAGE(STATUS "Info: DCMTK OPENSSL support will be enabled")
      SET(WITH_OPENSSL 1)
    ELSE(WITH_OPENSSLINC) # turn off library if library path not set
      MESSAGE(STATUS "Warning: OPENSSL support will be disabled because libopenssl directory was not specified. Correct path and re-enable DCMTK_WITH_OPENSSL.")
      SET(DCMTK_WITH_OPENSSL OFF CACHE BOOL "" FORCE)
      SET(WITH_OPENSSL "")
    ENDIF(WITH_OPENSSLINC)
  ENDIF(DCMTK_WITH_OPENSSL)

  # zlib support: configure compiler
  IF(DCMTK_WITH_ZLIB)
    IF(WITH_ZLIBINC)
      SET(ZLIB_INCDIR ${WITH_ZLIBINC}/include)
      SET(ZLIB_LIBDIR ${WITH_ZLIBINC}/lib)
      SET(ZLIB_LIBS debug zlib_d optimized zlib_o)
      MESSAGE(STATUS "Info: DCMTK ZLIB support will be enabled")
      SET(WITH_ZLIB 1)
    ELSE(WITH_ZLIBINC) # turn off library if library path not set
      MESSAGE(STATUS "Warning: ZLIB support will be disabled because zlib directory was not specified. Correct path and re-enable DCMTK_WITH_ZLIB.")
      SET(DCMTK_WITH_ZLIB OFF CACHE BOOL "" FORCE)
      SET(WITH_ZLIB "")
    ENDIF(WITH_ZLIBINC)
  ENDIF(DCMTK_WITH_ZLIB)

  # sndfile support: configure compiler
  IF(DCMTK_WITH_SNDFILE)
    IF(WITH_SNDFILEINC)
      SET(SNDFILE_INCDIR ${WITH_SNDFILEINC}/include)
      SET(SNDFILE_LIBDIR ${WITH_SNDFILEINC})
      SET(SNDFILE_LIBS debug libsndfile-1 optimized libsndfile-1)
      MESSAGE(STATUS "Info: DCMTK SNDFILE support will be enabled")
      SET(WITH_SNDFILE 1)
    ELSE(WITH_SNDFILEINC) # turn off library if library path not set
      MESSAGE(STATUS "Warning: SNDFILE support will be disabled because libsndfile was not found at the specified directory. Correct path and re-enable DCMTK_WITH_SNDFILE.")
      # SET(DCMTK_WITH_SNDFILE OFF CACHE BOOL "" FORCE)
      SET(WITH_SNDFILE "")
    ENDIF(WITH_SNDFILEINC)
  ENDIF(DCMTK_WITH_SNDFILE)

ELSE(WIN32)

  # Find TIFF
  IF(DCMTK_WITH_TIFF)
    FIND_PACKAGE(TIFF)
    INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIR})
    SET(LIBTIFF_LIBS ${TIFF_LIBRARY})
    # turn off library if it could not be found
    IF(NOT LIBTIFF_LIBS)
      MESSAGE(STATUS "Warning: TIFF support will be disabled because libtiff was not found.")
      SET(DCMTK_WITH_TIFF OFF CACHE BOOL "" FORCE)
      SET(WITH_LIBTIFF "")
    ELSE(NOT LIBTIFF_LIBS)
      MESSAGE(STATUS "Info: DCMTK TIFF support will be enabled")
      SET(WITH_LIBTIFF 1)
    ENDIF(NOT LIBTIFF_LIBS)
  ENDIF(DCMTK_WITH_TIFF)

  # Find PNG
  IF(DCMTK_WITH_PNG)
    FIND_PACKAGE(PNG)
    INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIR})
    SET(LIBPNG_LIBS ${PNG_LIBRARY})
    IF (NOT LIBPNG_LIBS)
      SET(DCMTK_WITH_PNG OFF CACHE BOOL "" FORCE)
      MESSAGE(STATUS "Warning: PNG support will be disabled because libpng was not found.")
      SET(WITH_LIBPNG "")
    ELSE(NOT LIBPNG_LIBS)
      MESSAGE(STATUS "Info: DCMTK PNG support will be enabled")
      SET(WITH_LIBPNG 1)
    ENDIF(NOT LIBPNG_LIBS)
  ENDIF(DCMTK_WITH_PNG)

  # Find OpenSSL
  IF(DCMTK_WITH_OPENSSL)
    FIND_PACKAGE(OpenSSL)
    INCLUDE_DIRECTORIES(${OPENSSL_INCLUDE_DIR})
    SET(OPENSSL_LIBS ${OPENSSL_LIBRARIES})
    IF (NOT OPENSSL_LIBS)
      MESSAGE(STATUS "Warning: OPENSSL support will be disabled because libopenssl was not found.")
      SET(WITH_OPENSSL "")
      SET(DCMTK_WITH_OPENSSL OFF CACHE BOOL "" FORCE)
    ELSE(NOT OPENSSL_LIBS)
      MESSAGE(STATUS "Info: DCMTK OPENSSL support will be enabled")
      SET(WITH_OPENSSL 1)
    ENDIF(NOT OPENSSL_LIBS)
  ENDIF(DCMTK_WITH_OPENSSL)

  # Find libXML2
  IF(DCMTK_WITH_XML)
    FIND_PACKAGE(LibXml2)
    INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR})
    SET(LIBXML_LIBS ${LIBXML2_LIBRARIES})
    IF (NOT LIBXML_LIBS)
      MESSAGE(STATUS "Warning: XML support will be disabled because libxml2 was not found.")
      SET(WITH_LIBXML "")
      SET(DCMTK_WITH_XML OFF CACHE BOOL "" FORCE)
    ELSE(NOT LIBXML_LIBS)
      MESSAGE(STATUS "Info: DCMTK XML support will be enabled")
      SET(WITH_LIBXML 1)
    ENDIF(NOT LIBXML_LIBS)
  ENDIF(DCMTK_WITH_XML)

  # Find zlib
  IF(DCMTK_WITH_ZLIB)
    FIND_PACKAGE(ZLIB)
    INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIRS})
    SET(ZLIB_LIBS ${ZLIB_LIBRARIES})
    IF (NOT ZLIB_LIBS)
      MESSAGE(STATUS "Warning: SNDFILE support will be disabled because libsndfile was not found.")
      SET(WITH_ZLIB "")
      SET(DCMTK_WITH_ZLIB OFF CACHE BOOL "" FORCE)
    ELSE(NOT ZLIB_LIBS)
      MESSAGE(STATUS "Info: DCMTK ZLIB support will be enabled")
      SET(WITH_ZLIB 1)
    ENDIF(NOT ZLIB_LIBS)
  ENDIF(DCMTK_WITH_ZLIB)

ENDIF(WIN32)

# Find doxygen
IF(DCMTK_WITH_DOXYGEN)
  FIND_PACKAGE(Doxygen) # will set variable DOXYGEN_EXECUTABLE
  IF(NOT DOXYGEN_EXECUTABLE)
    MESSAGE(STATUS "Warning: DOXYGEN support will be disabled because doxygen was not found.")
    SET(DCMTK_WITH_DOXYGEN OFF CACHE BOOL "" FORCE)
  ENDIF(NOT DOXYGEN_EXECUTABLE)
ENDIF(DCMTK_WITH_DOXYGEN)
