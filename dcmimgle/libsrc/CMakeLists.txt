# create library from source files
DCMTK_ADD_LIBRARY(dcmimgle
  dcmimage.cc
  dibaslut.cc
  diciefn.cc
  dicielut.cc
  didislut.cc
  didispfn.cc
  didocu.cc
  digsdfn.cc
  digsdlut.cc
  diimage.cc
  diinpx.cc
  diluptab.cc
  dimo1img.cc
  dimo2img.cc
  dimoimg.cc
  dimoimg3.cc
  dimoimg4.cc
  dimoimg5.cc
  dimomod.cc
  dimoopx.cc
  dimopx.cc
  diovdat.cc
  diovlay.cc
  diovlimg.cc
  diovpln.cc
  diutils.cc
)

DCMTK_TARGET_LINK_MODULES(dcmimgle ofstd oflog dcmdata)
