if(WIN32 AND NOT MINGW)

  # For Windows, we don't used FIND_PACKAGE because DCMTK usually is used with its
  # own set of 3rd-party support libraries that can be downloaded from DCMTK's
  # website (pre-built).

  # libxml support: find out whether user has library
  file(GLOB LIBXML_DIR "${DCMTK_SOURCE_DIR}/../libxml2*")
  find_path(WITH_LIBXMLINC "/include/libxml/parser.h" "${LIBXML_DIR}" NO_DEFAULT_PATH)

  # libpng support: find out whether user has library
  file(GLOB LIBPNG_DIR "${DCMTK_SOURCE_DIR}/../libpng*")
  find_path(WITH_LIBPNGINC "include/png.h" "${LIBPNG_DIR}" NO_DEFAULT_PATH)

  # libtiff support: find out whether user has library
  file(GLOB LIBTIFF_DIR "${DCMTK_SOURCE_DIR}/../libtiff*")
  find_path(WITH_LIBTIFFINC "include/tiff.h" "${LIBTIFF_DIR}" NO_DEFAULT_PATH)

  # OpenSSL support: find out whether user has library
  file(GLOB OPENSSL_DIR "${DCMTK_SOURCE_DIR}/../openssl*")
  find_path(WITH_OPENSSLINC "include/openssl/ssl.h" "${OPENSSL_DIR}" NO_DEFAULT_PATH)

  # zlib support: find out whether user has library
  file(GLOB ZLIB_DIR "${DCMTK_SOURCE_DIR}/../zlib*")
  find_path(WITH_ZLIBINC "include/zlib.h" "${ZLIB_DIR}" NO_DEFAULT_PATH)

  # sndfile support: find out whether user has library. Needed for module dcmwave (not in public DCMTK yet, marked as advanced)
  file(GLOB SNDFILE_DIR "${DCMTK_SOURCE_DIR}/../libsndfile*")
  find_path(WITH_SNDFILEINC "sndfile.h" "${SNDFILE_DIR}" NO_DEFAULT_PATH)
  mark_as_advanced(SNDFILE_DIR WITH_SNDFILEINC)

  # libiconv support: find out whether user has library
  file(GLOB LIBICONV_DIR "${DCMTK_SOURCE_DIR}/../libiconv*")
  find_path(WITH_LIBICONVINC "include/iconv.h" "${LIBICONV_DIR}" NO_DEFAULT_PATH)

  # OpenJPEG support: find out whether user has library
  file(GLOB OPENJPEG_DIR "${DCMTK_SOURCE_DIR}/../openjpeg*")
  find_path(WITH_OPENJPEGINC "lib/openjp2_o.lib" "${OPENJPEG_DIR}" NO_DEFAULT_PATH)

  # libxml support: configure compiler
  if(DCMTK_WITH_XML)
    if(WITH_LIBXMLINC)
      set(LIBXML_INCDIR "${WITH_LIBXMLINC}/include")
      set(LIBXML_LIBDIR "${WITH_LIBXMLINC}/lib")
      set(LIBXML_LIBS debug "${LIBXML_LIBDIR}/libxml2_d.lib" optimized "${LIBXML_LIBDIR}/libxml2_o.lib" debug "${LIBXML_LIBDIR}/iconv_d.lib" optimized "${LIBXML_LIBDIR}/iconv_o.lib")
      message(STATUS "Info: DCMTK XML support will be enabled")
      set(WITH_LIBXML 1)
      # this hides some warnings that are emitted when linking against libxmlXXX.lib instead of linking the DLL directly
      add_definitions("-DLIBXML_STATIC")
    else() # turn off library if library path not set
      message(STATUS "Warning: XML support will be disabled because libxml2 directory is not specified. Correct path and re-enable DCMTK_WITH_XML.")
      set(DCMTK_WITH_XML OFF CACHE BOOL "" FORCE)
      set(WITH_LIBXML "")
    endif()
  endif()

  # libpng support: configure compiler
  if(DCMTK_WITH_PNG)
    if(WITH_LIBPNGINC)
      set(LIBPNG_INCDIR "${WITH_LIBPNGINC}/include")
      set(LIBPNG_LIBDIR "${WITH_LIBPNGINC}/lib")
      set(LIBPNG_LIBS debug "${LIBPNG_LIBDIR}/libpng_d.lib" optimized "${LIBPNG_LIBDIR}/libpng_o.lib")
      message(STATUS "Info: DCMTK PNG support will be enabled")
      set(WITH_LIBPNG 1)
    else() # turn off library if library path not set
      message(STATUS "Warning: PNG support will be disabled because libpng directory was not specified. Correct path and re-enable DCMTK_WITH_PNG.")
      set(DCMTK_WITH_PNG OFF CACHE BOOL "" FORCE)
      set(WITH_LIBPNG "")
    endif()
  endif()

  # libtiff support: configure compiler
  if(DCMTK_WITH_TIFF)
    if(WITH_LIBTIFFINC)
      set(LIBTIFF_INCDIR "${WITH_LIBTIFFINC}/include")
      set(LIBTIFF_LIBDIR "${WITH_LIBTIFFINC}/lib")
      set(LIBTIFF_LIBS debug "${LIBTIFF_LIBDIR}/libtiff_d.lib" optimized "${LIBTIFF_LIBDIR}/libtiff_o.lib")
      message(STATUS "Info: DCMTK TIFF support will be enabled")
      set(WITH_LIBTIFF 1)
    else() # turn off library if library path not set
      message(STATUS "Warning: TIFF support will be disabled because libtiff directory was not specified. Correct path and re-enable DCMTK_WITH_TIFF.")
      set(DCMTK_WITH_TIFF OFF CACHE BOOL "" FORCE)
      set(WITH_LIBTIFF "")
    endif()
  endif()

  # OpenSSL support: configure compiler
  if(DCMTK_WITH_OPENSSL)
    if(WITH_OPENSSLINC)
      include(CheckCXXSourceCompiles)
      set(OPENSSL_BINDIR "${WITH_OPENSSLINC}/bin")
      set(OPENSSL_INCDIR "${WITH_OPENSSLINC}/include")
      set(OPENSSL_LIBDIR "${WITH_OPENSSLINC}/lib")
      # starting with OpenSSL 1.1.0, the Windows crypt32 library is needed for a static link of OpenSSL.
      set(OPENSSL_LIBS "crypt32" debug "${OPENSSL_LIBDIR}/dcmtkssl_d.lib" optimized "${OPENSSL_LIBDIR}/dcmtkssl_o.lib" debug "${OPENSSL_LIBDIR}/dcmtkcrypto_d.lib" optimized "${OPENSSL_LIBDIR}/dcmtkcrypto_o.lib")
      set(TEMP_INCLUDES "${CMAKE_REQUIRED_INCLUDES}")
      list(APPEND CMAKE_REQUIRED_INCLUDES "${OPENSSL_INCDIR}")
      CHECK_CXX_SOURCE_COMPILES("extern \"C\" {\n#include <openssl/ssl.h>\n}\nint main(){\n#if OPENSSL_VERSION_NUMBER < 0x10001000L\n#error OpenSSL too old\n#endif\n}\n" OPENSSL_VERSION_CHECK)
      set(CMAKE_REQUIRED_INCLUDES "${TEMP_INCLUDES}")
      if(OPENSSL_VERSION_CHECK)
        message(STATUS "Info: DCMTK OPENSSL support will be enabled")
        set(WITH_OPENSSL 1)
      else()
        message(STATUS "Info: DCMTK OPENSSL support will be disabled: DCMTK requires OpenSSL version 1.0.1 or newer")
        set(DCMTK_WITH_OPENSSL OFF CACHE BOOL "" FORCE)
        set(WITH_OPENSSL "")
      endif()
    else() # turn off library if library path not set
      message(STATUS "Warning: OPENSSL support will be disabled because openssl directory was not specified. Correct path and re-enable DCMTK_WITH_OPENSSL.")
      set(DCMTK_WITH_OPENSSL OFF CACHE BOOL "" FORCE)
      set(WITH_OPENSSL "")
    endif()
  endif()

  # zlib support: configure compiler
  if(DCMTK_WITH_ZLIB)
    if(WITH_ZLIBINC)
      set(ZLIB_INCDIR "${WITH_ZLIBINC}/include")
      set(ZLIB_LIBDIR "${WITH_ZLIBINC}/lib")
      set(ZLIB_LIBS debug "${ZLIB_LIBDIR}/zlib_d.lib" optimized "${ZLIB_LIBDIR}/zlib_o.lib")
      message(STATUS "Info: DCMTK ZLIB support will be enabled")
      set(WITH_ZLIB 1)
    else() # turn off library if library path not set
      message(STATUS "Warning: ZLIB support will be disabled because zlib directory was not specified. Correct path and re-enable DCMTK_WITH_ZLIB.")
      set(DCMTK_WITH_ZLIB OFF CACHE BOOL "" FORCE)
      set(WITH_ZLIB "")
    endif()
  endif()

  # sndfile support: configure compiler
  if(DCMTK_WITH_SNDFILE)
    if(WITH_SNDFILEINC)
      set(SNDFILE_INCDIR "${WITH_SNDFILEINC}/include")
      set(SNDFILE_LIBDIR "${WITH_SNDFILEINC}/lib")
      set(SNDFILE_LIBS debug "${SNDFILE_LIBDIR}/libsndfile_d.lib" optimized "${SNDFILE_LIBDIR}/libsndfile_o.lib")
      message(STATUS "Info: DCMTK SNDFILE support will be enabled")
      set(WITH_SNDFILE 1)
    else() # turn off library if library path not set
      message(STATUS "Warning: SNDFILE support will be disabled because libsndfile directory was not specified. Correct path and re-enable DCMTK_WITH_SNDFILE.")
      set(DCMTK_WITH_SNDFILE OFF CACHE BOOL "" FORCE)
      set(WITH_SNDFILE "")
    endif()
  endif()

  # libiconv support: configure compiler
  if(DCMTK_WITH_ICONV)
    if(WITH_LIBICONVINC)
      set(LIBICONV_INCDIR "${WITH_LIBICONVINC}/include")
      set(LIBICONV_LIBDIR "${WITH_LIBICONVINC}/lib")
      set(LIBICONV_LIBS debug "${LIBICONV_LIBDIR}/libiconv_d.lib" optimized "${LIBICONV_LIBDIR}/libiconv_o.lib")
      message(STATUS "Info: DCMTK ICONV support will be enabled")
      set(WITH_LIBICONV 1)
    else() # turn off library if library path not set
      message(STATUS "Warning: ICONV support will be disabled because libiconv directory was not specified. Correct path and re-enable DCMTK_WITH_ICONV.")
      set(DCMTK_WITH_ICONV OFF CACHE BOOL "" FORCE)
      set(WITH_LIBICONV "")
    endif()
  endif()

  # OpenJPEG support: configure compiler
  if(DCMTK_WITH_OPENJPEG)
    if(WITH_OPENJPEGINC)
      # Unfortunately, OpenJPEG uses a version number in the include path. This needs special handling.
      file(GLOB OPENJPEG2_DIR "${WITH_OPENJPEGINC}/include/openjpeg*")
      find_path(WITH_OPENJPEGINC1 "openjpeg.h" "${OPENJPEG2_DIR}" NO_DEFAULT_PATH)
      if ("${WITH_OPENJPEGINC1}" STREQUAL "WITH_OPENJPEGINC1-NOTFOUND")
          message(STATUS "Info: DCMTK OpenJPEG support will be disabled because the header files were not found.")
          set(DCMTK_WITH_OPENJPEG OFF CACHE BOOL "" FORCE)
          set(WITH_OPENJPEG "")
      else()
          set(OPENJPEG_INCDIR "${WITH_OPENJPEGINC1}")
          set(OPENJPEG_LIBDIR "${WITH_OPENJPEGINC}/lib")
          set(OPENJPEG_LIBS debug "${OPENJPEG_LIBDIR}/openjp2_d.lib" optimized "${OPENJPEG_LIBDIR}/openjp2_o.lib")
          message(STATUS "Info: DCMTK OpenJPEG support will be enabled")
          set(WITH_OPENJPEG 1)
      endif()
    else() # turn off library if library path not set
      message(STATUS "Warning: OpenJPEG support will be disabled because openjpeg directory was not specified. Correct path and re-enable DCMTK_WITH_OPENJPEG.")
      set(DCMTK_WITH_OPENJPEG OFF CACHE BOOL "" FORCE)
      set(WITH_OPENJPEG "")
    endif()
  endif()

else(WIN32 AND NOT MINGW)

  # Find TIFF
  if(DCMTK_WITH_TIFF)
    find_package(TIFF QUIET)
    # turn off library if it could not be found
    if(NOT TIFF_FOUND)
      message(STATUS "Warning: TIFF support will be disabled because libtiff was not found.")
      set(DCMTK_WITH_TIFF OFF CACHE BOOL "" FORCE)
      set(WITH_LIBTIFF "")
    else()
      set(WITH_LIBTIFF 1)
      # libtiff can be compiled with libjpeg support; if available, add libjpeg to library and include path
      find_package(JPEG QUIET)
      if(NOT JPEG_FOUND)
        message(STATUS "Info: DCMTK TIFF support will be enabled (but without JPEG)")
        include_directories(${TIFF_INCLUDE_DIR})
      else()
        message(STATUS "Info: DCMTK TIFF support will be enabled")
        include_directories(${TIFF_INCLUDE_DIR} ${JPEG_INCLUDE_DIR})
      endif()
      set(LIBTIFF_LIBS ${TIFF_LIBRARY} ${JPEG_LIBRARY})
    endif()
  endif()

  # Find PNG
  if(DCMTK_WITH_PNG)
    find_package(PNG QUIET)
    if(NOT PNG_FOUND)
      set(DCMTK_WITH_PNG OFF CACHE BOOL "" FORCE)
      message(STATUS "Warning: PNG support will be disabled because libpng was not found.")
      set(WITH_LIBPNG "")
    else()
      message(STATUS "Info: DCMTK PNG support will be enabled")
      set(WITH_LIBPNG 1)
      include_directories(${PNG_INCLUDE_DIR})
      set(LIBPNG_LIBS ${PNG_LIBRARY})
    endif()
  endif()

  # Find OpenSSL
  if(DCMTK_WITH_OPENSSL)
    find_package(OpenSSL QUIET)
    if(NOT OPENSSL_FOUND)
      message(STATUS "Warning: OPENSSL support will be disabled because openssl was not found.")
      set(WITH_OPENSSL "")
      set(DCMTK_WITH_OPENSSL OFF CACHE BOOL "" FORCE)
    else()
      include(CheckLibraryExists)
      include(CheckCXXSourceCompiles)
      CHECK_CXX_SOURCE_COMPILES("extern \"C\" {\n#include <openssl/ssl.h>\n}\nint main(){\n#if OPENSSL_VERSION_NUMBER < 0x10001000L\n#error OpenSSL too old\n#endif\n}\n" OPENSSL_VERSION_CHECK)
      if(OPENSSL_VERSION_CHECK)
        message(STATUS "Info: DCMTK OPENSSL support will be enabled")
        set(WITH_OPENSSL 1)
        include_directories(${OPENSSL_INCLUDE_DIR})
        set(OPENSSL_LIBS ${OPENSSL_LIBRARIES})
        CHECK_LIBRARY_EXISTS(dl dlopen "" HAVE_LIBDL)
        if(HAVE_LIBDL)
          set(OPENSSL_LIBS ${OPENSSL_LIBS} dl)
        endif()
      else()
        message(STATUS "Info: DCMTK OPENSSL support will be disabled: DCMTK requires OpenSSL version 1.0.1 or newer")
        set(DCMTK_WITH_OPENSSL OFF CACHE BOOL "" FORCE)
        set(WITH_OPENSSL "")
      endif()
    endif()
  endif()

  # Find libXML2
  if(DCMTK_WITH_XML)
    find_package(LibXml2 QUIET)
    if(NOT LIBXML2_FOUND)
      message(STATUS "Warning: XML support will be disabled because libxml2 was not found.")
      set(WITH_LIBXML "")
      set(DCMTK_WITH_XML OFF CACHE BOOL "" FORCE)
    else()
      message(STATUS "Info: DCMTK XML support will be enabled")
      set(WITH_LIBXML 1)
      include_directories(${LIBXML2_INCLUDE_DIR})
      set(LIBXML_LIBS ${LIBXML2_LIBRARIES})
    endif()
  endif()

  # Find zlib
  if(DCMTK_WITH_ZLIB)
    find_package(ZLIB QUIET)
    if(NOT ZLIB_FOUND)
      message(STATUS "Warning: ZLIB support will be disabled because zlib was not found.")
      set(WITH_ZLIB "")
      set(DCMTK_WITH_ZLIB OFF CACHE BOOL "" FORCE)
    else()
      message(STATUS "Info: DCMTK ZLIB support will be enabled")
      set(WITH_ZLIB 1)
      include_directories(${ZLIB_INCLUDE_DIRS})
      set(ZLIB_LIBS ${ZLIB_LIBRARIES})
    endif()
  endif()

  # Find libsndfile
  if(DCMTK_WITH_SNDFILE)
    find_package(Sndfile QUIET)
    if(NOT SNDFILE_LIBS)
      message(STATUS "Warning: SNDFILE support will be disabled because libsndfile was not found.")
      set(WITH_SNDFILE "")
      set(DCMTK_WITH_SNDFILE OFF CACHE BOOL "" FORCE)
    else()
      message(STATUS "Info: DCMTK SNDFILE support will be enabled")
      set(WITH_SNDFILE 1)
      include_directories(${SNDFILE_INCLUDE_DIRS})
      set(SNDFILE_LIBS ${SNDFILE_LIBRARIES})
    endif()
  endif()

  # Find libiconv
  if(DCMTK_WITH_ICONV)
    find_package(Iconv QUIET)
    find_package(Charset QUIET)
    if(ICONV_FOUND)
        if(NOT Iconv_IS_BUILT_IN)
            set(LIBICONV_FOUND ${ICONV_FOUND})
        else()
            message(STATUS "Info: found builtin ICONV support inside the C standard library.")
            set(DCMTK_WITH_STDLIBC_ICONV ON CACHE BOOL "" FORCE)
        endif()
        if(DEFINED ICONV_SECOND_ARGUMENT_IS_CONST)
            get_property(HELPSTRING CACHE ICONV_SECOND_ARGUMENT_IS_CONST PROPERTY HELPSTRING)
            set(LIBICONV_SECOND_ARGUMENT_CONST ${ICONV_SECOND_ARGUMENT_IS_CONST} CACHE INTERNAL "${HELPSTRING}")
        endif()
    endif()
    if(NOT LIBICONV_FOUND OR NOT LIBCHARSET_FOUND)
      message(STATUS "Warning: ICONV support will be disabled because libiconv was not found. Correct LIBICONV_LIBDIR and LIBICONV_INCLUDE_DIR and re-enable DCMTK_WITH_ICONV.")
      set(DCMTK_WITH_ICONV OFF CACHE BOOL "" FORCE)
      set(WITH_LIBICONV "")
    else()
      message(STATUS "Info: DCMTK ICONV support will be enabled")
      set(WITH_LIBICONV 1)
      set(LIBICONV_INCDIR ${LIBICONV_INCLUDE_DIRS} ${Iconv_INCLUDE_DIRS} ${ICONV_INCLUDE_DIR} ${LIBCHARSET_INCLUDE_DIRS})
      set(LIBICONV_LIBDIR ${LIBICONV_LIBDIR})
      set(LIBICONV_LIBS ${LIBICONV_LIBRARIES} ${Iconv_LIBRARIES} ${ICONV_LIBRARIES} ${LIBCHARSET_LIBRARY})
      include_directories(${LIBICONV_INCDIR})
    endif()
  endif()

  # Find libwrap
  if(DCMTK_WITH_WRAP)
    find_package(Wrap QUIET)
    if(NOT WRAP_FOUND)
      message(STATUS "Warning: WRAP support will be disabled because libwrap was not found.")
      set(WITH_TCPWRAPPER "")
      set(DCMTK_WITH_WRAP OFF CACHE BOOL "" FORCE)
    else()
      message(STATUS "Info: DCMTK WRAP support will be enabled")
      set(WITH_TCPWRAPPER 1)
      include_directories(${WRAP_INCLUDE_DIRS})
      set(WRAP_LIBS ${WRAP_LIBRARIES})
    endif()
  endif()

  # Find OpenJPEG
  if(DCMTK_WITH_OPENJPEG)
    find_package(OpenJPEG QUIET)
    if(NOT OPENJPEG_FOUND)
      message(STATUS "Warning: OpenJPEG support will be disabled because the OpenJPEG library was not found.")
      set(WITH_OPENJPEG "")
      set(DCMTK_WITH_OPENJPEG OFF CACHE BOOL "" FORCE)
    else()
      message(STATUS "Info: DCMTK OpenJPEG support will be enabled")
      set(WITH_OPENJPEG 1)
      include_directories(${OPENJPEG_INCLUDE_DIRS})
      set(OPENJPEG_LIBS ${OPENJPEG_LIBRARIES})
    endif()
  endif()

endif(WIN32 AND NOT MINGW)

if(NOT DEFINED DCMTK_WITH_STDLIBC_ICONV)
  include(CheckCXXSourceCompiles)
  CHECK_CXX_SOURCE_COMPILES("#include <iconv.h>\nint main(){iconv_t cd = iconv_open(\"\",\"\");iconv(cd,0,0,0,0);iconv_close(cd);return 0;}" WITH_STDLIBC_ICONV)
  if(WITH_STDLIBC_ICONV)
    message(STATUS "Info: found builtin ICONV support inside the C standard library.")
    set(DCMTK_WITH_STDLIBC_ICONV ON CACHE BOOL "" FORCE)
  else()
    set(DCMTK_WITH_STDLIBC_ICONV OFF CACHE BOOL "" FORCE)
  endif()
endif()

if(DCMTK_WITH_ICU)
  find_package(ICU COMPONENTS uc data QUIET)
  if(NOT ICU_FOUND)
    message(STATUS "Warning: ICU support will be disabled because the ICU were not found.")
    set(DCMTK_WITH_ICU OFF CACHE BOOL "" FORCE)
    set(WITH_ICU "")
  else()
    message(STATUS "Info: DCMTK ICU support will be enabled")
    set(WITH_ICU 1)
    set(ICU_INCDIR ${ICU_INCLUDE_DIR})
    set(ICU_LIBS ${ICU_LIBRARIES})
    include_directories(${ICU_INCLUDE_DIR})
  endif()
endif()

# Find doxygen
if(DCMTK_WITH_DOXYGEN)
  find_package(Doxygen QUIET) # will set variable DOXYGEN_EXECUTABLE
  if(NOT DOXYGEN_FOUND)
    message(STATUS "Warning: DOXYGEN support will be disabled because doxygen was not found.")
    set(DCMTK_WITH_DOXYGEN OFF CACHE BOOL "" FORCE)
  endif()
endif()

if(DCMTK_WITH_ICONV OR DCMTK_WITH_STDLIBC_ICONV)
  include(CheckCXXSourceCompiles)
  set(CMAKE_REQUIRED_INCLUDES ${LIBICONV_INCDIR})
  set(CMAKE_REQUIRED_LIBRARIES ${LIBICONV_LIBS})
  CHECK_CXX_SOURCE_COMPILES("
  #include <iconv.h>
  int main() {
    iconv_t cd = 0;
    const char *in = 0;
    iconv(cd, &in, 0, 0, 0);
    return 0;
  }" LIBICONV_SECOND_ARGUMENT_CONST)
  set(CMAKE_REQUIRED_INCLUDES)
  set(CMAKE_REQUIRED_LIBRARIES)
endif()
