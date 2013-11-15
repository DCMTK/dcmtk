# create library from source files
DCMTK_ADD_LIBRARY(dcmdata cmdlnarg dcbytstr dcchrstr dccodec dcdatset dcddirif dcdicdir dcdicent dcdict dcdictzz dcdirrec dcelem dcerror dcfilefo dchashdi dcistrma dcistrmb dcistrmf dcistrmz dcitem dclist dcmetinf dcobject dcostrma dcostrmb dcostrmf dcostrmz dcpcache dcpixel dcpixseq dcpxitem dcrleccd dcrlecce dcrlecp dcrledrg dcrleerg dcrlerp dcsequen dcspchrs dcstack dcswap dctag dctagkey dctypes dcuid dcwcache dcvr dcvrae dcvras dcvrat dcvrcs dcvrda dcvrds dcvrdt dcvrfd dcvrfl dcvris dcvrlo dcvrlt dcvrobow dcvrof dcvrod dcvrpn dcvrpobw dcvrsh dcvrsl dcvrss dcvrst dcvrtm dcvrui dcvrul dcvrulup dcvrus dcvrut dcxfer dcpath vrscan vrscanl dcfilter)

DCMTK_TARGET_LINK_MODULES(dcmdata ofstd oflog)
DCMTK_TARGET_LINK_LIBRARIES(dcmdata ${ZLIB_LIBS})

# Various dictionary-related commands
SET(DICOMDIC   "${dcmdata_SOURCE_DIR}/data/dicom.dic")
SET(PRIVATEDIC "${dcmdata_SOURCE_DIR}/data/private.dic")
SET(DICTIONARIES "${DICOMDIC}" "${PRIVATEDIC}")

IF(BUILD_APPS)
  # don't use DCMTK_ADD_EXECUTABLE() since we don't want to install the files
  ADD_EXECUTABLE(mkdictbi EXCLUDE_FROM_ALL mkdictbi dctagkey dcdicent dcdict dcdictbi dcvr dchashdi cmdlnarg dctypes)
  ADD_EXECUTABLE(mkdeftag EXCLUDE_FROM_ALL mkdeftag dctagkey dcdicent dcdict dcdictbi dcvr dchashdi cmdlnarg dctypes)
ENDIF(BUILD_APPS)
DCMTK_TARGET_LINK_MODULES(mkdictbi ofstd oflog)
DCMTK_TARGET_LINK_MODULES(mkdeftag ofstd oflog)

ADD_CUSTOM_TARGET(gendeftag
        COMMAND mkdeftag -o ${dcmdata_SOURCE_DIR}/include/dcmtk/dcmdata/dcdeftag.h ${DICTIONARIES}
        DEPENDS mkdeftag
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Generate a new include file of defined tags")

ADD_CUSTOM_TARGET(dcdictzz
        COMMAND mkdictbi -o dcdictzz.cc ${DICTIONARIES}
        DEPENDS mkdictbi
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Generate a file which can be used as a builtin dictionary")

ADD_CUSTOM_TARGET(builtindict
        COMMAND mkdictbi -o dcdictbi.cc ${DICTIONARIES}
        DEPENDS mkdictbi
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Generate a builtin data dictionary")

ADD_CUSTOM_TARGET(nobuiltindict
        COMMAND ${CMAKE_COMMAND} -E copy dcdictbi.nul dcdictbi.cc
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Generate an empty builtin data dictionary")

ADD_CUSTOM_TARGET(mkvrscan
        COMMAND flex -8 --header-file=vrscanl.h --outfile=vrscanl.c vrscanl.l
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Generate vrscanl.c and vrscanl.h")
