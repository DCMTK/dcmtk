# create library from source files
DCMTK_ADD_LIBRARY(dcmqrdb dcmqrcbf dcmqrcbg dcmqrcbm dcmqrcbs dcmqrcnf dcmqrdbi dcmqrdbs dcmqropt dcmqrptb dcmqrsrv dcmqrtis)

DCMTK_TARGET_LINK_MODULES(dcmqrdb ofstd dcmdata dcmnet)
