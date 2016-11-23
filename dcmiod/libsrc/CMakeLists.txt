# create library from source files
DCMTK_ADD_LIBRARY(dcmiod
  cielabutil
  iodcontentitemmacro
  iodcommn
  iodmacro
  iodrules
  iodreferences
  iodutil
  iodtypes
  modacquisitioncontext
  modbase
  modcommoninstanceref
  modenhequipment
  modenhusimage
  modenhusseries
  modequipment
  modfloatingpointimagepixel
  modfor
  modgeneralimage
  modgeneralseries
  modgeneralstudy
  modhelp
  modimagepixel
  modimagepixelbase
  modmultiframefg
  modmultiframedimension
  modpatient
  modpatientstudy
  modsegmentationseries
  modsopcommon
  modsynchronization
  modusfor)

DCMTK_TARGET_LINK_MODULES(dcmiod dcmdata ofstd oflog)

