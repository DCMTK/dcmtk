# create library from source files
DCMTK_ADD_LIBRARY(dcmwlm wlds wldsfs wlfsim wlmactmg)

TARGET_LINK_LIBRARIES(dcmwlm ofstd dcmdata dcmnet)
