# create library from source files
set(dcmpstat_src 
    dcmpstat.cc
    dviface.cc
    dvpsab.cc
    dvpsabl.cc
    dvpsal.cc
    dvpsall.cc
    dvpscf.cc
    dvpscu.cc
    dvpscul.cc
    dvpsda.cc
    dvpsdal.cc
    dvpsfs.cc
    dvpsga.cc
    dvpsgal.cc
    dvpsgl.cc
    dvpsgll.cc
    dvpsgr.cc
    dvpsgrl.cc
    dvpshlp.cc
    dvpsib.cc
    dvpsibl.cc
    dvpsmsg.cc
    dvpsov.cc
    dvpsovl.cc
    dvpspl2.cc
    dvpspl.cc
    dvpspll.cc
    dvpspr.cc
    dvpsprt.cc
    dvpsri.cc
    dvpsril.cc
    dvpsrs.cc
    dvpsrsl.cc
    dvpssp.cc
    dvpsspl.cc
    dvpssv.cc
    dvpssvl.cc
    dvpstat.cc
    dvpstx.cc
    dvpstxl.cc
    dvpsvl.cc
    dvpsvll.cc
    dvpsvw.cc
    dvpsvwl.cc
    dvsighdl.cc
)

DCMTK_ADD_LIBRARY(dcmpstat ${dcmpstat_src} )

DCMTK_TARGET_LINK_MODULES(dcmpstat ofstd oflog dcmdata dcmimgle dcmimage dcmnet dcmdsig dcmtls dcmsr dcmqrdb)
set_property(TARGET dcmpstat PROPERTY OUTPUT_NAME "dcmpstat${DCMTK_TLS_LIBRARY_POSTFIX}")
