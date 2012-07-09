# create library from source files
ADD_LIBRARY(dcmimgle dcmimage dibaslut diciefn dicielut didislut didispfn didocu digsdfn digsdlut diimage diinpx diluptab dimo1img dimo2img dimoimg dimoimg3 dimoimg4 dimoimg5 dimomod dimoopx dimopx diovdat diovlay diovlimg diovpln diutils)

TARGET_LINK_LIBRARIES(dcmimgle ofstd oflog dcmdata)
DCMTK_SETUP_LIBRARY(dcmimgle)

# declare installation files
INSTALL(TARGETS dcmimgle
        COMPONENT lib
        RUNTIME DESTINATION ${DCMTK_INSTALL_BINDIR}
        LIBRARY DESTINATION ${DCMTK_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${DCMTK_INSTALL_LIBDIR})
