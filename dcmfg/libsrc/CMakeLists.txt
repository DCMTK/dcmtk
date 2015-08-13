# create library from source files
DCMTK_ADD_LIBRARY(dcmfg
  fg
  fgbase
  fgderimg
  fgfact
  fgfracon
  fgframevoilut
  fgimagedatatype
  fginterface
  fgpixmsr
  fgplanor
  fgplanorvol
  fgplanpo
  fgplanposvol
  fgrealworldvaluemapping
  fgseg
  fgusimagedescription
  fgtypes
  stack
  stackinterface)

DCMTK_TARGET_LINK_MODULES(dcmfg dcmiod dcmdata ofstd oflog)
