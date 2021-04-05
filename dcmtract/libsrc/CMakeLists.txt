# create library from source files
DCMTK_ADD_LIBRARY(dcmtract
    trcmeasurement.cc
    trcmodtractresults.cc
    trcstatistic.cc
    trctrack.cc
    trctrackset.cc
    trctractographyresults.cc
    trctypes.cc
)
DCMTK_TARGET_LINK_MODULES(dcmtract dcmiod dcmdata ofstd oflog)
