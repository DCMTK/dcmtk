# create library from source files
DCMTK_ADD_LIBRARY(dcmnet assoc cond dcasccff dcasccfg dccfenmp dccfpcmp dccfprmp dccfrsmp dccftsmp dccfuidh dcmlayer dcmtrans dcompat dimcancl dimcmd dimdump dimecho dimfind dimget dimmove dimse dimstore diutil dul dulconst dulextra dulfsm dulparse dulpres extneg lst dfindscu dstorscp dstorscu dcuserid scu scp scpthrd scpcfg scppool dwrap)

DCMTK_TARGET_LINK_MODULES(dcmnet ofstd oflog dcmdata)
DCMTK_TARGET_LINK_LIBRARIES(dcmnet ${WRAP_LIBS})
