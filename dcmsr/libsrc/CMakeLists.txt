# create library from source files
DCMTK_ADD_LIBRARY(dcmsr dsrcitem dsrcodtn dsrcodvl dsrcomtn dsrcomvl dsrcontn dsrcsidl dsrdattn dsrdoc dsrdoctn dsrdoctr dsrdtitn dsrimgfr dsrimgtn dsrimgvl dsrnumtn dsrnumvl dsrpnmtn dsrrefin dsrreftn dsrscogr dsrsc3gr dsrscotn dsrsc3tn dsrscovl dsrsc3vl dsrsoprf dsrstrvl dsrtcodt dsrtcosp dsrtcotn dsrtcoto dsrtcovl dsrtextn dsrtimtn dsrtncsr dsrtree dsrtypes dsruidtn dsrwavch dsrwavtn dsrwavvl dsrxmlc dsrxmld dsriodcc dsrbascc dsrenhcc dsrcomcc dsrkeycc dsrmamcc dsrchecc dsrcolcc dsrprocc dsrxrdcc dsrspecc dsrmaccc dsrimpcc)

TARGET_LINK_LIBRARIES(dcmsr ofstd oflog dcmdata dcmimgle dcmimage ${LIBXML_LIBS})

# declare installation files
INSTALL(TARGETS dcmsr
        COMPONENT lib
        RUNTIME DESTINATION ${DCMTK_INSTALL_BINDIR}
        LIBRARY DESTINATION ${DCMTK_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${DCMTK_INSTALL_LIBDIR})
