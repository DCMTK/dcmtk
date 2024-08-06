# declare project
project(dcmapps)

# in dcmapps, we permit dependencies on all modules
include_directories(
  "${dcmdata_SOURCE_DIR}/include"
  "${dcmect_SOURCE_DIR}/include"
  "${dcmfg_SOURCE_DIR}/include"
  "${dcmimage_SOURCE_DIR}/include"
  "${dcmimgle_SOURCE_DIR}/include"
  "${dcmiod_SOURCE_DIR}/include"
  "${dcmjpeg_SOURCE_DIR}/include"
  "${dcmjpls_SOURCE_DIR}/include"
  "${dcmnet_SOURCE_DIR}/include"
  "${dcmpmap_SOURCE_DIR}/include"
  "${dcmpstat_SOURCE_DIR}/include"
  "${dcmqrdb_SOURCE_DIR}/include"
  "${dcmrt_SOURCE_DIR}/include"
  "${dcmseg_SOURCE_DIR}/include"
  "${dcmsign_SOURCE_DIR}/include"
  "${dcmsr_SOURCE_DIR}/include"
  "${dcmtls_SOURCE_DIR}/include"
  "${dcmtract_SOURCE_DIR}/include"
  "${dcmwlm_SOURCE_DIR}/include"
  "${oficonv_SOURCE_DIR}/include"
  "${oflog_SOURCE_DIR}/include"
  "${ofstd_SOURCE_DIR}/include"

  ${ZLIB_INCDIR} ${OPENSSL_INCDIR})

# recurse into subdirectories
foreach(SUBDIR apps)
  add_subdirectory(${SUBDIR})
endforeach()
