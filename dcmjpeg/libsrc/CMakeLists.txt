# declare additional include directories
include_directories("${dcmjpeg_SOURCE_DIR}/include" "${ofstd_SOURCE_DIR}/include" "${oflog_SOURCE_DIR}/include" "${dcmdata_SOURCE_DIR}/include" "${dcmimgle_SOURCE_DIR}/include" "${dcmimage_SOURCE_DIR}/include" "${dcmjpeg_SOURCE_DIR}/libijg8" "${dcmjpeg_SOURCE_DIR}/libijg12" "${dcmjpeg_SOURCE_DIR}/libijg16" ${ZLIB_INCDIR})

# create library from source files
DCMTK_ADD_LIBRARY(dcmjpeg
  ddpiimpl.cc
  dipijpeg.cc
  djcodecd.cc
  djcodece.cc
  djcparam.cc
  djdecbas.cc
  djdecext.cc
  djdeclol.cc
  djdecode.cc
  djdecpro.cc
  djdecsps.cc
  djdecsv1.cc
  djdijg12.cc
  djdijg8.cc
  djdijg16.cc
  djeijg12.cc
  djeijg8.cc
  djeijg16.cc
  djencbas.cc
  djencext.cc
  djenclol.cc
  djencode.cc
  djencpro.cc
  djencsps.cc
  djencsv1.cc
  djrplol.cc
  djrploss.cc
  djutils.cc
)

DCMTK_TARGET_LINK_MODULES(dcmjpeg ofstd oflog dcmdata dcmimgle dcmimage ijg8 ijg12 ijg16)
