# create library from source files
DCMTK_ADD_LIBRARY(dcmimage diargimg dicmyimg dicoimg dicoopx dicopx dihsvimg dilogger dipalimg dipipng dipitiff diqtctab diqtfs diqthash diqthitl diqtpbox diquant diregist dirgbimg diybrimg diyf2img diyp2img dcmicmph)

DCMTK_TARGET_LINK_MODULES(dcmimage oflog dcmdata dcmimgle)
DCMTK_TARGET_LINK_LIBRARIES(dcmimage ${LIBTIFF_LIBS} ${LIBPNG_LIBS})
