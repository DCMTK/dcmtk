# create library from source files

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

DCMTK_ADD_LIBRARY(dcmdata
  cmdlnarg dcbytstr dcchrstr dccodec dcdatset dcdatutl dcddirif dcdicdir dcdicent
  dcdict dcdictbi dcdirrec dcelem dcencdoc dcerror dcfilefo dcfilter dchashdi dcistrma
  dcistrmb dcistrmf dcistrmz dcitem dcjson dclist dcmatch dcmetinf dcobject dcostrma
  dcostrmb dcostrmf dcostrmz dcpath dcpcache dcpixel dcpixseq dcpxitem dcrleccd
  dcrlecce dcrlecp dcrledrg dcrleerg dcrlerp dcsequen dcspchrs dcstack dcswap dctag
  dctagkey dctypes dcuid dcvr dcvrae dcvras dcvrat dcvrcs dcvrda dcvrds dcvrdt
  dcvrfd dcvrfl dcvris dcvrlo dcvrlt dcvrobow dcvrod dcvrof dcvrol dcvrov dcvrpn
  dcvrpobw dcvrsh dcvrsl dcvrss dcvrst dcvrsv dcvrtm dcvruc dcvrui dcvrul dcvrulup
  dcvrur dcvrus dcvrut dcvruv dcwcache dcxfer vrscan vrscanl)

DCMTK_TARGET_LINK_MODULES(dcmdata ofstd oflog)
DCMTK_TARGET_LINK_LIBRARIES(dcmdata ${ZLIB_LIBS})

# Various dictionary-related commands
set(DICOMDIC   "${dcmdata_SOURCE_DIR}/data/dicom.dic")
set(PRIVATEDIC "${dcmdata_SOURCE_DIR}/data/private.dic")
set(DICTIONARIES "${DICOMDIC}" "${PRIVATEDIC}")

if(BUILD_APPS)
  # don't use DCMTK_ADD_EXECUTABLE() since we don't want to install the files
  add_executable(mkdictbi EXCLUDE_FROM_ALL mkdictbi dctagkey dcdicent dcdict dcvr dchashdi cmdlnarg dctypes)
  add_executable(mkdeftag EXCLUDE_FROM_ALL mkdeftag dctagkey dcdicent dcdict dcvr dchashdi cmdlnarg dctypes)
  # workaround for Visual Studio to disable dll storage specifiers when linking directly into the executable.
  # this is needed since the built-in dictionary code is created by the tools below and thus those tools
  # statically link the few required dcmdata source files instead of linking to dcmdata as a whole.
  set_target_properties(mkdictbi mkdeftag PROPERTIES COMPILE_DEFINITIONS "DCMDATA_BUILD_DICTIONARY")
endif()
DCMTK_TARGET_LINK_MODULES(mkdictbi ofstd oflog)
DCMTK_TARGET_LINK_MODULES(mkdeftag ofstd oflog)

add_custom_target(updatedeftag
        COMMAND mkdeftag -o "${dcmdata_SOURCE_DIR}/include/dcmtk/dcmdata/dcdeftag.h" ${DICTIONARIES}
        DEPENDS mkdeftag
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Generate a new include file of defined tags")

add_custom_target(updatebuiltindict
        COMMAND mkdictbi -o dcdictbi.cc ${DICTIONARIES}
        DEPENDS mkdictbi
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Generate a builtin data dictionary")

add_custom_target(mkvrscan
        COMMAND flex -8 --header-file=vrscanl.h --outfile=vrscanl.c vrscanl.l
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Generate vrscanl.c and vrscanl.h")
