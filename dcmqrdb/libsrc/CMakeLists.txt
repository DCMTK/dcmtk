# create library from source files
DCMTK_ADD_LIBRARY(dcmqrdb
  dcmqrcbf.cc
  dcmqrcbg.cc
  dcmqrcbm.cc
  dcmqrcbs.cc
  dcmqrcnf.cc
  dcmqrdbi.cc
  dcmqrdbs.cc
  dcmqropt.cc
  dcmqrptb.cc
  dcmqrsrv.cc
  dcmqrtis.cc
)

DCMTK_TARGET_LINK_MODULES(dcmqrdb ofstd dcmdata dcmnet dcmtls)
