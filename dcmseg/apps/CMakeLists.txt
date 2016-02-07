# declare executables
FOREACH(PROGRAM makeseg rwseg)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM makeseg rwseg)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmseg dcmfg dcmiod dcmdata oflog ofstd)
ENDFOREACH(PROGRAM)
