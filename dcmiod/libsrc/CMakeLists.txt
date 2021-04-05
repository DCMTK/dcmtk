# create library from source files
set(dcmiod_src
  cielabutil.cc
  iodcommn.cc
  iodcontentitemmacro.cc
  iodmacro.cc
  iodreferences.cc
  iodrules.cc
  iodtypes.cc
  iodutil.cc
  modacquisitioncontext.cc
  modbase.cc
  modcommoninstanceref.cc
  modenhequipment.cc
  modenhusimage.cc
  modenhusseries.cc
  modequipment.cc
  modfloatingpointimagepixel.cc
  modfor.cc
  modgeneralimage.cc
  modgeneralseries.cc
  modgeneralstudy.cc
  modhelp.cc
  modimagepixelbase.cc
  modimagepixel.cc
  modmultiframedimension.cc
  modmultiframefg.cc
  modpatient.cc
  modpatientstudy.cc
  modsegmentationseries.cc
  modsopcommon.cc
  modsynchronization.cc
  modusfor.cc
)
DCMTK_ADD_LIBRARY(dcmiod ${dcmiod_src} )

DCMTK_TARGET_LINK_MODULES(dcmiod dcmdata ofstd oflog)

