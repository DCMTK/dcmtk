# declare additional include directories needed for compression support
include_directories("${dcmjpls_SOURCE_DIR}/include" "${dcmjpeg_SOURCE_DIR}/include" "${dcmimage_SOURCE_DIR}/include" "${dcmimgle_SOURCE_DIR}/include")

# declare executables
foreach(PROGRAM dcmrecv dcmsend echoscu findscu getscu movescu storescp storescu termscu)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM}.cc)
endforeach()

# declare installation files
if(WITH_OPENSSL)
  if(WIN32)
    install(DIRECTORY "${OPENSSL_BINDIR}/"
        DESTINATION "${CMAKE_INSTALL_BINDIR}"
        COMPONENT bin
        CONFIGURATIONS Release MinSizeRel
        FILES_MATCHING REGEX "dcmtk(eay|crypto|ssl).*\\.dll"
        REGEX "dcmtk(eay|crypto|ssl)_d.*\\.dll" EXCLUDE
    )
    install(DIRECTORY "${OPENSSL_BINDIR}/"
        DESTINATION "${CMAKE_INSTALL_BINDIR}"
        COMPONENT bin
        CONFIGURATIONS Debug RelWithDebInfo
        FILES_MATCHING REGEX "dcmtk(eay|crypto|ssl)_d.*\\.dll"
    )
  endif()
endif()

# make sure executables are linked to the corresponding libraries
foreach(PROGRAM dcmrecv dcmsend echoscu findscu getscu movescu storescp storescu termscu)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmnet dcmdata oflog ofstd)
endforeach()
foreach(PROGRAM dcmrecv echoscu findscu storescp storescu getscu)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmtls)
endforeach()

# uncomment the following line if "storescu" is compiled with ON_THE_FLY_COMPRESSION defined
#DCMTK_TARGET_LINK_MODULES(storescu dcmjpls dcmjpeg dcmimage)

# "dcmsend" always needs compression support
DCMTK_TARGET_LINK_MODULES(dcmsend dcmjpls dcmjpeg dcmimage)
