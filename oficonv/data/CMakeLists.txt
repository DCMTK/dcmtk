
# add path to the data files to a CMake variable for being used by the unit tests

set(DCMTK_ICONV_DATAFILES
  "${CMAKE_CURRENT_SOURCE_DIR}"
  CACHE INTERNAL "paths to the oficonv data files"
)

# declare installation files

# Note: Any change to the install instructions below needs to be mirrored in the
# package generation scripts at packages/CMakeLists.txt

install(FILES
    "csmapper/charset.pivot.pvdb"
    "csmapper/mapper.dir"
    "csmapper/mapper.dir.db"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/csmapper"
  COMPONENT data)

install(FILES
    "csmapper/CP/CP936EXT%UCS.mps"
    "csmapper/CP/UCS%CP936EXT.mps"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/csmapper/CP"
  COMPONENT data)

install(FILES
    "csmapper/GB/GB18030%UCS@BMP.mps"
    "csmapper/GB/GB2312%UCS.mps"
    "csmapper/GB/GB2312EXT%UCS.mps"
    "csmapper/GB/GB2312GBK%UCS.mps"
    "csmapper/GB/GB2312UDA1%UCS.mps"
    "csmapper/GB/GB2312UDA2%UCS.mps"
    "csmapper/GB/GBKEXT%UCS.mps"
    "csmapper/GB/GBKUDA%UCS.mps"
    "csmapper/GB/UCS%GB2312.mps"
    "csmapper/GB/UCS%GB2312EXT.mps"
    "csmapper/GB/UCS%GB2312UDA1.mps"
    "csmapper/GB/UCS%GB2312UDA2.mps"
    "csmapper/GB/UCS%GBKEXT.mps"
    "csmapper/GB/UCS%GBKUDA.mps"
    "csmapper/GB/UCS@BMP%GB18030.mps"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/csmapper/GB"
  COMPONENT data)

install(FILES
    "csmapper/ISO-8859/ISO-8859-11%UCS.mps"
    "csmapper/ISO-8859/ISO-8859-15%UCS.mps"
    "csmapper/ISO-8859/ISO-8859-2%UCS.mps"
    "csmapper/ISO-8859/ISO-8859-3%UCS.mps"
    "csmapper/ISO-8859/ISO-8859-4%UCS.mps"
    "csmapper/ISO-8859/ISO-8859-5%UCS.mps"
    "csmapper/ISO-8859/ISO-8859-6%UCS.mps"
    "csmapper/ISO-8859/ISO-8859-7%UCS.mps"
    "csmapper/ISO-8859/ISO-8859-8%UCS.mps"
    "csmapper/ISO-8859/ISO-8859-9%UCS.mps"
    "csmapper/ISO-8859/UCS%ISO-8859-11.mps"
    "csmapper/ISO-8859/UCS%ISO-8859-15.mps"
    "csmapper/ISO-8859/UCS%ISO-8859-2.mps"
    "csmapper/ISO-8859/UCS%ISO-8859-3.mps"
    "csmapper/ISO-8859/UCS%ISO-8859-4.mps"
    "csmapper/ISO-8859/UCS%ISO-8859-5.mps"
    "csmapper/ISO-8859/UCS%ISO-8859-6.mps"
    "csmapper/ISO-8859/UCS%ISO-8859-7.mps"
    "csmapper/ISO-8859/UCS%ISO-8859-8.mps"
    "csmapper/ISO-8859/UCS%ISO-8859-9.mps"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/csmapper/ISO-8859"
  COMPONENT data)

install(FILES
    "csmapper/ISO646/ISO646-JP%UCS.646"
    "csmapper/ISO646/ISO646-US%UCS.646"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/csmapper/ISO646"
  COMPONENT data)

install(FILES
    "csmapper/JIS/JISX0201-KANA%UCS.mps"
    "csmapper/JIS/JISX0208@1990%UCS.mps"
    "csmapper/JIS/JISX0212%UCS.mps"
    "csmapper/JIS/UCS%JISX0201-KANA.mps"
    "csmapper/JIS/UCS%JISX0208@1990.mps"
    "csmapper/JIS/UCS%JISX0212.mps"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/csmapper/JIS"
  COMPONENT data)

install(FILES
    "csmapper/KS/KSC5601HANGUL%UCS.mps"
    "csmapper/KS/KSC5601HANJA%UCS.mps"
    "csmapper/KS/UCS%KSC5601HANGUL.mps"
    "csmapper/KS/UCS%KSC5601HANJA.mps"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/csmapper/KS"
  COMPONENT data)

install(FILES
    "esdb/esdb.alias.db"
    "esdb/esdb.dir.db"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/esdb"
  COMPONENT data)

install(FILES
    "esdb/EUC/EUC-CN.esdb"
    "esdb/EUC/EUC-KR.esdb"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/esdb/EUC"
  COMPONENT data)

install(FILES
    "esdb/GB/GB18030.esdb"
    "esdb/GB/GBK.esdb"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/esdb/GB"
  COMPONENT data)

install(FILES
    "esdb/ISO-8859/ISO-8859-1.esdb"
    "esdb/ISO-8859/ISO-8859-11.esdb"
    "esdb/ISO-8859/ISO-8859-15.esdb"
    "esdb/ISO-8859/ISO-8859-2.esdb"
    "esdb/ISO-8859/ISO-8859-3.esdb"
    "esdb/ISO-8859/ISO-8859-4.esdb"
    "esdb/ISO-8859/ISO-8859-5.esdb"
    "esdb/ISO-8859/ISO-8859-6.esdb"
    "esdb/ISO-8859/ISO-8859-7.esdb"
    "esdb/ISO-8859/ISO-8859-8.esdb"
    "esdb/ISO-8859/ISO-8859-9.esdb"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/esdb/ISO-8859"
  COMPONENT data)

install(FILES
    "esdb/ISO646/ISO646-US.esdb"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/esdb/ISO646"
  COMPONENT data)

install(FILES
    "esdb/MISC/JISX0201-KANA.esdb"
    "esdb/MISC/JISX0208@1990.esdb"
    "esdb/MISC/JISX0212.esdb"
    "esdb/MISC/Shift_JIS.esdb"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/esdb/MISC"
  COMPONENT data)

install(FILES
    "esdb/UTF/UTF-16-INTERNAL.esdb"
    "esdb/UTF/UTF-16-SWAPPED.esdb"
    "esdb/UTF/UTF-16.esdb"
    "esdb/UTF/UTF-16BE.esdb"
    "esdb/UTF/UTF-16LE.esdb"
    "esdb/UTF/UTF-32-INTERNAL.esdb"
    "esdb/UTF/UTF-32-SWAPPED.esdb"
    "esdb/UTF/UTF-32.esdb"
    "esdb/UTF/UTF-32BE.esdb"
    "esdb/UTF/UTF-32LE.esdb"
    "esdb/UTF/UTF-8.esdb"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/esdb/UTF"
  COMPONENT data)

