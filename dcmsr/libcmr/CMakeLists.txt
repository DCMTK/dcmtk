# create library from source files
DCMTK_ADD_LIBRARY(cmr tid1001 tid1204 cid5000 cid5001 cid7445 cid7452 cid7453)

DCMTK_TARGET_LINK_MODULES(cmr dcmsr)
