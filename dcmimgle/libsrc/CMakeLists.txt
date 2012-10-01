# create library from source files
DCMTK_ADD_LIBRARY(dcmimgle dcmimage dibaslut diciefn dicielut didislut didispfn didocu digsdfn digsdlut diimage diinpx diluptab dimo1img dimo2img dimoimg dimoimg3 dimoimg4 dimoimg5 dimomod dimoopx dimopx diovdat diovlay diovlimg diovpln diutils)

TARGET_LINK_LIBRARIES(dcmimgle ofstd oflog dcmdata)
