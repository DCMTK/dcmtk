# create library from source files
DCMTK_ADD_LIBRARY(dcmnet
  assoc.cc
  cond.cc
  dcasccff.cc
  dcasccfg.cc
  dccfenmp.cc
  dccfpcmp.cc
  dccfprmp.cc
  dccfrsmp.cc
  dccftsmp.cc
  dccfuidh.cc
  dcmlayer.cc
  dcmtrans.cc
  dcompat.cc
  dcuserid.cc
  dfindscu.cc
  dimcancl.cc
  dimcmd.cc
  dimdump.cc
  dimecho.cc
  dimfind.cc
  dimget.cc
  dimmove.cc
  dimse.cc
  dimstore.cc
  diutil.cc
  dstorscp.cc
  dstorscu.cc
  dul.cc
  dulconst.cc
  dulextra.cc
  dulfsm.cc
  dulparse.cc
  dulpres.cc
  dwrap.c
  extneg.cc
  helpers.cc
  lst.cc
  scp.cc
  scpcfg.cc
  scppool.cc
  scpthrd.cc
  scu.cc
)

DCMTK_TARGET_LINK_MODULES(dcmnet ofstd oflog dcmdata)
DCMTK_TARGET_LINK_LIBRARIES(dcmnet ${WRAP_LIBS})

if(TARGET dcmnet)
  set_property(TARGET dcmnet PROPERTY OUTPUT_NAME "dcmnet${DCMTK_TLS_LIBRARY_POSTFIX}")
endif()
