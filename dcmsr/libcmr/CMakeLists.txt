# create library from source files
DCMTK_ADD_LIBRARY(cmr init tid1001 tid1204 cid11 cid42 cid4020 cid4021 cid4031 cid5000 cid5001 cid7445 cid7452 cid7453 cid10013 cid10033 srnumvl)

DCMTK_TARGET_LINK_MODULES(cmr dcmsr)
