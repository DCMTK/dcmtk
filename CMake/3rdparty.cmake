IF(WIN32 AND NOT MINGW)

  # For Windows, we don't used FIND_PACKAGE because DCMTK usually is used with its
  # own set of 3rd-party support libraries that can be downloaded from DCMTK's
  # website (pre-built).

  # libxml support: find out whether user has library
  GET_FILENAME_COMPONENT(LIBXML_DIR "${DCMTK_SOURCE_DIR}/../libxml2-2.7.7" ABSOLUTE)
  FIND_PATH(WITH_LIBXMLINC "/include/libxml/parser.h" "${LIBXML_DIR}" NO_DEFAULT_PATH)

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

  # libiconv support: find out whether user has library
  GET_FILENAME_COMPONENT(LIBICONV_DIR "${DCMTK_SOURCE_DIR}/../libiconv-1.14" ABSOLUTE)
  FIND_PATH(WITH_LIBICONVINC "include/iconv.h" "${LIBICONV_DIR}" NO_DEFAULT_PATH)

  # libxml support: configure compiler
  IF(DCMTK_WITH_XML)
    IF(WITH_LIBXMLINC)
      SET(LIBXML_INCDIR "${WITH_LIBXMLINC}/include")
      SET(LIBXML_LIBDIR "${WITH_LIBXMLINC}/lib")
      SET(LIBXML_LIBS debug "${LIBXML_LIBDIR}/libxml2_d.lib" optimized "${LIBXML_LIBDIR}/libxml2_o.lib" debug "${LIBXML_LIBDIR}/iconv_d.lib" optimized "${LIBXML_LIBDIR}/iconv_o.lib")
      MESSAGE(STATUS "Info: DCMTK XML support will be enabled")
      SET(WITH_LIBXML 1)
      # this hides some warnings that are emitted when linking against libxmlXXX.lib instead of linking the DLL directly
      ADD_DEFINITIONS("-DLIBXML_STATIC")
    ELSE(WITH_LIBXMLINC) # turn off library if library path not set
      MESSAGE(STATUS "Warning: XML support will be disabled because libxml2 directory is not specified. Correct path and re-enable DCMTK_WITH_XML.")
      SET(DCMTK_WITH_XML OFF CACHE BOOL "" FORCE)
      SET(WITH_LIBXML "")
    ENDIF(WITH_LIBXMLINC)
  ENDIF(DCMTK_WITH_XML)

  # libpng support: configure compiler
  IF(DCMTK_WITH_PNG)
    IF(WITH_LIBPNGINC)
      SET(LIBPNG_INCDIR "${WITH_LIBPNGINC}/include")
      SET(LIBPNG_LIBDIR "${WITH_LIBPNGINC}/lib")
      SET(LIBPNG_LIBS debug "${LIBPNG_LIBDIR}/libpng_d.lib" optimized "${LIBPNG_LIBDIR}/libpng_o.lib")
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
    IF(WITH_LIBTIFFINC)
      SET(LIBTIFF_INCDIR "${WITH_LIBTIFFINC}/include")
      SET(LIBTIFF_LIBDIR "${WITH_LIBTIFFINC}/lib")
      SET(LIBTIFF_LIBS debug "${LIBTIFF_LIBDIR}/libtiff_d.lib" optimized "${LIBTIFF_LIBDIR}/libtiff_o.lib")
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
      SET(OPENSSL_BINDIR "${WITH_OPENSSLINC}/bin")
      SET(OPENSSL_INCDIR "${WITH_OPENSSLINC}/include")
      SET(OPENSSL_LIBDIR "${WITH_OPENSSLINC}/lib")
      SET(OPENSSL_LIBS debug "${OPENSSL_LIBDIR}/dcmtkssl_d.lib" optimized "${OPENSSL_LIBDIR}/dcmtkssl_o.lib" debug "${OPENSSL_LIBDIR}/dcmtkeay_d.lib" optimized "${OPENSSL_LIBDIR}/dcmtkeay_o.lib")
      MESSAGE(STATUS "Info: DCMTK OPENSSL support will be enabled")
      SET(WITH_OPENSSL 1)
    ELSE(WITH_OPENSSLINC) # turn off library if library path not set
      MESSAGE(STATUS "Warning: OPENSSL support will be disabled because openssl directory was not specified. Correct path and re-enable DCMTK_WITH_OPENSSL.")
      SET(DCMTK_WITH_OPENSSL OFF CACHE BOOL "" FORCE)
      SET(WITH_OPENSSL "")
    ENDIF(WITH_OPENSSLINC)
  ENDIF(DCMTK_WITH_OPENSSL)

  # zlib support: configure compiler
  IF(DCMTK_WITH_ZLIB)
    IF(WITH_ZLIBINC)
      SET(ZLIB_INCDIR "${WITH_ZLIBINC}/include")
      SET(ZLIB_LIBDIR "${WITH_ZLIBINC}/lib")
      SET(ZLIB_LIBS debug "${ZLIB_LIBDIR}/zlib_d.lib" optimized "${ZLIB_LIBDIR}/zlib_o.lib")
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
      SET(SNDFILE_INCDIR "${WITH_SNDFILEINC}/include")
      SET(SNDFILE_LIBDIR "${WITH_SNDFILEINC}/lib")
      SET(SNDFILE_LIBS debug "${SNDFILE_LIBDIR}/libsndfile_d.lib" optimized "${SNDFILE_LIBDIR}/libsndfile_o.lib")
      MESSAGE(STATUS "Info: DCMTK SNDFILE support will be enabled")
      SET(WITH_SNDFILE 1)
    ELSE(WITH_SNDFILEINC) # turn off library if library path not set
      MESSAGE(STATUS "Warning: SNDFILE support will be disabled because libsndfile directory was not specified. Correct path and re-enable DCMTK_WITH_SNDFILE.")
      SET(DCMTK_WITH_SNDFILE OFF CACHE BOOL "" FORCE)
      SET(WITH_SNDFILE "")
    ENDIF(WITH_SNDFILEINC)
  ENDIF(DCMTK_WITH_SNDFILE)

  # libiconv support: configure compiler
  IF(DCMTK_WITH_ICONV)
    IF(WITH_LIBICONVINC)
      SET(LIBICONV_INCDIR "${WITH_LIBICONVINC}/include")
      SET(LIBICONV_LIBDIR "${WITH_LIBICONVINC}/lib")
      SET(LIBICONV_LIBS debug "${LIBICONV_LIBDIR}/libiconv_d.lib" optimized "${LIBICONV_LIBDIR}/libiconv_o.lib")
      MESSAGE(STATUS "Info: DCMTK ICONV support will be enabled")
      SET(WITH_LIBICONV 1)
    ELSE(WITH_LIBICONVINC) # turn off library if library path not set
      MESSAGE(STATUS "Warning: ICONV support will be disabled because libiconv directory was not specified. Correct path and re-enable DCMTK_WITH_ICONV.")
      SET(DCMTK_WITH_ICONV OFF CACHE BOOL "" FORCE)
      SET(WITH_LIBICONV "")
    ENDIF(WITH_LIBICONVINC)
  ENDIF(DCMTK_WITH_ICONV)

ELSE(WIN32 AND NOT MINGW)

  # Find TIFF
  IF(DCMTK_WITH_TIFF)
    FIND_PACKAGE(TIFF QUIET)
    # turn off library if it could not be found
    IF(NOT TIFF_FOUND)
      MESSAGE(STATUS "Warning: TIFF support will be disabled because libtiff was not found.")
      SET(DCMTK_WITH_TIFF OFF CACHE BOOL "" FORCE)
      SET(WITH_LIBTIFF "")
    ELSE(NOT TIFF_FOUND)
      MESSAGE(STATUS "Info: DCMTK TIFF support will be enabled")
      SET(WITH_LIBTIFF 1)
      # libtiff can be compiled with libjpeg support; if available, add libjpeg to library and include path
      FIND_PACKAGE(JPEG QUIET)
      INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIR} ${JPEG_INCLUDE_DIR})
      SET(LIBTIFF_LIBS ${TIFF_LIBRARY} ${JPEG_LIBRARY})
    ENDIF(NOT TIFF_FOUND)
  ENDIF(DCMTK_WITH_TIFF)

  # Find PNG
  IF(DCMTK_WITH_PNG)
    FIND_PACKAGE(PNG QUIET)
    IF(NOT PNG_FOUND)
      SET(DCMTK_WITH_PNG OFF CACHE BOOL "" FORCE)
      MESSAGE(STATUS "Warning: PNG support will be disabled because libpng was not found.")
      SET(WITH_LIBPNG "")
    ELSE(NOT PNG_FOUND)
      MESSAGE(STATUS "Info: DCMTK PNG support will be enabled")
      SET(WITH_LIBPNG 1)
      INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIR})
      SET(LIBPNG_LIBS ${PNG_LIBRARY})
    ENDIF(NOT PNG_FOUND)
  ENDIF(DCMTK_WITH_PNG)

  # Find OpenSSL
  IF(DCMTK_WITH_OPENSSL)
    FIND_PACKAGE(OpenSSL QUIET)
    IF(NOT OPENSSL_FOUND)
      MESSAGE(STATUS "Warning: OPENSSL support will be disabled because openssl was not found.")
      SET(WITH_OPENSSL "")
      SET(DCMTK_WITH_OPENSSL OFF CACHE BOOL "" FORCE)
    ELSE(NOT OPENSSL_FOUND)
      MESSAGE(STATUS "Info: DCMTK OPENSSL support will be enabled")
      SET(WITH_OPENSSL 1)
      INCLUDE_DIRECTORIES(${OPENSSL_INCLUDE_DIR})
      SET(OPENSSL_LIBS ${OPENSSL_LIBRARIES})
    ENDIF(NOT OPENSSL_FOUND)
  ENDIF(DCMTK_WITH_OPENSSL)

  # Find libXML2
  IF(DCMTK_WITH_XML)
    FIND_PACKAGE(LibXml2 QUIET)
    IF(NOT LIBXML2_FOUND)
      MESSAGE(STATUS "Warning: XML support will be disabled because libxml2 was not found.")
      SET(WITH_LIBXML "")
      SET(DCMTK_WITH_XML OFF CACHE BOOL "" FORCE)
    ELSE(NOT LIBXML2_FOUND)
      MESSAGE(STATUS "Info: DCMTK XML support will be enabled")
      SET(WITH_LIBXML 1)
      INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR})
      SET(LIBXML_LIBS ${LIBXML2_LIBRARIES})
    ENDIF(NOT LIBXML2_FOUND)
  ENDIF(DCMTK_WITH_XML)

  # Find zlib
  IF(DCMTK_WITH_ZLIB)
    FIND_PACKAGE(ZLIB QUIET)
    IF(NOT ZLIB_FOUND)
      MESSAGE(STATUS "Warning: ZLIB support will be disabled because zlib was not found.")
      SET(WITH_ZLIB "")
      SET(DCMTK_WITH_ZLIB OFF CACHE BOOL "" FORCE)
    ELSE(NOT ZLIB_FOUND)
      MESSAGE(STATUS "Info: DCMTK ZLIB support will be enabled")
      SET(WITH_ZLIB 1)
      INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIRS})
      SET(ZLIB_LIBS ${ZLIB_LIBRARIES})
    ENDIF(NOT ZLIB_FOUND)
  ENDIF(DCMTK_WITH_ZLIB)

  # Find libsndfile
  IF(DCMTK_WITH_SNDFILE)
    FIND_PACKAGE(Sndfile QUIET)
    IF(NOT SNDFILE_LIBS)
      MESSAGE(STATUS "Warning: SNDFILE support will be disabled because libsndfile was not found.")
      SET(WITH_SNDFILE "")
      SET(DCMTK_WITH_SNDFILE OFF CACHE BOOL "" FORCE)
    ELSE(NOT SNDFILE_LIBS)
      MESSAGE(STATUS "Info: DCMTK SNDFILE support will be enabled")
      SET(WITH_SNDFILE 1)
      INCLUDE_DIRECTORIES(${SNDFILE_INCLUDE_DIRS})
      SET(SNDFILE_LIBS ${SNDFILE_LIBRARIES})
    ENDIF(NOT SNDFILE_LIBS)
  ENDIF(DCMTK_WITH_SNDFILE)

  # Find libiconv
  IF(DCMTK_WITH_ICONV)
    FIND_PACKAGE(ICONV QUIET)
    FIND_PACKAGE(Charset QUIET)
    IF(NOT LIBICONV_FOUND OR NOT LIBCHARSET_FOUND)
      MESSAGE(STATUS "Warning: ICONV support will be disabled because libiconv was not found. Correct LIBICONV_LIBDIR and LIBICONV_INCLUDE_DIR and re-enable DCMTK_WITH_ICONV.")
      SET(DCMTK_WITH_ICONV OFF CACHE BOOL "" FORCE)
      SET(WITH_LIBICONV "")
    ELSE(NOT LIBICONV_FOUND OR NOT LIBCHARSET_FOUND)
      MESSAGE(STATUS "Info: DCMTK ICONV support will be enabled")
      SET(WITH_LIBICONV 1)
      SET(LIBICONV_INCDIR ${LIBICONV_INCLUDE_DIRS} ${LIBCHARSET_INCLUDE_DIRS})
      SET(LIBICONV_LIBDIR ${LIBICONV_LIBDIR})
      SET(LIBICONV_LIBS ${LIBICONV_LIBRARIES} ${LIBCHARSET_LIBRARY})
      INCLUDE_DIRECTORIES(${LIBICONV_INCDIR})
    ENDIF(NOT LIBICONV_FOUND OR NOT LIBCHARSET_FOUND)
  ENDIF(DCMTK_WITH_ICONV)

  # Find libwrap
  IF(DCMTK_WITH_WRAP)
    FIND_PACKAGE(Wrap QUIET)
    IF(NOT WRAP_FOUND)
      MESSAGE(STATUS "Warning: WRAP support will be disabled because libwrap was not found.")
      SET(WITH_TCPWRAPPER "")
      SET(DCMTK_WITH_WRAP OFF CACHE BOOL "" FORCE)
    ELSE(NOT WRAP_FOUND)
      MESSAGE(STATUS "Info: DCMTK WRAP support will be enabled")
      SET(WITH_TCPWRAPPER 1)
      INCLUDE_DIRECTORIES(${WRAP_INCLUDE_DIRS})
      SET(WRAP_LIBS ${WRAP_LIBRARIES})
    ENDIF(NOT WRAP_FOUND)
  ENDIF(DCMTK_WITH_WRAP)

ENDIF()

IF(NOT DEFINED DCMTK_WITH_STDLIBC_ICONV)
  INCLUDE(CheckCXXSourceCompiles)
  CHECK_CXX_SOURCE_COMPILES("#include <iconv.h>\nint main(){iconv_t cd = iconv_open(\"\",\"\");iconv(cd,0,0,0,0);iconv_close(cd);return 0;}" WITH_STDLIBC_ICONV)
  IF(WITH_STDLIBC_ICONV)
    MESSAGE(STATUS "Info: found builtin ICONV support inside the C standard library.")
    SET(DCMTK_WITH_STDLIBC_ICONV ON CACHE BOOL "" FORCE)
  ELSE(WITH_STDLIBC_ICONV)
    SET(DCMTK_WITH_STDLIBC_ICONV OFF CACHE BOOL "" FORCE)
  ENDIF(WITH_STDLIBC_ICONV)
ENDIF(NOT DEFINED DCMTK_WITH_STDLIBC_ICONV)

IF(DCMTK_WITH_ICU)
  FIND_PACKAGE(ICU COMPONENTS uc data)
  IF(NOT ICU_FOUND)
    MESSAGE(STATUS "Warning: ICU support will be disabled because the ICU were not found.")
    SET(DCMTK_WITH_ICU OFF CACHE BOOL "" FORCE)
    SET(WITH_ICU "")
  ELSE(NOT ICU_FOUND)
    MESSAGE(STATUS "Info: DCMTK ICU support will be enabled")
    SET(WITH_ICU 1)
    SET(ICU_INCDIR ${ICU_INCLUDE_DIR})
    SET(ICU_LIBS ${ICU_LIBRARIES})
    INCLUDE_DIRECTORIES(${ICU_INCLUDE_DIR})
  ENDIF(NOT ICU_FOUND)
ENDIF(DCMTK_WITH_ICU)

# Find doxygen
IF(DCMTK_WITH_DOXYGEN)
  FIND_PACKAGE(Doxygen QUIET) # will set variable DOXYGEN_EXECUTABLE
  IF(NOT DOXYGEN_FOUND)
    MESSAGE(STATUS "Warning: DOXYGEN support will be disabled because doxygen was not found.")
    SET(DCMTK_WITH_DOXYGEN OFF CACHE BOOL "" FORCE)
  ENDIF(NOT DOXYGEN_FOUND)
ENDIF(DCMTK_WITH_DOXYGEN)

IF(DCMTK_WITH_ICONV OR DCMTK_WITH_STDLIBC_ICONV)
  INCLUDE(CheckCXXSourceCompiles)
  SET(CMAKE_REQUIRED_INCLUDES ${LIBICONV_INCDIR})
  SET(CMAKE_REQUIRED_LIBRARIES ${LIBICONV_LIBS})
  CHECK_CXX_SOURCE_COMPILES("
  #include <iconv.h>
  int main() {
    iconv_t cd = 0;
    const char *in = 0;
    iconv(cd, &in, 0, 0, 0);
    return 0;
  }" LIBICONV_SECOND_ARGUMENT_CONST)
  SET(CMAKE_REQUIRED_INCLUDES)
  SET(CMAKE_REQUIRED_LIBRARIES)
ENDIF(DCMTK_WITH_ICONV OR DCMTK_WITH_STDLIBC_ICONV)
