# create library from source files
set(dcmimgle_src
    dcmimage.cc
    dibaslut.cc
    diciefn.cc
    dicielut.cc
    didislut.cc
    didispfn.cc
    didocu.cc
    digsdfn.cc
    digsdlut.cc
    diimage.cc
    diinpx.cc
    diluptab.cc
    dimo1img.cc
    dimo2img.cc
    dimoimg3.cc
    dimoimg4.cc
    dimoimg5.cc
    dimoimg.cc
    dimomod.cc
    dimoopx.cc
    dimopx.cc
    diovdat.cc
    diovlay.cc
    diovlimg.cc
    diovpln.cc
    diutils.cc
)
DCMTK_ADD_LIBRARY(dcmimgle ${dcmimgle_src})

DCMTK_TARGET_LINK_MODULES(dcmimgle ofstd oflog dcmdata)
