# create library from source files
ADD_LIBRARY(dcmqrdb dcmqrcbf dcmqrcbg dcmqrcbm dcmqrcbs dcmqrcnf dcmqrdbi dcmqrdbs dcmqropt dcmqrptb dcmqrsrv dcmqrtis)

# declare installation files
INSTALL_TARGETS(${DCMTK_INSTALL_LIBDIR} dcmqrdb)
