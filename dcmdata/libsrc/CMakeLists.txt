# create library from source files

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

DCMTK_ADD_LIBRARY(dcmdata
  cmdlnarg.cc
  dcbytstr.cc
  dcchrstr.cc
  dccodec.cc
  dcdatset.cc
  dcdatutl.cc
  dcddirif.cc
  dcdicdir.cc
  dcdicent.cc
  dcdict.cc
  dcdictbi.cc
  dcdirrec.cc
  dcelem.cc
  dcencdoc.cc
  dcerror.cc
  dcfilefo.cc
  dcfilter.cc
  dchashdi.cc
  dcistrma.cc
  dcistrmb.cc
  dcistrmf.cc
  dcistrms.cc
  dcistrmz.cc
  dcitem.cc
  dcjson.cc
  dclist.cc
  dcmatch.cc
  dcmetinf.cc
  dcobject.cc
  dcostrma.cc
  dcostrmb.cc
  dcostrmf.cc
  dcostrms.cc
  dcostrmz.cc
  dcpath.cc
  dcpcache.cc
  dcpixel.cc
  dcpixseq.cc
  dcpxitem.cc
  dcrleccd.cc
  dcrlecce.cc
  dcrlecp.cc
  dcrledrg.cc
  dcrleerg.cc
  dcrlerp.cc
  dcsequen.cc
  dcspchrs.cc
  dcstack.cc
  dcswap.cc
  dctag.cc
  dctagkey.cc
  dctypes.cc
  dcuid.cc
  dcvr.cc
  dcvrae.cc
  dcvras.cc
  dcvrat.cc
  dcvrcs.cc
  dcvrda.cc
  dcvrds.cc
  dcvrdt.cc
  dcvrfd.cc
  dcvrfl.cc
  dcvris.cc
  dcvrlo.cc
  dcvrlt.cc
  dcvrobow.cc
  dcvrod.cc
  dcvrof.cc
  dcvrol.cc
  dcvrov.cc
  dcvrpn.cc
  dcvrpobw.cc
  dcvrsh.cc
  dcvrsl.cc
  dcvrss.cc
  dcvrst.cc
  dcvrsv.cc
  dcvrtm.cc
  dcvruc.cc
  dcvrui.cc
  dcvrul.cc
  dcvrulup.cc
  dcvrur.cc
  dcvrus.cc
  dcvrut.cc
  dcvruv.cc
  dcwcache.cc
  dcxfer.cc
  vrscan.cc
  vrscanl.c
)

DCMTK_TARGET_LINK_MODULES(dcmdata ofstd oflog)
DCMTK_TARGET_LINK_LIBRARIES(dcmdata ${ZLIB_LIBS})

# Various dictionary-related commands
set(DICOMDIC   "${dcmdata_SOURCE_DIR}/data/dicom.dic")
set(PRIVATEDIC "${dcmdata_SOURCE_DIR}/data/private.dic")
set(DICTIONARIES "${DICOMDIC}" "${PRIVATEDIC}")

if(BUILD_APPS)
  # don't use DCMTK_ADD_EXECUTABLE() since we don't want to install the files
  add_executable(mkdictbi EXCLUDE_FROM_ALL
    cmdlnarg.cc
    dcdicent.cc
    dcdict.cc
    dchashdi.cc
    dctagkey.cc
    dctypes.cc
    dcvr.cc
    mkdictbi.cc
  )
  add_executable(mkdeftag EXCLUDE_FROM_ALL
    cmdlnarg.cc
    dcdicent.cc
    dcdict.cc
    dchashdi.cc
    dctagkey.cc
    dctypes.cc
    dcvr.cc
    mkdeftag.cc
  )
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
