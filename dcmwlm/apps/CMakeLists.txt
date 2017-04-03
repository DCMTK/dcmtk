# declare additional include directories
INCLUDE_DIRECTORIES("${dcmtls_SOURCE_DIR}/include")

# declare executables
DCMTK_ADD_EXECUTABLE(wlmscpfs wlmscpfs wlcefs)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(wlmscpfs dcmwlm dcmnet dcmtls dcmdata oflog ofstd)
