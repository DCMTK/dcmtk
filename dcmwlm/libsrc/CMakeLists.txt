# create library from source files
DCMTK_ADD_LIBRARY(dcmwlm wlds wldsfs wlfsim wlmactmg)

DCMTK_TARGET_LINK_MODULES(dcmwlm ofstd dcmdata dcmnet)
