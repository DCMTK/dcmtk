# create library from source files
ADD_LIBRARY(dcmnet assoc cond dcasccff dcasccfg dccfenmp dccfpcmp dccfprmp dccfrsmp dccftsmp dccfuidh dcmlayer dcmtrans dcompat dimcancl dimcmd dimdump dimecho dimfind dimget dimmove dimse dimstore diutil dul dulconst dulextra dulfsm dulparse dulpres extneg lst dfindscu dcuserid scu scp)

# declare installation files
INSTALL_TARGETS(${INSTALL_LIBDIR} dcmnet)
