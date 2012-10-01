# create library from source files
DCMTK_ADD_LIBRARY(dcmpstat dcmpstat dviface dvpsab dvpsabl dvpsal dvpsall dvpscf dvpscu dvpscul dvpsda dvpsdal dvpsfs dvpsga dvpsgal dvpsgl dvpsgll dvpsgr dvpsgrl dvpshlp dvpsib dvpsibl dvpsmsg dvpsov dvpsovl dvpspl dvpspl2 dvpspll dvpspr dvpsprt dvpsri dvpsril dvpsrs dvpsrsl dvpssp dvpsspl dvpssv dvpssvl dvpstat dvpstx dvpstxl dvpsvl dvpsvll dvpsvw dvpsvwl dvsighdl)

DCMTK_TARGET_LINK_MODULES(dcmpstat ofstd oflog dcmdata dcmimgle dcmimage dcmnet dcmdsig dcmtls dcmsr dcmqrdb)
