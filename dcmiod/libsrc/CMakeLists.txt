# create library from source files
DCMTK_ADD_LIBRARY(dcmiod
  cielabutil.cc
  iodcontentitemmacro.cc
  iodcommn.cc
  iodmacro.cc
  iodrules.cc
  iodreferences.cc
  iodutil.cc
  iodtypes.cc
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
  modimagepixel.cc
  modimagepixelbase.cc
  modmultiframefg.cc
  modmultiframedimension.cc
  modpatient.cc
  modpatientstudy.cc
  modsegmentationseries.cc
  modsopcommon.cc
  modsynchronization.cc
  modusfor.cc
)

DCMTK_TARGET_LINK_MODULES(dcmiod dcmdata ofstd oflog)

