# create library from source files
set(dcmfg_src
    concatenationcreator.cc
    concatenationloader.cc
    fgbase.cc
    fg.cc
    fgctacquisitiondetails.cc
    fgctacquisitiontype.cc
    fgctadditionalxraysource.cc
    fgctexposure.cc
    fgctgeometry.cc
    fgctimageframetype.cc
    fgctposition.cc
    fgctreconstruction.cc
    fgcttabledynamics.cc
    fgctxraydetails.cc
    fgderimg.cc
    fgfact.cc
    fgfracon.cc
    fgframeanatomy.cc
    fgframevoilut.cc
    fgimagedatatype.cc
    fginterface.cc
    fgirradiationeventid.cc
    fgparametricmapframetype.cc
    fgpixeltransform.cc
    fgpixmsr.cc
    fgplanor.cc
    fgplanorvol.cc
    fgplanpo.cc
    fgplanposvol.cc
    fgrealworldvaluemapping.cc
    fgseg.cc
    fgtemporalposition.cc
    fgtypes.cc
    fgusimagedescription.cc
    stack.cc
    stackinterface.cc
)
DCMTK_ADD_LIBRARY(dcmfg ${dcmfg_src} )

DCMTK_TARGET_LINK_MODULES(dcmfg dcmiod dcmdata ofstd oflog)
