# create library from source files
ADD_LIBRARY(dcmimage diargimg dicmyimg dicoimg dicoopx dicopx dihsvimg dilogger dipalimg dipipng dipitiff diqtctab diqtfs diqthash diqthitl diqtpbox diquant diregist dirgbimg diybrimg diyf2img diyp2img)

TARGET_LINK_LIBRARIES(dcmimage oflog dcmdata dcmimgle ${LIBTIFF_LIBS} ${LIBPNG_LIBS})

# set proper version information for shared library
IF(BUILD_SHARED_LIBS)
  SET_TARGET_PROPERTIES(dcmimage PROPERTIES ${DCMTK_LIBRARY_PROPERTIES})
ENDIF(BUILD_SHARED_LIBS)

# declare installation files
INSTALL(TARGETS dcmimage
        COMPONENT lib
        RUNTIME DESTINATION ${DCMTK_INSTALL_BINDIR}
        LIBRARY DESTINATION ${DCMTK_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${DCMTK_INSTALL_LIBDIR})
