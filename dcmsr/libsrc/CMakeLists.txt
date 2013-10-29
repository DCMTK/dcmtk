# create library from source files
DCMTK_ADD_LIBRARY(dcmsr dsrcitem dsrcodtn dsrcodvl dsrcomtn dsrcomvl dsrcontn dsrcsidl dsrdattn dsrdoc dsrdoctn dsrdoctr dsrdtitn dsrimgfr dsrimgse dsrimgtn dsrimgvl dsrnumtn dsrnumvl dsrpnmtn dsrrefin dsrreftn dsrscogr dsrsc3gr dsrscotn dsrsc3tn dsrscovl dsrsc3vl dsrsoprf dsrstrvl dsrtcodt dsrtcosp dsrtcotn dsrtcoto dsrtcovl dsrtextn dsrtimtn dsrtncsr dsrtree dsrtypes dsruidtn dsrwavch dsrwavtn dsrwavvl dsrxmlc dsrxmld dsriodcc dsrbascc dsrenhcc dsrcomcc dsrkeycc dsrmamcc dsrchecc dsrcolcc dsrprocc dsrxrdcc dsrspecc dsrmaccc dsrimpcc dsrc3dcc)

DCMTK_TARGET_LINK_MODULES(dcmsr ofstd oflog dcmdata dcmimgle dcmimage)
DCMTK_TARGET_LINK_LIBRARIES(dcmsr ${LIBXML_LIBS})
