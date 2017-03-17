DCMTK_ADD_EXECUTABLE(msgserv msgserv)
DCMTK_TARGET_LINK_MODULES(msgserv dcmpstat dcmdsig dcmsr dcmimage dcmimgle dcmqrdb dcmnet dcmtls dcmdata oflog ofstd)
