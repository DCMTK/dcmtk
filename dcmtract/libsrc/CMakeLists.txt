# create library from source files
DCMTK_ADD_LIBRARY(dcmtract
  trctypes.cc
  trctrackset.cc
  trctrack.cc
  trcmeasurement.cc
  trcstatistic.cc
  trcmodtractresults.cc
  trctractographyresults.cc
)

DCMTK_TARGET_LINK_MODULES(dcmtract dcmiod dcmdata ofstd oflog)
