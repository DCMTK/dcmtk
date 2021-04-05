# create library from source files
set(dcmimage_src
    dcmicmph.cc
    diargimg.cc
    dicmyimg.cc
    dicoimg.cc
    dicoopx.cc
    dicopx.cc
    dihsvimg.cc
    dilogger.cc
    dipalimg.cc
    dipipng.cc
    dipitiff.cc
    diqtctab.cc
    diqtfs.cc
    diqthash.cc
    diqthitl.cc
    diqtpbox.cc
    diquant.cc
    diregist.cc
    dirgbimg.cc
    diybrimg.cc
    diyf2img.cc
    diyp2img.cc
)
DCMTK_ADD_LIBRARY(dcmimage ${dcmimage_src})

DCMTK_TARGET_LINK_MODULES(dcmimage oflog dcmdata dcmimgle)
DCMTK_TARGET_LINK_LIBRARIES(dcmimage ${LIBTIFF_LIBS} ${LIBPNG_LIBS})
