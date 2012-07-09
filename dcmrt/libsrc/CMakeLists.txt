# create library from source files
ADD_LIBRARY(dcmrt drtdose drtimage drtplan drtstruct drttreat drtionpl drtiontr drttypes drtadcs drtads drtas1 drtas5 drtas6 drtas7 drtass drtbads drtbas drtbcps drtbl2 drtbl5 drtbldls drtbldps drtblds1 drtblds5 drtblds6 drtbldts drtbrcss drtbrdrs drtbrs drtbs drtcbars drtccs drtcdrs drtces drtchs drtcims drtcis drtcncs drtcos drtcpas drtcps drtcs drtcshs drtcsis drtcss drtdcs drtdimcs drtdrs drtds drtdspcs drtdss drtdvhs drtdvrrs drteas drtes drtfds drtfgs drtfgss drtfms drtforrs drtfsss drthsdrs drtiblds drtibls drtibs drticpds drticps drtics drtiis drtircs drtitts drtiwps drtiws drtlsds6 drtlsds7 drtlsds drtmacds drtmas drtmdrs drtmls drtmps drtmss drtmucs drtoas drtopis drtos drtpbcs drtpcs drtpcxs drtpics drtporcs drtporis drtppcs drtprsis drtpscs drtpss drtpvis drtras drtrbas2 drtrbass8 drtrbls drtrbos1 drtrbos6 drtrbos7 drtrbs2 drtrbs8 drtrcdrs drtrcos drtrcps drtrcs drtrdros drtrdrs1 drtrdrs6 drtrdrs8 drtrds drtrfgs drtrfors drtrics drtrims drtris drtrlsds drtrmdrs drtrms drtrmss6 drtrmss7 drtrpcs drtrpis drtrpps drtrppss drtrps drtrris1 drtrris6 drtrris9 drtrrms drtrros drtrrpcs drtrrros drtrrs drtrrshs drtrrtps3 drtrrtps4 drtrrtps5 drtrrtps drtrses drtrshs6 drtrshs7 drtrshs drtrsis drtrsns drtrss drtrsss drtrsts drtrtrs drtrvis drtrws drtsds drtshds drtsis drtsns drtspcs drtss drtssrs drttms0 drttms9 drttscds drttsibs drttsmds drttts drtvls drtwps drtws)

TARGET_LINK_LIBRARIES(dcmrt ofstd oflog dcmdata)
DCMTK_SETUP_LIBRARY(dcmrt)

# declare installation files
INSTALL(TARGETS dcmrt
        COMPONENT lib
        RUNTIME DESTINATION ${DCMTK_INSTALL_BINDIR}
        LIBRARY DESTINATION ${DCMTK_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${DCMTK_INSTALL_LIBDIR})
