# create library from source files
DCMTK_ADD_LIBRARY(dcmiod
  cielabutil
  iodcommn
  iodmacro
  iodimage
  iodrules
  iodutil
  iodtypes
  modacquisitioncontext
  modbase
  modcommoninstanceref
  modenhequipment
  modenhusimage
  modenhusseries
  modequipment
  modfor
  modgeneralimage
  modgeneralseries
  modgeneralstudy
  modhelp
  modimagepixel
  modmultiframefg
  modmultiframedimension
  modpatient
  modpatientstudy
  modsegmentationseries
  modsopcommon
  modsynchronization
  modusfor)

DCMTK_TARGET_LINK_MODULES(dcmiod dcmdata ofstd oflog)
DCMTK_TARGET_LINK_LIBRARIES(dcmiod ${ZLIB_LIBS})

