# declare executables
DCMTK_ADD_TEST_EXECUTABLE(msgserv msgserv.cc)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(msgserv dcmpstat dcmdsig dcmsr dcmimage dcmimgle dcmqrdb dcmnet dcmtls dcmdata oflog ofstd)
