# create library from source files
DCMTK_ADD_LIBRARY(dcmqrdb dcmqrcbf dcmqrcbg dcmqrcbm dcmqrcbs dcmqrcnf dcmqrdbi dcmqrdbs dcmqropt dcmqrptb dcmqrsrv dcmqrtis)

TARGET_LINK_LIBRARIES(dcmqrdb ofstd dcmdata dcmnet)
