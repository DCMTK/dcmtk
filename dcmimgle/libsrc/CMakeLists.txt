# create library from source files
ADD_LIBRARY(dcmimgle dcmimage dibaslut diciefn dicielut didislut didispfn didocu digsdfn digsdlut diimage diinpx diluptab dimo1img dimo2img dimoimg dimoimg3 dimoimg4 dimoimg5 dimomod dimoopx dimopx diovdat diovlay diovlimg diovpln diutils)

# declare installation files
INSTALL_TARGETS(${INSTALL_LIBDIR} dcmimgle)
