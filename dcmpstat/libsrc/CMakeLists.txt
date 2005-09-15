# create library from source files
ADD_LIBRARY(dcmpstat dcmpstat dviface dvpsab dvpsabl dvpsal dvpsall dvpscf dvpscu dvpscul dvpsda dvpsdal dvpsfs dvpsga dvpsgal dvpsgl dvpsgll dvpsgr dvpsgrl dvpshlp dvpsib dvpsibl dvpsmsg dvpsov dvpsovl dvpspl dvpspl2 dvpspll dvpspr dvpsprt dvpsri dvpsril dvpsrs dvpsrsl dvpssp dvpsspl dvpssv dvpssvl dvpstat dvpstx dvpstxl dvpsvl dvpsvll dvpsvw dvpsvwl dvsighdl)

# declare installation files
INSTALL_TARGETS(${INSTALL_LIBDIR} dcmpstat)
