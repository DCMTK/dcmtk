# create library from source files
DCMTK_ADD_LIBRARY(dcmsr dsrcitem dsrcodtn dsrcodvl dsrcomtn dsrcomvl dsrcontn dsrcsidl dsrdattn dsrdncsr dsrdnflt dsrdoc dsrdocst dsrdoctn dsrdoctr dsrdtitn dsrimgfr dsrimgse dsrimgtn dsrimgvl dsritcsr dsrnumtn dsrnumvl dsrpnmtn dsrposcn dsrrefin dsrreftn dsrscogr dsrsc3gr dsrscotn dsrsc3tn dsrscovl dsrsc3vl dsrsoprf dsrstrvl dsrtcodt dsrtcosp dsrtcotn dsrtcoto dsrtcovl dsrtextn dsrtimtn dsrtpltn dsrtree dsrtypes dsruidtn dsrwavch dsrwavtn dsrwavvl dsrxmlc dsrxmld dsriodcc dsrbascc dsrenhcc dsrcomcc dsrkeycc dsrmamcc dsrchecc dsrcolcc dsrprocc dsrxrdcc dsrspecc dsrmaccc dsrimpcc dsrc3dcc dsrrrdcc dsracqcc dsrsaecc dsrprdcc dsrpficc dsrplicc dsrctpl dsrrtpl dsrstpl dsrctxgr)

DCMTK_TARGET_LINK_MODULES(dcmsr ofstd oflog dcmdata dcmimgle dcmimage)
DCMTK_TARGET_LINK_LIBRARIES(dcmsr ${LIBXML_LIBS})
