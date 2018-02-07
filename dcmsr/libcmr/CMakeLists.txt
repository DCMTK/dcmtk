# create library from source files
DCMTK_ADD_LIBRARY(cmr init logger srnumvl tid300 tid1001 tid1204 tid1411 tid1419m tid1500 tid1501 tid15def tid1600 tid4019 cid11 cid29 cid29e cid42 cid100 cid244 cid244e cid4020 cid4021 cid4031 cid4031e cid5000 cid5001 cid6147 cid7021 cid7181 cid7445 cid7452 cid7453 cid7464 cid7469 cid10013 cid10013e cid10033 cid10033e)

DCMTK_TARGET_LINK_MODULES(cmr dcmsr)
