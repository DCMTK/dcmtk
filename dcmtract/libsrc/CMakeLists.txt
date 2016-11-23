# create library from source files
DCMTK_ADD_LIBRARY(dcmtract trctypes trctrackset trctrack trcmeasurement trcstatistic trcmodtractresults trctractographyresults)

DCMTK_TARGET_LINK_MODULES(dcmtract dcmiod dcmdata ofstd oflog)
