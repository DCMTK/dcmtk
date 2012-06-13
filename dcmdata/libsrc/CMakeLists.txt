# create library from source files
ADD_LIBRARY(dcmdata cmdlnarg dcbytstr dcchrstr dccodec dcdatset dcddirif dcdicdir dcdicent dcdict dcdictzz dcdirrec dcelem dcerror dcfilefo dchashdi dcistrma dcistrmb dcistrmf dcistrmz dcitem dclist dcmetinf dcobject dcostrma dcostrmb dcostrmf dcostrmz dcpcache dcpixel dcpixseq dcpxitem dcrleccd dcrlecce dcrlecp dcrledrg dcrleerg dcrlerp dcsequen dcspchrs dcstack dcswap dctag dctagkey dctypes dcuid dcwcache dcvr dcvrae dcvras dcvrat dcvrcs dcvrda dcvrds dcvrdt dcvrfd dcvrfl dcvris dcvrlo dcvrlt dcvrobow dcvrof dcvrpn dcvrpobw dcvrsh dcvrsl dcvrss dcvrst dcvrtm dcvrui dcvrul dcvrulup dcvrus dcvrut dcxfer dcpath vrscan vrscanl)

TARGET_LINK_LIBRARIES(dcmdata ofstd oflog ${ZLIB_LIBS})

# set proper version information for shared library
IF(BUILD_SHARED_LIBS)
  SET_TARGET_PROPERTIES(dcmdata PROPERTIES ${DCMTK_LIBRARY_PROPERTIES})
ENDIF(BUILD_SHARED_LIBS)

# declare installation files
INSTALL(TARGETS dcmdata
        COMPONENT lib
        RUNTIME DESTINATION ${DCMTK_INSTALL_BINDIR}
        LIBRARY DESTINATION ${DCMTK_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${DCMTK_INSTALL_LIBDIR})
