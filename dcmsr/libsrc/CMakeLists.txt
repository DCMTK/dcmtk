# create library from source files
ADD_LIBRARY(dcmsr dsrcitem dsrcodtn dsrcodvl dsrcomtn dsrcomvl dsrcontn dsrcsidl dsrdattn dsrdoc dsrdoctn dsrdoctr dsrdtitn dsrimgfr dsrimgtn dsrimgvl dsrnumtn dsrnumvl dsrpnmtn dsrreftn dsrscogr dsrsc3gr dsrscotn dsrsc3tn dsrscovl dsrsc3vl dsrsoprf dsrstrvl dsrtcodt dsrtcosp dsrtcotn dsrtcoto dsrtcovl dsrtextn dsrtimtn dsrtncsr dsrtree dsrtypes dsruidtn dsrwavch dsrwavtn dsrwavvl dsrxmlc dsrxmld dsriodcc dsrbascc dsrenhcc dsrcomcc dsrkeycc dsrmamcc dsrchecc dsrcolcc dsrprocc dsrxrdcc dsrspecc dsrmaccc dsrimpcc)

# declare installation files
INSTALL_TARGETS(${INSTALL_LIBDIR} dcmsr)
