# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103
# TARGTYPE "Win32 (x86) Static Library" 0x0104

!IF "$(CFG)" == ""
CFG=dcmtk - Win32 Release
!MESSAGE No configuration specified.  Defaulting to dcmtk - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "dcmtk - Win32 Release" && "$(CFG)" != "dcmtk - Win32 Debug" &&\
 "$(CFG)" != "dcmdata - Win32 Release" && "$(CFG)" != "dcmdata - Win32 Debug" &&\
 "$(CFG)" != "mkdeftag - Win32 Release" && "$(CFG)" != "mkdeftag - Win32 Debug"\
 && "$(CFG)" != "mkdictbi - Win32 Release" && "$(CFG)" !=\
 "mkdictbi - Win32 Debug" && "$(CFG)" != "dcmdump - Win32 Release" && "$(CFG)"\
 != "dcmdump - Win32 Debug" && "$(CFG)" != "dcmconv - Win32 Release" && "$(CFG)"\
 != "dcmconv - Win32 Debug" && "$(CFG)" != "dump2dcm - Win32 Release" &&\
 "$(CFG)" != "dump2dcm - Win32 Debug" && "$(CFG)" != "dcmftest - Win32 Release"\
 && "$(CFG)" != "dcmftest - Win32 Debug" && "$(CFG)" !=\
 "dcmgpdir - Win32 Release" && "$(CFG)" != "dcmgpdir - Win32 Debug" && "$(CFG)"\
 != "dcmnet - Win32 Release" && "$(CFG)" != "dcmnet - Win32 Debug" && "$(CFG)"\
 != "echoscu - Win32 Release" && "$(CFG)" != "echoscu - Win32 Debug" && "$(CFG)"\
 != "storescp - Win32 Release" && "$(CFG)" != "storescp - Win32 Debug" &&\
 "$(CFG)" != "storescu - Win32 Release" && "$(CFG)" != "storescu - Win32 Debug"\
 && "$(CFG)" != "findscu - Win32 Release" && "$(CFG)" != "findscu - Win32 Debug"\
 && "$(CFG)" != "movescu - Win32 Release" && "$(CFG)" != "movescu - Win32 Debug"\
 && "$(CFG)" != "imagectnlib - Win32 Release" && "$(CFG)" !=\
 "imagectnlib - Win32 Debug" && "$(CFG)" != "assctest - Win32 Release" &&\
 "$(CFG)" != "assctest - Win32 Debug" && "$(CFG)" != "imagectn - Win32 Release"\
 && "$(CFG)" != "imagectn - Win32 Debug" && "$(CFG)" != "ti - Win32 Release" &&\
 "$(CFG)" != "ti - Win32 Debug" && "$(CFG)" != "dbregimg - Win32 Release" &&\
 "$(CFG)" != "dbregimg - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "msvc4.mak" CFG="dcmtk - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dcmtk - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "dcmtk - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "dcmdata - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "dcmdata - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mkdeftag - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "mkdeftag - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "mkdictbi - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "mkdictbi - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "dcmdump - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "dcmdump - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "dcmconv - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "dcmconv - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "dump2dcm - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "dump2dcm - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "dcmftest - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "dcmftest - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "dcmgpdir - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "dcmgpdir - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "dcmnet - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "dcmnet - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "echoscu - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "echoscu - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "storescp - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "storescp - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "storescu - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "storescu - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "findscu - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "findscu - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "movescu - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "movescu - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "imagectnlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "imagectnlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "assctest - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "assctest - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "imagectn - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "imagectn - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "ti - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "ti - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "dbregimg - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "dbregimg - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "movescu - Win32 Debug"

!IF  "$(CFG)" == "dcmtk - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "dbregimg - Win32 Release" "ti - Win32 Release"\
 "imagectn - Win32 Release" "assctest - Win32 Release"\
 "imagectnlib - Win32 Release" "movescu - Win32 Release"\
 "findscu - Win32 Release" "storescu - Win32 Release" "storescp - Win32 Release"\
 "echoscu - Win32 Release" "dcmnet - Win32 Release" "dcmgpdir - Win32 Release"\
 "dcmftest - Win32 Release" "dump2dcm - Win32 Release" "dcmconv - Win32 Release"\
 "dcmdump - Win32 Release" "mkdictbi - Win32 Release" "mkdeftag - Win32 Release"\
 "dcmdata - Win32 Release" "$(OUTDIR)\msvc4.lib"

CLEAN : 
	-@erase "$(OUTDIR)\msvc4.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)/msvc4.pch" /YX /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/msvc4.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/msvc4.lib" 
LIB32_OBJS= \
	".\dcmdata\libsrc\Release\dcmdata.lib" \
	".\dcmnet\libsrc\Release\dcmnet.lib" \
	".\imagectn\libsrc\Release\imagectnlib.lib"

"$(OUTDIR)\msvc4.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dcmtk - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "dbregimg - Win32 Debug" "ti - Win32 Debug" "imagectn - Win32 Debug"\
 "assctest - Win32 Debug" "imagectnlib - Win32 Debug" "movescu - Win32 Debug"\
 "findscu - Win32 Debug" "storescu - Win32 Debug" "storescp - Win32 Debug"\
 "echoscu - Win32 Debug" "dcmnet - Win32 Debug" "dcmgpdir - Win32 Debug"\
 "dcmftest - Win32 Debug" "dump2dcm - Win32 Debug" "dcmconv - Win32 Debug"\
 "dcmdump - Win32 Debug" "mkdictbi - Win32 Debug" "mkdeftag - Win32 Debug"\
 "dcmdata - Win32 Debug" "$(OUTDIR)\msvc4.lib"

CLEAN : 
	-@erase "$(OUTDIR)\msvc4.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /Z7 /Od /I "config/include" /I "dcmdata/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /Z7 /Od /I "config/include" /I "dcmdata/include"\
 /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)/msvc4.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/msvc4.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/msvc4.lib" 
LIB32_OBJS= \
	".\dcmdata\libsrc\Debug\dcmdata.lib" \
	".\dcmnet\libsrc\Debug\dcmnet.lib" \
	".\imagectn\libsrc\Debug\imagectnlib.lib"

"$(OUTDIR)\msvc4.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "dcmdata\Release"
# PROP BASE Intermediate_Dir "dcmdata\Release"
# PROP BASE Target_Dir "dcmdata"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "dcmdata\libsrc\Release"
# PROP Intermediate_Dir "dcmdata\libsrc\Release"
# PROP Target_Dir "dcmdata"
OUTDIR=.\dcmdata\libsrc\Release
INTDIR=.\dcmdata\libsrc\Release

ALL : "$(OUTDIR)\dcmdata.lib"

CLEAN : 
	-@erase "$(INTDIR)\cmdlnarg.obj"
	-@erase "$(INTDIR)\dcbuf.obj"
	-@erase "$(INTDIR)\dcbytstr.obj"
	-@erase "$(INTDIR)\dcchrstr.obj"
	-@erase "$(INTDIR)\dcdatset.obj"
	-@erase "$(INTDIR)\dcdebug.obj"
	-@erase "$(INTDIR)\dcdicdir.obj"
	-@erase "$(INTDIR)\dcdicent.obj"
	-@erase "$(INTDIR)\dcdict.obj"
	-@erase "$(INTDIR)\dcdictzz.obj"
	-@erase "$(INTDIR)\dcdirrec.obj"
	-@erase "$(INTDIR)\dcelem.obj"
	-@erase "$(INTDIR)\dcentbst.obj"
	-@erase "$(INTDIR)\dcentlst.obj"
	-@erase "$(INTDIR)\dcentset.obj"
	-@erase "$(INTDIR)\dcerror.obj"
	-@erase "$(INTDIR)\dcfilefo.obj"
	-@erase "$(INTDIR)\dcitem.obj"
	-@erase "$(INTDIR)\dclist.obj"
	-@erase "$(INTDIR)\dcmetinf.obj"
	-@erase "$(INTDIR)\dcobject.obj"
	-@erase "$(INTDIR)\dcpixseq.obj"
	-@erase "$(INTDIR)\dcpxitem.obj"
	-@erase "$(INTDIR)\dcsequen.obj"
	-@erase "$(INTDIR)\dcstack.obj"
	-@erase "$(INTDIR)\dcstream.obj"
	-@erase "$(INTDIR)\dcswap.obj"
	-@erase "$(INTDIR)\dctag.obj"
	-@erase "$(INTDIR)\dctagkey.obj"
	-@erase "$(INTDIR)\dcuid.obj"
	-@erase "$(INTDIR)\dcutils.obj"
	-@erase "$(INTDIR)\dcvm.obj"
	-@erase "$(INTDIR)\dcvr.obj"
	-@erase "$(INTDIR)\dcvrae.obj"
	-@erase "$(INTDIR)\dcvras.obj"
	-@erase "$(INTDIR)\dcvrat.obj"
	-@erase "$(INTDIR)\dcvrcs.obj"
	-@erase "$(INTDIR)\dcvrda.obj"
	-@erase "$(INTDIR)\dcvrds.obj"
	-@erase "$(INTDIR)\dcvrdt.obj"
	-@erase "$(INTDIR)\dcvrfd.obj"
	-@erase "$(INTDIR)\dcvrfl.obj"
	-@erase "$(INTDIR)\dcvris.obj"
	-@erase "$(INTDIR)\dcvrlo.obj"
	-@erase "$(INTDIR)\dcvrlt.obj"
	-@erase "$(INTDIR)\dcvrobow.obj"
	-@erase "$(INTDIR)\dcvrpn.obj"
	-@erase "$(INTDIR)\dcvrsh.obj"
	-@erase "$(INTDIR)\dcvrsl.obj"
	-@erase "$(INTDIR)\dcvrss.obj"
	-@erase "$(INTDIR)\dcvrst.obj"
	-@erase "$(INTDIR)\dcvrtm.obj"
	-@erase "$(INTDIR)\dcvrui.obj"
	-@erase "$(INTDIR)\dcvrul.obj"
	-@erase "$(INTDIR)\dcvrulup.obj"
	-@erase "$(INTDIR)\dcvrus.obj"
	-@erase "$(INTDIR)\dcxfer.obj"
	-@erase "$(OUTDIR)\dcmdata.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /D\
 "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)/dcmdata.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\dcmdata\libsrc\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dcmdata.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/dcmdata.lib" 
LIB32_OBJS= \
	"$(INTDIR)\cmdlnarg.obj" \
	"$(INTDIR)\dcbuf.obj" \
	"$(INTDIR)\dcbytstr.obj" \
	"$(INTDIR)\dcchrstr.obj" \
	"$(INTDIR)\dcdatset.obj" \
	"$(INTDIR)\dcdebug.obj" \
	"$(INTDIR)\dcdicdir.obj" \
	"$(INTDIR)\dcdicent.obj" \
	"$(INTDIR)\dcdict.obj" \
	"$(INTDIR)\dcdictzz.obj" \
	"$(INTDIR)\dcdirrec.obj" \
	"$(INTDIR)\dcelem.obj" \
	"$(INTDIR)\dcentbst.obj" \
	"$(INTDIR)\dcentlst.obj" \
	"$(INTDIR)\dcentset.obj" \
	"$(INTDIR)\dcerror.obj" \
	"$(INTDIR)\dcfilefo.obj" \
	"$(INTDIR)\dcitem.obj" \
	"$(INTDIR)\dclist.obj" \
	"$(INTDIR)\dcmetinf.obj" \
	"$(INTDIR)\dcobject.obj" \
	"$(INTDIR)\dcpixseq.obj" \
	"$(INTDIR)\dcpxitem.obj" \
	"$(INTDIR)\dcsequen.obj" \
	"$(INTDIR)\dcstack.obj" \
	"$(INTDIR)\dcstream.obj" \
	"$(INTDIR)\dcswap.obj" \
	"$(INTDIR)\dctag.obj" \
	"$(INTDIR)\dctagkey.obj" \
	"$(INTDIR)\dcuid.obj" \
	"$(INTDIR)\dcutils.obj" \
	"$(INTDIR)\dcvm.obj" \
	"$(INTDIR)\dcvr.obj" \
	"$(INTDIR)\dcvrae.obj" \
	"$(INTDIR)\dcvras.obj" \
	"$(INTDIR)\dcvrat.obj" \
	"$(INTDIR)\dcvrcs.obj" \
	"$(INTDIR)\dcvrda.obj" \
	"$(INTDIR)\dcvrds.obj" \
	"$(INTDIR)\dcvrdt.obj" \
	"$(INTDIR)\dcvrfd.obj" \
	"$(INTDIR)\dcvrfl.obj" \
	"$(INTDIR)\dcvris.obj" \
	"$(INTDIR)\dcvrlo.obj" \
	"$(INTDIR)\dcvrlt.obj" \
	"$(INTDIR)\dcvrobow.obj" \
	"$(INTDIR)\dcvrpn.obj" \
	"$(INTDIR)\dcvrsh.obj" \
	"$(INTDIR)\dcvrsl.obj" \
	"$(INTDIR)\dcvrss.obj" \
	"$(INTDIR)\dcvrst.obj" \
	"$(INTDIR)\dcvrtm.obj" \
	"$(INTDIR)\dcvrui.obj" \
	"$(INTDIR)\dcvrul.obj" \
	"$(INTDIR)\dcvrulup.obj" \
	"$(INTDIR)\dcvrus.obj" \
	"$(INTDIR)\dcxfer.obj"

"$(OUTDIR)\dcmdata.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "dcmdata\dcmdata_"
# PROP BASE Intermediate_Dir "dcmdata\dcmdata_"
# PROP BASE Target_Dir "dcmdata"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "dcmdata\libsrc\Debug"
# PROP Intermediate_Dir "dcmdata\libsrc\Debug"
# PROP Target_Dir "dcmdata"
OUTDIR=.\dcmdata\libsrc\Debug
INTDIR=.\dcmdata\libsrc\Debug

ALL : "$(OUTDIR)\dcmdata.lib"

CLEAN : 
	-@erase "$(INTDIR)\cmdlnarg.obj"
	-@erase "$(INTDIR)\dcbuf.obj"
	-@erase "$(INTDIR)\dcbytstr.obj"
	-@erase "$(INTDIR)\dcchrstr.obj"
	-@erase "$(INTDIR)\dcdatset.obj"
	-@erase "$(INTDIR)\dcdebug.obj"
	-@erase "$(INTDIR)\dcdicdir.obj"
	-@erase "$(INTDIR)\dcdicent.obj"
	-@erase "$(INTDIR)\dcdict.obj"
	-@erase "$(INTDIR)\dcdictzz.obj"
	-@erase "$(INTDIR)\dcdirrec.obj"
	-@erase "$(INTDIR)\dcelem.obj"
	-@erase "$(INTDIR)\dcentbst.obj"
	-@erase "$(INTDIR)\dcentlst.obj"
	-@erase "$(INTDIR)\dcentset.obj"
	-@erase "$(INTDIR)\dcerror.obj"
	-@erase "$(INTDIR)\dcfilefo.obj"
	-@erase "$(INTDIR)\dcitem.obj"
	-@erase "$(INTDIR)\dclist.obj"
	-@erase "$(INTDIR)\dcmetinf.obj"
	-@erase "$(INTDIR)\dcobject.obj"
	-@erase "$(INTDIR)\dcpixseq.obj"
	-@erase "$(INTDIR)\dcpxitem.obj"
	-@erase "$(INTDIR)\dcsequen.obj"
	-@erase "$(INTDIR)\dcstack.obj"
	-@erase "$(INTDIR)\dcstream.obj"
	-@erase "$(INTDIR)\dcswap.obj"
	-@erase "$(INTDIR)\dctag.obj"
	-@erase "$(INTDIR)\dctagkey.obj"
	-@erase "$(INTDIR)\dcuid.obj"
	-@erase "$(INTDIR)\dcutils.obj"
	-@erase "$(INTDIR)\dcvm.obj"
	-@erase "$(INTDIR)\dcvr.obj"
	-@erase "$(INTDIR)\dcvrae.obj"
	-@erase "$(INTDIR)\dcvras.obj"
	-@erase "$(INTDIR)\dcvrat.obj"
	-@erase "$(INTDIR)\dcvrcs.obj"
	-@erase "$(INTDIR)\dcvrda.obj"
	-@erase "$(INTDIR)\dcvrds.obj"
	-@erase "$(INTDIR)\dcvrdt.obj"
	-@erase "$(INTDIR)\dcvrfd.obj"
	-@erase "$(INTDIR)\dcvrfl.obj"
	-@erase "$(INTDIR)\dcvris.obj"
	-@erase "$(INTDIR)\dcvrlo.obj"
	-@erase "$(INTDIR)\dcvrlt.obj"
	-@erase "$(INTDIR)\dcvrobow.obj"
	-@erase "$(INTDIR)\dcvrpn.obj"
	-@erase "$(INTDIR)\dcvrsh.obj"
	-@erase "$(INTDIR)\dcvrsl.obj"
	-@erase "$(INTDIR)\dcvrss.obj"
	-@erase "$(INTDIR)\dcvrst.obj"
	-@erase "$(INTDIR)\dcvrtm.obj"
	-@erase "$(INTDIR)\dcvrui.obj"
	-@erase "$(INTDIR)\dcvrul.obj"
	-@erase "$(INTDIR)\dcvrulup.obj"
	-@erase "$(INTDIR)\dcvrus.obj"
	-@erase "$(INTDIR)\dcxfer.obj"
	-@erase "$(OUTDIR)\dcmdata.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /I "config/include" /I "dcmdata/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /I "config/include" /I "dcmdata/include"\
 /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)/dcmdata.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\dcmdata\libsrc\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dcmdata.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/dcmdata.lib" 
LIB32_OBJS= \
	"$(INTDIR)\cmdlnarg.obj" \
	"$(INTDIR)\dcbuf.obj" \
	"$(INTDIR)\dcbytstr.obj" \
	"$(INTDIR)\dcchrstr.obj" \
	"$(INTDIR)\dcdatset.obj" \
	"$(INTDIR)\dcdebug.obj" \
	"$(INTDIR)\dcdicdir.obj" \
	"$(INTDIR)\dcdicent.obj" \
	"$(INTDIR)\dcdict.obj" \
	"$(INTDIR)\dcdictzz.obj" \
	"$(INTDIR)\dcdirrec.obj" \
	"$(INTDIR)\dcelem.obj" \
	"$(INTDIR)\dcentbst.obj" \
	"$(INTDIR)\dcentlst.obj" \
	"$(INTDIR)\dcentset.obj" \
	"$(INTDIR)\dcerror.obj" \
	"$(INTDIR)\dcfilefo.obj" \
	"$(INTDIR)\dcitem.obj" \
	"$(INTDIR)\dclist.obj" \
	"$(INTDIR)\dcmetinf.obj" \
	"$(INTDIR)\dcobject.obj" \
	"$(INTDIR)\dcpixseq.obj" \
	"$(INTDIR)\dcpxitem.obj" \
	"$(INTDIR)\dcsequen.obj" \
	"$(INTDIR)\dcstack.obj" \
	"$(INTDIR)\dcstream.obj" \
	"$(INTDIR)\dcswap.obj" \
	"$(INTDIR)\dctag.obj" \
	"$(INTDIR)\dctagkey.obj" \
	"$(INTDIR)\dcuid.obj" \
	"$(INTDIR)\dcutils.obj" \
	"$(INTDIR)\dcvm.obj" \
	"$(INTDIR)\dcvr.obj" \
	"$(INTDIR)\dcvrae.obj" \
	"$(INTDIR)\dcvras.obj" \
	"$(INTDIR)\dcvrat.obj" \
	"$(INTDIR)\dcvrcs.obj" \
	"$(INTDIR)\dcvrda.obj" \
	"$(INTDIR)\dcvrds.obj" \
	"$(INTDIR)\dcvrdt.obj" \
	"$(INTDIR)\dcvrfd.obj" \
	"$(INTDIR)\dcvrfl.obj" \
	"$(INTDIR)\dcvris.obj" \
	"$(INTDIR)\dcvrlo.obj" \
	"$(INTDIR)\dcvrlt.obj" \
	"$(INTDIR)\dcvrobow.obj" \
	"$(INTDIR)\dcvrpn.obj" \
	"$(INTDIR)\dcvrsh.obj" \
	"$(INTDIR)\dcvrsl.obj" \
	"$(INTDIR)\dcvrss.obj" \
	"$(INTDIR)\dcvrst.obj" \
	"$(INTDIR)\dcvrtm.obj" \
	"$(INTDIR)\dcvrui.obj" \
	"$(INTDIR)\dcvrul.obj" \
	"$(INTDIR)\dcvrulup.obj" \
	"$(INTDIR)\dcvrus.obj" \
	"$(INTDIR)\dcxfer.obj"

"$(OUTDIR)\dcmdata.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mkdeftag - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mkdeftag\Release"
# PROP BASE Intermediate_Dir "mkdeftag\Release"
# PROP BASE Target_Dir "mkdeftag"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "dcmdata\libsrc\Release"
# PROP Intermediate_Dir "dcmdata\libsrc\Release"
# PROP Target_Dir "mkdeftag"
OUTDIR=.\dcmdata\libsrc\Release
INTDIR=.\dcmdata\libsrc\Release

ALL : "dcmdata - Win32 Release" "$(OUTDIR)\mkdeftag.exe"

CLEAN : 
	-@erase "$(INTDIR)\mkdeftag.obj"
	-@erase "$(OUTDIR)\mkdeftag.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /D\
 "WIN32" /D "NDEBUG" /D "_CONSOLE" /Fp"$(INTDIR)/mkdeftag.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\dcmdata\libsrc\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mkdeftag.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/mkdeftag.pdb" /machine:I386\
 /out:"$(OUTDIR)/mkdeftag.exe" 
LINK32_OBJS= \
	"$(INTDIR)\mkdeftag.obj" \
	"$(OUTDIR)\dcmdata.lib"

"$(OUTDIR)\mkdeftag.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mkdeftag - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "mkdeftag\Debug"
# PROP BASE Intermediate_Dir "mkdeftag\Debug"
# PROP BASE Target_Dir "mkdeftag"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "dcmdata\libsrc\Debug"
# PROP Intermediate_Dir "dcmdata\libsrc\Debug"
# PROP Target_Dir "mkdeftag"
OUTDIR=.\dcmdata\libsrc\Debug
INTDIR=.\dcmdata\libsrc\Debug

ALL : "dcmdata - Win32 Debug" "$(OUTDIR)\mkdeftag.exe"

CLEAN : 
	-@erase "$(INTDIR)\mkdeftag.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\mkdeftag.exe"
	-@erase "$(OUTDIR)\mkdeftag.ilk"
	-@erase "$(OUTDIR)\mkdeftag.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "config/include" /I "dcmdata/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "DEBUG" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "config/include" /I\
 "dcmdata/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "DEBUG"\
 /Fp"$(INTDIR)/mkdeftag.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\dcmdata\libsrc\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mkdeftag.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:yes /pdb:"$(OUTDIR)/mkdeftag.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/mkdeftag.exe" 
LINK32_OBJS= \
	"$(INTDIR)\mkdeftag.obj" \
	"$(OUTDIR)\dcmdata.lib"

"$(OUTDIR)\mkdeftag.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mkdictbi - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mkdictbi\Release"
# PROP BASE Intermediate_Dir "mkdictbi\Release"
# PROP BASE Target_Dir "mkdictbi"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "dcmdata\libsrc\Release"
# PROP Intermediate_Dir "dcmdata\libsrc\Release"
# PROP Target_Dir "mkdictbi"
OUTDIR=.\dcmdata\libsrc\Release
INTDIR=.\dcmdata\libsrc\Release

ALL : "dcmdata - Win32 Release" "$(OUTDIR)\mkdictbi.exe"

CLEAN : 
	-@erase "$(INTDIR)\mkdictbi.obj"
	-@erase "$(OUTDIR)\mkdictbi.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /D\
 "WIN32" /D "NDEBUG" /D "_CONSOLE" /Fp"$(INTDIR)/mkdictbi.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\dcmdata\libsrc\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mkdictbi.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/mkdictbi.pdb" /machine:I386\
 /out:"$(OUTDIR)/mkdictbi.exe" 
LINK32_OBJS= \
	"$(INTDIR)\mkdictbi.obj" \
	"$(OUTDIR)\dcmdata.lib"

"$(OUTDIR)\mkdictbi.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mkdictbi - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "mkdictbi\Debug"
# PROP BASE Intermediate_Dir "mkdictbi\Debug"
# PROP BASE Target_Dir "mkdictbi"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "dcmdata\libsrc\Debug"
# PROP Intermediate_Dir "dcmdata\libsrc\Debug"
# PROP Target_Dir "mkdictbi"
OUTDIR=.\dcmdata\libsrc\Debug
INTDIR=.\dcmdata\libsrc\Debug

ALL : "dcmdata - Win32 Debug" "$(OUTDIR)\mkdictbi.exe"

CLEAN : 
	-@erase "$(INTDIR)\mkdictbi.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\mkdictbi.exe"
	-@erase "$(OUTDIR)\mkdictbi.ilk"
	-@erase "$(OUTDIR)\mkdictbi.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "config/include" /I "dcmdata/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "DEBUG" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "config/include" /I\
 "dcmdata/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "DEBUG"\
 /Fp"$(INTDIR)/mkdictbi.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\dcmdata\libsrc\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mkdictbi.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:yes /pdb:"$(OUTDIR)/mkdictbi.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/mkdictbi.exe" 
LINK32_OBJS= \
	"$(INTDIR)\mkdictbi.obj" \
	"$(OUTDIR)\dcmdata.lib"

"$(OUTDIR)\mkdictbi.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dcmdump - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "dcmdump\Release"
# PROP BASE Intermediate_Dir "dcmdump\Release"
# PROP BASE Target_Dir "dcmdump"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "dcmdata\apps\Release"
# PROP Intermediate_Dir "dcmdata\apps\Release"
# PROP Target_Dir "dcmdump"
OUTDIR=.\dcmdata\apps\Release
INTDIR=.\dcmdata\apps\Release

ALL : "dcmdata - Win32 Release" "$(OUTDIR)\dcmdump.exe"

CLEAN : 
	-@erase "$(INTDIR)\dcmdump.obj"
	-@erase "$(OUTDIR)\dcmdump.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /D\
 "WIN32" /D "NDEBUG" /D "_CONSOLE" /Fp"$(INTDIR)/dcmdump.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\dcmdata\apps\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dcmdump.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/dcmdump.pdb" /machine:I386\
 /out:"$(OUTDIR)/dcmdump.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dcmdump.obj" \
	".\dcmdata\libsrc\Release\dcmdata.lib"

"$(OUTDIR)\dcmdump.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dcmdump - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "dcmdump\Debug"
# PROP BASE Intermediate_Dir "dcmdump\Debug"
# PROP BASE Target_Dir "dcmdump"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "dcmdata\apps\Debug"
# PROP Intermediate_Dir "dcmdata\apps\Debug"
# PROP Target_Dir "dcmdump"
OUTDIR=.\dcmdata\apps\Debug
INTDIR=.\dcmdata\apps\Debug

ALL : "dcmdata - Win32 Debug" "$(OUTDIR)\dcmdump.exe"

CLEAN : 
	-@erase "$(INTDIR)\dcmdump.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\dcmdump.exe"
	-@erase "$(OUTDIR)\dcmdump.ilk"
	-@erase "$(OUTDIR)\dcmdump.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "config/include" /I "dcmdata/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "DEBUG" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "config/include" /I\
 "dcmdata/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "DEBUG"\
 /Fp"$(INTDIR)/dcmdump.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\dcmdata\apps\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dcmdump.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:yes /pdb:"$(OUTDIR)/dcmdump.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/dcmdump.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dcmdump.obj" \
	".\dcmdata\libsrc\Debug\dcmdata.lib"

"$(OUTDIR)\dcmdump.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dcmconv - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "dcmconv\Release"
# PROP BASE Intermediate_Dir "dcmconv\Release"
# PROP BASE Target_Dir "dcmconv"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "dcmdata\apps\Release"
# PROP Intermediate_Dir "dcmdata\apps\Release"
# PROP Target_Dir "dcmconv"
OUTDIR=.\dcmdata\apps\Release
INTDIR=.\dcmdata\apps\Release

ALL : "dcmdata - Win32 Release" "$(OUTDIR)\dcmconv.exe"

CLEAN : 
	-@erase "$(INTDIR)\dcmconv.obj"
	-@erase "$(OUTDIR)\dcmconv.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /D\
 "WIN32" /D "NDEBUG" /D "_CONSOLE" /Fp"$(INTDIR)/dcmconv.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\dcmdata\apps\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dcmconv.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/dcmconv.pdb" /machine:I386\
 /out:"$(OUTDIR)/dcmconv.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dcmconv.obj" \
	".\dcmdata\libsrc\Release\dcmdata.lib"

"$(OUTDIR)\dcmconv.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dcmconv - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "dcmconv\Debug"
# PROP BASE Intermediate_Dir "dcmconv\Debug"
# PROP BASE Target_Dir "dcmconv"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "dcmdata\apps\Debug"
# PROP Intermediate_Dir "dcmdata\apps\Debug"
# PROP Target_Dir "dcmconv"
OUTDIR=.\dcmdata\apps\Debug
INTDIR=.\dcmdata\apps\Debug

ALL : "dcmdata - Win32 Debug" "$(OUTDIR)\dcmconv.exe"

CLEAN : 
	-@erase "$(INTDIR)\dcmconv.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\dcmconv.exe"
	-@erase "$(OUTDIR)\dcmconv.ilk"
	-@erase "$(OUTDIR)\dcmconv.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "config/include" /I "dcmdata/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "DEBUG" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "config/include" /I\
 "dcmdata/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "DEBUG"\
 /Fp"$(INTDIR)/dcmconv.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\dcmdata\apps\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dcmconv.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:yes /pdb:"$(OUTDIR)/dcmconv.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/dcmconv.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dcmconv.obj" \
	".\dcmdata\libsrc\Debug\dcmdata.lib"

"$(OUTDIR)\dcmconv.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dump2dcm - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "dump2dcm\Release"
# PROP BASE Intermediate_Dir "dump2dcm\Release"
# PROP BASE Target_Dir "dump2dcm"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "dcmdata\apps\Release"
# PROP Intermediate_Dir "dcmdata\apps\Release"
# PROP Target_Dir "dump2dcm"
OUTDIR=.\dcmdata\apps\Release
INTDIR=.\dcmdata\apps\Release

ALL : "dcmdata - Win32 Release" "$(OUTDIR)\dump2dcm.exe"

CLEAN : 
	-@erase "$(INTDIR)\dump2dcm.obj"
	-@erase "$(OUTDIR)\dump2dcm.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /D\
 "WIN32" /D "NDEBUG" /D "_CONSOLE" /Fp"$(INTDIR)/dump2dcm.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\dcmdata\apps\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dump2dcm.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/dump2dcm.pdb" /machine:I386\
 /out:"$(OUTDIR)/dump2dcm.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dump2dcm.obj" \
	".\dcmdata\libsrc\Release\dcmdata.lib"

"$(OUTDIR)\dump2dcm.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dump2dcm - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "dump2dcm\Debug"
# PROP BASE Intermediate_Dir "dump2dcm\Debug"
# PROP BASE Target_Dir "dump2dcm"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "dcmdata\apps\Debug"
# PROP Intermediate_Dir "dcmdata\apps\Debug"
# PROP Target_Dir "dump2dcm"
OUTDIR=.\dcmdata\apps\Debug
INTDIR=.\dcmdata\apps\Debug

ALL : "dcmdata - Win32 Debug" "$(OUTDIR)\dump2dcm.exe"

CLEAN : 
	-@erase "$(INTDIR)\dump2dcm.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\dump2dcm.exe"
	-@erase "$(OUTDIR)\dump2dcm.ilk"
	-@erase "$(OUTDIR)\dump2dcm.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "config/include" /I "dcmdata/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "DEBUG" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "config/include" /I\
 "dcmdata/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "DEBUG"\
 /Fp"$(INTDIR)/dump2dcm.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\dcmdata\apps\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dump2dcm.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:yes /pdb:"$(OUTDIR)/dump2dcm.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/dump2dcm.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dump2dcm.obj" \
	".\dcmdata\libsrc\Debug\dcmdata.lib"

"$(OUTDIR)\dump2dcm.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dcmftest - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "dcmftest\Release"
# PROP BASE Intermediate_Dir "dcmftest\Release"
# PROP BASE Target_Dir "dcmftest"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "dcmdata\apps\Release"
# PROP Intermediate_Dir "dcmdata\apps\Release"
# PROP Target_Dir "dcmftest"
OUTDIR=.\dcmdata\apps\Release
INTDIR=.\dcmdata\apps\Release

ALL : "dcmdata - Win32 Release" "$(OUTDIR)\dcmftest.exe"

CLEAN : 
	-@erase "$(INTDIR)\dcmftest.obj"
	-@erase "$(OUTDIR)\dcmftest.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /D\
 "WIN32" /D "NDEBUG" /D "_CONSOLE" /Fp"$(INTDIR)/dcmftest.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\dcmdata\apps\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dcmftest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/dcmftest.pdb" /machine:I386\
 /out:"$(OUTDIR)/dcmftest.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dcmftest.obj" \
	".\dcmdata\libsrc\Release\dcmdata.lib"

"$(OUTDIR)\dcmftest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dcmftest - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "dcmftest\Debug"
# PROP BASE Intermediate_Dir "dcmftest\Debug"
# PROP BASE Target_Dir "dcmftest"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "dcmdata\apps\Debug"
# PROP Intermediate_Dir "dcmdata\apps\Debug"
# PROP Target_Dir "dcmftest"
OUTDIR=.\dcmdata\apps\Debug
INTDIR=.\dcmdata\apps\Debug

ALL : "dcmdata - Win32 Debug" "$(OUTDIR)\dcmftest.exe"

CLEAN : 
	-@erase "$(INTDIR)\dcmftest.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\dcmftest.exe"
	-@erase "$(OUTDIR)\dcmftest.ilk"
	-@erase "$(OUTDIR)\dcmftest.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "config/include" /I "dcmdata/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "DEBUG" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "config/include" /I\
 "dcmdata/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "DEBUG"\
 /Fp"$(INTDIR)/dcmftest.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\dcmdata\apps\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dcmftest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /subsystem:console /incremental:yes /pdb:"$(OUTDIR)/dcmftest.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/dcmftest.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dcmftest.obj" \
	".\dcmdata\libsrc\Debug\dcmdata.lib"

"$(OUTDIR)\dcmftest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dcmgpdir - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "dcmgpdir\Release"
# PROP BASE Intermediate_Dir "dcmgpdir\Release"
# PROP BASE Target_Dir "dcmgpdir"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "dcmdata\apps\Release"
# PROP Intermediate_Dir "dcmdata\apps\Release"
# PROP Target_Dir "dcmgpdir"
OUTDIR=.\dcmdata\apps\Release
INTDIR=.\dcmdata\apps\Release

ALL : "dcmdata - Win32 Release" "$(OUTDIR)\dcmgpdir.exe"

CLEAN : 
	-@erase "$(INTDIR)\dcmgpdir.obj"
	-@erase "$(OUTDIR)\dcmgpdir.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /D\
 "WIN32" /D "NDEBUG" /D "_CONSOLE" /Fp"$(INTDIR)/dcmgpdir.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\dcmdata\apps\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dcmgpdir.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/dcmgpdir.pdb" /machine:I386\
 /out:"$(OUTDIR)/dcmgpdir.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dcmgpdir.obj" \
	".\dcmdata\libsrc\Release\dcmdata.lib"

"$(OUTDIR)\dcmgpdir.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dcmgpdir - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "dcmgpdir\Debug"
# PROP BASE Intermediate_Dir "dcmgpdir\Debug"
# PROP BASE Target_Dir "dcmgpdir"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "dcmdata\apps\Debug"
# PROP Intermediate_Dir "dcmdata\apps\Debug"
# PROP Target_Dir "dcmgpdir"
OUTDIR=.\dcmdata\apps\Debug
INTDIR=.\dcmdata\apps\Debug

ALL : "dcmdata - Win32 Debug" "$(OUTDIR)\dcmgpdir.exe" "$(OUTDIR)\dcmgpdir.bsc"

CLEAN : 
	-@erase "$(INTDIR)\dcmgpdir.obj"
	-@erase "$(INTDIR)\dcmgpdir.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\dcmgpdir.bsc"
	-@erase "$(OUTDIR)\dcmgpdir.exe"
	-@erase "$(OUTDIR)\dcmgpdir.ilk"
	-@erase "$(OUTDIR)\dcmgpdir.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "config/include" /I "dcmdata/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "DEBUG" /FR /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "config/include" /I\
 "dcmdata/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "DEBUG"\
 /FR"$(INTDIR)/" /Fp"$(INTDIR)/dcmgpdir.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/"\
 /c 
CPP_OBJS=.\dcmdata\apps\Debug/
CPP_SBRS=.\dcmdata\apps\Debug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dcmgpdir.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dcmgpdir.sbr"

"$(OUTDIR)\dcmgpdir.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:yes /pdb:"$(OUTDIR)/dcmgpdir.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/dcmgpdir.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dcmgpdir.obj" \
	".\dcmdata\libsrc\Debug\dcmdata.lib"

"$(OUTDIR)\dcmgpdir.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dcmnet - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "dcmnet\Release"
# PROP BASE Intermediate_Dir "dcmnet\Release"
# PROP BASE Target_Dir "dcmnet"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "dcmnet\libsrc\Release"
# PROP Intermediate_Dir "dcmnet\libsrc\Release"
# PROP Target_Dir "dcmnet"
OUTDIR=.\dcmnet\libsrc\Release
INTDIR=.\dcmnet\libsrc\Release

ALL : "$(OUTDIR)\dcmnet.lib"

CLEAN : 
	-@erase "$(INTDIR)\asccond.obj"
	-@erase "$(INTDIR)\assoc.obj"
	-@erase "$(INTDIR)\cond.obj"
	-@erase "$(INTDIR)\dcompat.obj"
	-@erase "$(INTDIR)\dimcancl.obj"
	-@erase "$(INTDIR)\dimcmd.obj"
	-@erase "$(INTDIR)\dimcond.obj"
	-@erase "$(INTDIR)\dimdump.obj"
	-@erase "$(INTDIR)\dimecho.obj"
	-@erase "$(INTDIR)\dimfind.obj"
	-@erase "$(INTDIR)\dimmove.obj"
	-@erase "$(INTDIR)\dimse.obj"
	-@erase "$(INTDIR)\dimstore.obj"
	-@erase "$(INTDIR)\diutil.obj"
	-@erase "$(INTDIR)\dul.obj"
	-@erase "$(INTDIR)\dulcond.obj"
	-@erase "$(INTDIR)\dulconst.obj"
	-@erase "$(INTDIR)\dulextra.obj"
	-@erase "$(INTDIR)\dulfsm.obj"
	-@erase "$(INTDIR)\dulparse.obj"
	-@erase "$(INTDIR)\dulpres.obj"
	-@erase "$(INTDIR)\lst.obj"
	-@erase "$(INTDIR)\lstcond.obj"
	-@erase "$(OUTDIR)\dcmnet.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /I "dcmnet/include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /I\
 "dcmnet/include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)/dcmnet.pch"\
 /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\dcmnet\libsrc\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dcmnet.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/dcmnet.lib" 
LIB32_OBJS= \
	"$(INTDIR)\asccond.obj" \
	"$(INTDIR)\assoc.obj" \
	"$(INTDIR)\cond.obj" \
	"$(INTDIR)\dcompat.obj" \
	"$(INTDIR)\dimcancl.obj" \
	"$(INTDIR)\dimcmd.obj" \
	"$(INTDIR)\dimcond.obj" \
	"$(INTDIR)\dimdump.obj" \
	"$(INTDIR)\dimecho.obj" \
	"$(INTDIR)\dimfind.obj" \
	"$(INTDIR)\dimmove.obj" \
	"$(INTDIR)\dimse.obj" \
	"$(INTDIR)\dimstore.obj" \
	"$(INTDIR)\diutil.obj" \
	"$(INTDIR)\dul.obj" \
	"$(INTDIR)\dulcond.obj" \
	"$(INTDIR)\dulconst.obj" \
	"$(INTDIR)\dulextra.obj" \
	"$(INTDIR)\dulfsm.obj" \
	"$(INTDIR)\dulparse.obj" \
	"$(INTDIR)\dulpres.obj" \
	"$(INTDIR)\lst.obj" \
	"$(INTDIR)\lstcond.obj"

"$(OUTDIR)\dcmnet.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "dcmnet\Debug"
# PROP BASE Intermediate_Dir "dcmnet\Debug"
# PROP BASE Target_Dir "dcmnet"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "dcmnet\libsrc\Debug"
# PROP Intermediate_Dir "dcmnet\libsrc\Debug"
# PROP Target_Dir "dcmnet"
OUTDIR=.\dcmnet\libsrc\Debug
INTDIR=.\dcmnet\libsrc\Debug

ALL : "$(OUTDIR)\dcmnet.lib"

CLEAN : 
	-@erase "$(INTDIR)\asccond.obj"
	-@erase "$(INTDIR)\assoc.obj"
	-@erase "$(INTDIR)\cond.obj"
	-@erase "$(INTDIR)\dcompat.obj"
	-@erase "$(INTDIR)\dimcancl.obj"
	-@erase "$(INTDIR)\dimcmd.obj"
	-@erase "$(INTDIR)\dimcond.obj"
	-@erase "$(INTDIR)\dimdump.obj"
	-@erase "$(INTDIR)\dimecho.obj"
	-@erase "$(INTDIR)\dimfind.obj"
	-@erase "$(INTDIR)\dimmove.obj"
	-@erase "$(INTDIR)\dimse.obj"
	-@erase "$(INTDIR)\dimstore.obj"
	-@erase "$(INTDIR)\diutil.obj"
	-@erase "$(INTDIR)\dul.obj"
	-@erase "$(INTDIR)\dulcond.obj"
	-@erase "$(INTDIR)\dulconst.obj"
	-@erase "$(INTDIR)\dulextra.obj"
	-@erase "$(INTDIR)\dulfsm.obj"
	-@erase "$(INTDIR)\dulparse.obj"
	-@erase "$(INTDIR)\dulpres.obj"
	-@erase "$(INTDIR)\lst.obj"
	-@erase "$(INTDIR)\lstcond.obj"
	-@erase "$(OUTDIR)\dcmnet.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /I "config/include" /I "dcmdata/include" /I "dcmnet/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "DEBUG" /YX /c
CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /I "config/include" /I "dcmdata/include"\
 /I "dcmnet/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "DEBUG"\
 /Fp"$(INTDIR)/dcmnet.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\dcmnet\libsrc\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dcmnet.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/dcmnet.lib" 
LIB32_OBJS= \
	"$(INTDIR)\asccond.obj" \
	"$(INTDIR)\assoc.obj" \
	"$(INTDIR)\cond.obj" \
	"$(INTDIR)\dcompat.obj" \
	"$(INTDIR)\dimcancl.obj" \
	"$(INTDIR)\dimcmd.obj" \
	"$(INTDIR)\dimcond.obj" \
	"$(INTDIR)\dimdump.obj" \
	"$(INTDIR)\dimecho.obj" \
	"$(INTDIR)\dimfind.obj" \
	"$(INTDIR)\dimmove.obj" \
	"$(INTDIR)\dimse.obj" \
	"$(INTDIR)\dimstore.obj" \
	"$(INTDIR)\diutil.obj" \
	"$(INTDIR)\dul.obj" \
	"$(INTDIR)\dulcond.obj" \
	"$(INTDIR)\dulconst.obj" \
	"$(INTDIR)\dulextra.obj" \
	"$(INTDIR)\dulfsm.obj" \
	"$(INTDIR)\dulparse.obj" \
	"$(INTDIR)\dulpres.obj" \
	"$(INTDIR)\lst.obj" \
	"$(INTDIR)\lstcond.obj"

"$(OUTDIR)\dcmnet.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "echoscu - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "echoscu\Release"
# PROP BASE Intermediate_Dir "echoscu\Release"
# PROP BASE Target_Dir "echoscu"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "dcmnet\apps\Release"
# PROP Intermediate_Dir "dcmnet\apps\Release"
# PROP Target_Dir "echoscu"
OUTDIR=.\dcmnet\apps\Release
INTDIR=.\dcmnet\apps\Release

ALL : "dcmnet - Win32 Release" "dcmdata - Win32 Release"\
 "$(OUTDIR)\echoscu.exe"

CLEAN : 
	-@erase "$(INTDIR)\echoscu.obj"
	-@erase "$(OUTDIR)\echoscu.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /I "dcmnet/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /I\
 "dcmnet/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/echoscu.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\dcmnet\apps\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/echoscu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/echoscu.pdb" /machine:I386\
 /out:"$(OUTDIR)/echoscu.exe" 
LINK32_OBJS= \
	"$(INTDIR)\echoscu.obj" \
	".\dcmdata\libsrc\Release\dcmdata.lib" \
	".\dcmnet\libsrc\Release\dcmnet.lib"

"$(OUTDIR)\echoscu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "echoscu - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "echoscu\Debug"
# PROP BASE Intermediate_Dir "echoscu\Debug"
# PROP BASE Target_Dir "echoscu"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "dcmnet\apps\Debug"
# PROP Intermediate_Dir "dcmnet\apps\Debug"
# PROP Target_Dir "echoscu"
OUTDIR=.\dcmnet\apps\Debug
INTDIR=.\dcmnet\apps\Debug

ALL : "dcmnet - Win32 Debug" "dcmdata - Win32 Debug" "$(OUTDIR)\echoscu.exe"

CLEAN : 
	-@erase "$(INTDIR)\echoscu.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\echoscu.exe"
	-@erase "$(OUTDIR)\echoscu.ilk"
	-@erase "$(OUTDIR)\echoscu.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "config/include" /I "dcmdata/include" /I "dcmnet/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "DEBUG" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "config/include" /I\
 "dcmdata/include" /I "dcmnet/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D\
 "DEBUG" /Fp"$(INTDIR)/echoscu.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\dcmnet\apps\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/echoscu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:yes /pdb:"$(OUTDIR)/echoscu.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/echoscu.exe" 
LINK32_OBJS= \
	"$(INTDIR)\echoscu.obj" \
	".\dcmdata\libsrc\Debug\dcmdata.lib" \
	".\dcmnet\libsrc\Debug\dcmnet.lib"

"$(OUTDIR)\echoscu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "storescp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "storescp\Release"
# PROP BASE Intermediate_Dir "storescp\Release"
# PROP BASE Target_Dir "storescp"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "dcmnet\apps\Release"
# PROP Intermediate_Dir "dcmnet\apps\Release"
# PROP Target_Dir "storescp"
OUTDIR=.\dcmnet\apps\Release
INTDIR=.\dcmnet\apps\Release

ALL : "dcmnet - Win32 Release" "dcmdata - Win32 Release"\
 "$(OUTDIR)\storescp.exe"

CLEAN : 
	-@erase "$(INTDIR)\storescp.obj"
	-@erase "$(OUTDIR)\storescp.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /I "dcmnet/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /I\
 "dcmnet/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/storescp.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\dcmnet\apps\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/storescp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/storescp.pdb" /machine:I386\
 /out:"$(OUTDIR)/storescp.exe" 
LINK32_OBJS= \
	"$(INTDIR)\storescp.obj" \
	".\dcmdata\libsrc\Release\dcmdata.lib" \
	".\dcmnet\libsrc\Release\dcmnet.lib"

"$(OUTDIR)\storescp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "storescp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "storescp\Debug"
# PROP BASE Intermediate_Dir "storescp\Debug"
# PROP BASE Target_Dir "storescp"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "dcmnet\apps\Debug"
# PROP Intermediate_Dir "dcmnet\apps\Debug"
# PROP Target_Dir "storescp"
OUTDIR=.\dcmnet\apps\Debug
INTDIR=.\dcmnet\apps\Debug

ALL : "dcmnet - Win32 Debug" "dcmdata - Win32 Debug" "$(OUTDIR)\storescp.exe"

CLEAN : 
	-@erase "$(INTDIR)\storescp.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\storescp.exe"
	-@erase "$(OUTDIR)\storescp.ilk"
	-@erase "$(OUTDIR)\storescp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "config/include" /I "dcmdata/include" /I "dcmnet/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "DEBUG" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "config/include" /I\
 "dcmdata/include" /I "dcmnet/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D\
 "DEBUG" /Fp"$(INTDIR)/storescp.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\dcmnet\apps\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/storescp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:yes /pdb:"$(OUTDIR)/storescp.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/storescp.exe" 
LINK32_OBJS= \
	"$(INTDIR)\storescp.obj" \
	".\dcmdata\libsrc\Debug\dcmdata.lib" \
	".\dcmnet\libsrc\Debug\dcmnet.lib"

"$(OUTDIR)\storescp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "storescu - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "storescu\Release"
# PROP BASE Intermediate_Dir "storescu\Release"
# PROP BASE Target_Dir "storescu"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "dcmnet\apps\Release"
# PROP Intermediate_Dir "dcmnet\apps\Release"
# PROP Target_Dir "storescu"
OUTDIR=.\dcmnet\apps\Release
INTDIR=.\dcmnet\apps\Release

ALL : "dcmnet - Win32 Release" "dcmdata - Win32 Release"\
 "$(OUTDIR)\storescu.exe"

CLEAN : 
	-@erase "$(INTDIR)\storescu.obj"
	-@erase "$(OUTDIR)\storescu.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /I "dcmnet/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /I\
 "dcmnet/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/storescu.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\dcmnet\apps\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/storescu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/storescu.pdb" /machine:I386\
 /out:"$(OUTDIR)/storescu.exe" 
LINK32_OBJS= \
	"$(INTDIR)\storescu.obj" \
	".\dcmdata\libsrc\Release\dcmdata.lib" \
	".\dcmnet\libsrc\Release\dcmnet.lib"

"$(OUTDIR)\storescu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "storescu - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "storescu\Debug"
# PROP BASE Intermediate_Dir "storescu\Debug"
# PROP BASE Target_Dir "storescu"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "dcmnet\apps\Debug"
# PROP Intermediate_Dir "dcmnet\apps\Debug"
# PROP Target_Dir "storescu"
OUTDIR=.\dcmnet\apps\Debug
INTDIR=.\dcmnet\apps\Debug

ALL : "dcmnet - Win32 Debug" "dcmdata - Win32 Debug" "$(OUTDIR)\storescu.exe"

CLEAN : 
	-@erase "$(INTDIR)\storescu.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\storescu.exe"
	-@erase "$(OUTDIR)\storescu.ilk"
	-@erase "$(OUTDIR)\storescu.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "config/include" /I "dcmdata/include" /I "dcmnet/include" /D "WIN32" /D "_DEBUG" /D "_CONSOL" /D "DEBUG" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "config/include" /I\
 "dcmdata/include" /I "dcmnet/include" /D "WIN32" /D "_DEBUG" /D "_CONSOL" /D\
 "DEBUG" /Fp"$(INTDIR)/storescu.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\dcmnet\apps\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/storescu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:yes /pdb:"$(OUTDIR)/storescu.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/storescu.exe" 
LINK32_OBJS= \
	"$(INTDIR)\storescu.obj" \
	".\dcmdata\libsrc\Debug\dcmdata.lib" \
	".\dcmnet\libsrc\Debug\dcmnet.lib"

"$(OUTDIR)\storescu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "findscu - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "findscu\Release"
# PROP BASE Intermediate_Dir "findscu\Release"
# PROP BASE Target_Dir "findscu"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "dcmnet\apps\Release"
# PROP Intermediate_Dir "dcmnet\apps\Release"
# PROP Target_Dir "findscu"
OUTDIR=.\dcmnet\apps\Release
INTDIR=.\dcmnet\apps\Release

ALL : "dcmnet - Win32 Release" "dcmdata - Win32 Release"\
 "$(OUTDIR)\findscu.exe"

CLEAN : 
	-@erase "$(INTDIR)\findscu.obj"
	-@erase "$(OUTDIR)\findscu.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /I "dcmnet/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /I\
 "dcmnet/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/findscu.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\dcmnet\apps\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/findscu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/findscu.pdb" /machine:I386\
 /out:"$(OUTDIR)/findscu.exe" 
LINK32_OBJS= \
	"$(INTDIR)\findscu.obj" \
	".\dcmdata\libsrc\Release\dcmdata.lib" \
	".\dcmnet\libsrc\Release\dcmnet.lib"

"$(OUTDIR)\findscu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "findscu - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "findscu\Debug"
# PROP BASE Intermediate_Dir "findscu\Debug"
# PROP BASE Target_Dir "findscu"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "dcmnet\apps\Debug"
# PROP Intermediate_Dir "dcmnet\apps\Debug"
# PROP Target_Dir "findscu"
OUTDIR=.\dcmnet\apps\Debug
INTDIR=.\dcmnet\apps\Debug

ALL : "dcmnet - Win32 Debug" "dcmdata - Win32 Debug" "$(OUTDIR)\findscu.exe"

CLEAN : 
	-@erase "$(INTDIR)\findscu.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\findscu.exe"
	-@erase "$(OUTDIR)\findscu.ilk"
	-@erase "$(OUTDIR)\findscu.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "config/include" /I "dcmdata/include" /I "dcmnet/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "DEBUG" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "config/include" /I\
 "dcmdata/include" /I "dcmnet/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D\
 "DEBUG" /Fp"$(INTDIR)/findscu.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\dcmnet\apps\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/findscu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:yes /pdb:"$(OUTDIR)/findscu.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/findscu.exe" 
LINK32_OBJS= \
	"$(INTDIR)\findscu.obj" \
	".\dcmdata\libsrc\Debug\dcmdata.lib" \
	".\dcmnet\libsrc\Debug\dcmnet.lib"

"$(OUTDIR)\findscu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "movescu - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "movescu\Release"
# PROP BASE Intermediate_Dir "movescu\Release"
# PROP BASE Target_Dir "movescu"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "dcmnet\apps\Release"
# PROP Intermediate_Dir "dcmnet\apps\Release"
# PROP Target_Dir "movescu"
OUTDIR=.\dcmnet\apps\Release
INTDIR=.\dcmnet\apps\Release

ALL : "dcmnet - Win32 Release" "dcmdata - Win32 Release"\
 "$(OUTDIR)\movescu.exe"

CLEAN : 
	-@erase "$(INTDIR)\movescu.obj"
	-@erase "$(OUTDIR)\movescu.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /I "dcmnet/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /I\
 "dcmnet/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/movescu.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\dcmnet\apps\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/movescu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/movescu.pdb" /machine:I386\
 /out:"$(OUTDIR)/movescu.exe" 
LINK32_OBJS= \
	"$(INTDIR)\movescu.obj" \
	".\dcmdata\libsrc\Release\dcmdata.lib" \
	".\dcmnet\libsrc\Release\dcmnet.lib"

"$(OUTDIR)\movescu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "movescu - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "movescu\Debug"
# PROP BASE Intermediate_Dir "movescu\Debug"
# PROP BASE Target_Dir "movescu"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "dcmnet\apps\Debug"
# PROP Intermediate_Dir "dcmnet\apps\Debug"
# PROP Target_Dir "movescu"
OUTDIR=.\dcmnet\apps\Debug
INTDIR=.\dcmnet\apps\Debug

ALL : "dcmnet - Win32 Debug" "dcmdata - Win32 Debug" "$(OUTDIR)\movescu.exe"\
 "$(OUTDIR)\movescu.bsc"

CLEAN : 
	-@erase "$(INTDIR)\movescu.obj"
	-@erase "$(INTDIR)\movescu.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\movescu.bsc"
	-@erase "$(OUTDIR)\movescu.exe"
	-@erase "$(OUTDIR)\movescu.ilk"
	-@erase "$(OUTDIR)\movescu.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "config/include" /I "dcmdata/include" /I "dcmnet/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "DEBUG" /FR /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "config/include" /I\
 "dcmdata/include" /I "dcmnet/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D\
 "DEBUG" /FR"$(INTDIR)/" /Fp"$(INTDIR)/movescu.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\dcmnet\apps\Debug/
CPP_SBRS=.\dcmnet\apps\Debug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/movescu.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\movescu.sbr"

"$(OUTDIR)\movescu.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:yes /pdb:"$(OUTDIR)/movescu.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/movescu.exe" 
LINK32_OBJS= \
	"$(INTDIR)\movescu.obj" \
	".\dcmdata\libsrc\Debug\dcmdata.lib" \
	".\dcmnet\libsrc\Debug\dcmnet.lib"

"$(OUTDIR)\movescu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "imagectnlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "imagectnlib\Release"
# PROP BASE Intermediate_Dir "imagectnlib\Release"
# PROP BASE Target_Dir "imagectnlib"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "imagectn\libsrc\Release"
# PROP Intermediate_Dir "imagectn\libsrc\Release"
# PROP Target_Dir "imagectnlib"
OUTDIR=.\imagectn\libsrc\Release
INTDIR=.\imagectn\libsrc\Release

ALL : "$(OUTDIR)\imagectnlib.lib"

CLEAN : 
	-@erase "$(INTDIR)\dbcond.obj"
	-@erase "$(INTDIR)\dbfind.obj"
	-@erase "$(INTDIR)\dbindex.obj"
	-@erase "$(INTDIR)\dbmove.obj"
	-@erase "$(INTDIR)\dbstore.obj"
	-@erase "$(INTDIR)\dbutils.obj"
	-@erase "$(OUTDIR)\imagectnlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /I "dcmnet/include" /I "imagectn/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "NODISPLAY" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /I\
 "dcmnet/include" /I "imagectn/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "NODISPLAY" /Fp"$(INTDIR)/imagectnlib.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\imagectn\libsrc\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/imagectnlib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/imagectnlib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\dbcond.obj" \
	"$(INTDIR)\dbfind.obj" \
	"$(INTDIR)\dbindex.obj" \
	"$(INTDIR)\dbmove.obj" \
	"$(INTDIR)\dbstore.obj" \
	"$(INTDIR)\dbutils.obj"

"$(OUTDIR)\imagectnlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "imagectnlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "imagectnlib\Debug"
# PROP BASE Intermediate_Dir "imagectnlib\Debug"
# PROP BASE Target_Dir "imagectnlib"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "imagectn\libsrc\Debug"
# PROP Intermediate_Dir "imagectn\libsrc\Debug"
# PROP Target_Dir "imagectnlib"
OUTDIR=.\imagectn\libsrc\Debug
INTDIR=.\imagectn\libsrc\Debug

ALL : "$(OUTDIR)\imagectnlib.lib" "$(OUTDIR)\imagectnlib.bsc"

CLEAN : 
	-@erase "$(INTDIR)\dbcond.obj"
	-@erase "$(INTDIR)\dbcond.sbr"
	-@erase "$(INTDIR)\dbfind.obj"
	-@erase "$(INTDIR)\dbfind.sbr"
	-@erase "$(INTDIR)\dbindex.obj"
	-@erase "$(INTDIR)\dbindex.sbr"
	-@erase "$(INTDIR)\dbmove.obj"
	-@erase "$(INTDIR)\dbmove.sbr"
	-@erase "$(INTDIR)\dbstore.obj"
	-@erase "$(INTDIR)\dbstore.sbr"
	-@erase "$(INTDIR)\dbutils.obj"
	-@erase "$(INTDIR)\dbutils.sbr"
	-@erase "$(OUTDIR)\imagectnlib.bsc"
	-@erase "$(OUTDIR)\imagectnlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /I "config/include" /I "dcmdata/include" /I "dcmnet/include" /I "imagectn/include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "NODISPLAY" /D "DEBUG" /FR /YX /c
CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /I "config/include" /I "dcmdata/include"\
 /I "dcmnet/include" /I "imagectn/include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "NODISPLAY" /D "DEBUG" /FR"$(INTDIR)/" /Fp"$(INTDIR)/imagectnlib.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\imagectn\libsrc\Debug/
CPP_SBRS=.\imagectn\libsrc\Debug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/imagectnlib.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dbcond.sbr" \
	"$(INTDIR)\dbfind.sbr" \
	"$(INTDIR)\dbindex.sbr" \
	"$(INTDIR)\dbmove.sbr" \
	"$(INTDIR)\dbstore.sbr" \
	"$(INTDIR)\dbutils.sbr"

"$(OUTDIR)\imagectnlib.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/imagectnlib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\dbcond.obj" \
	"$(INTDIR)\dbfind.obj" \
	"$(INTDIR)\dbindex.obj" \
	"$(INTDIR)\dbmove.obj" \
	"$(INTDIR)\dbstore.obj" \
	"$(INTDIR)\dbutils.obj"

"$(OUTDIR)\imagectnlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "assctest - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "assctest\Release"
# PROP BASE Intermediate_Dir "assctest\Release"
# PROP BASE Target_Dir "assctest"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "dcmnet\tests\Release"
# PROP Intermediate_Dir "dcmnet\tests\Release"
# PROP Target_Dir "assctest"
OUTDIR=.\dcmnet\tests\Release
INTDIR=.\dcmnet\tests\Release

ALL : "dcmnet - Win32 Release" "dcmdata - Win32 Release"\
 "$(OUTDIR)\assctest.exe"

CLEAN : 
	-@erase "$(INTDIR)\assctest.obj"
	-@erase "$(OUTDIR)\assctest.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /I "dcmnet/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /I\
 "dcmnet/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/assctest.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\dcmnet\tests\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/assctest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/assctest.pdb" /machine:I386\
 /out:"$(OUTDIR)/assctest.exe" 
LINK32_OBJS= \
	"$(INTDIR)\assctest.obj" \
	".\dcmdata\libsrc\Release\dcmdata.lib" \
	".\dcmnet\libsrc\Release\dcmnet.lib"

"$(OUTDIR)\assctest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "assctest - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "assctest\Debug"
# PROP BASE Intermediate_Dir "assctest\Debug"
# PROP BASE Target_Dir "assctest"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "dcmnet\tests\Debug"
# PROP Intermediate_Dir "dcmnet\tests\Debug"
# PROP Target_Dir "assctest"
OUTDIR=.\dcmnet\tests\Debug
INTDIR=.\dcmnet\tests\Debug

ALL : "dcmnet - Win32 Debug" "dcmdata - Win32 Debug" "$(OUTDIR)\assctest.exe"

CLEAN : 
	-@erase "$(INTDIR)\assctest.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\assctest.exe"
	-@erase "$(OUTDIR)\assctest.ilk"
	-@erase "$(OUTDIR)\assctest.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "config/include" /I "dcmdata/include" /I "dcmnet/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "DEBUG" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "config/include" /I\
 "dcmdata/include" /I "dcmnet/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D\
 "DEBUG" /Fp"$(INTDIR)/assctest.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\dcmnet\tests\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/assctest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:yes /pdb:"$(OUTDIR)/assctest.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/assctest.exe" 
LINK32_OBJS= \
	"$(INTDIR)\assctest.obj" \
	".\dcmdata\libsrc\Debug\dcmdata.lib" \
	".\dcmnet\libsrc\Debug\dcmnet.lib"

"$(OUTDIR)\assctest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "imagectn - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "imagectn\Release"
# PROP BASE Intermediate_Dir "imagectn\Release"
# PROP BASE Target_Dir "imagectn"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "imagectn\apps\Release"
# PROP Intermediate_Dir "imagectn\apps\Release"
# PROP Target_Dir "imagectn"
OUTDIR=.\imagectn\apps\Release
INTDIR=.\imagectn\apps\Release

ALL : "imagectnlib - Win32 Release" "dcmnet - Win32 Release"\
 "dcmdata - Win32 Release" "$(OUTDIR)\imagectn.exe"

CLEAN : 
	-@erase "$(INTDIR)\cnf.obj"
	-@erase "$(INTDIR)\cnfexnt.obj"
	-@erase "$(INTDIR)\cnfpriv.obj"
	-@erase "$(INTDIR)\imagectn.obj"
	-@erase "$(INTDIR)\sce.obj"
	-@erase "$(INTDIR)\sceecho.obj"
	-@erase "$(INTDIR)\scefind.obj"
	-@erase "$(INTDIR)\scemove.obj"
	-@erase "$(INTDIR)\scestore.obj"
	-@erase "$(OUTDIR)\imagectn.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /I "dcmnet/include" /I "imagectn/include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "NODISPLAY" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /I\
 "dcmnet/include" /I "imagectn/include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D\
 "NODISPLAY" /Fp"$(INTDIR)/imagectn.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\imagectn\apps\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/imagectn.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/imagectn.pdb" /machine:I386\
 /out:"$(OUTDIR)/imagectn.exe" 
LINK32_OBJS= \
	"$(INTDIR)\cnf.obj" \
	"$(INTDIR)\cnfexnt.obj" \
	"$(INTDIR)\cnfpriv.obj" \
	"$(INTDIR)\imagectn.obj" \
	"$(INTDIR)\sce.obj" \
	"$(INTDIR)\sceecho.obj" \
	"$(INTDIR)\scefind.obj" \
	"$(INTDIR)\scemove.obj" \
	"$(INTDIR)\scestore.obj" \
	".\dcmdata\libsrc\Release\dcmdata.lib" \
	".\dcmnet\libsrc\Release\dcmnet.lib" \
	".\imagectn\libsrc\Release\imagectnlib.lib"

"$(OUTDIR)\imagectn.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "imagectn - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "imagectn\Debug"
# PROP BASE Intermediate_Dir "imagectn\Debug"
# PROP BASE Target_Dir "imagectn"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "imagectn\apps\Debug"
# PROP Intermediate_Dir "imagectn\apps\Debug"
# PROP Target_Dir "imagectn"
OUTDIR=.\imagectn\apps\Debug
INTDIR=.\imagectn\apps\Debug

ALL : "imagectnlib - Win32 Debug" "dcmnet - Win32 Debug"\
 "dcmdata - Win32 Debug" "$(OUTDIR)\imagectn.exe"

CLEAN : 
	-@erase "$(INTDIR)\cnf.obj"
	-@erase "$(INTDIR)\cnfexnt.obj"
	-@erase "$(INTDIR)\cnfpriv.obj"
	-@erase "$(INTDIR)\imagectn.obj"
	-@erase "$(INTDIR)\sce.obj"
	-@erase "$(INTDIR)\sceecho.obj"
	-@erase "$(INTDIR)\scefind.obj"
	-@erase "$(INTDIR)\scemove.obj"
	-@erase "$(INTDIR)\scestore.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\imagectn.exe"
	-@erase "$(OUTDIR)\imagectn.ilk"
	-@erase "$(OUTDIR)\imagectn.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "config/include" /I "dcmdata/include" /I "dcmnet/include" /I "imagectn/include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "NODISPLAY" /D "DEBUG" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "config/include" /I\
 "dcmdata/include" /I "dcmnet/include" /I "imagectn/include" /D "_DEBUG" /D\
 "WIN32" /D "_CONSOLE" /D "NODISPLAY" /D "DEBUG" /Fp"$(INTDIR)/imagectn.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\imagectn\apps\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/imagectn.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:yes /pdb:"$(OUTDIR)/imagectn.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/imagectn.exe" 
LINK32_OBJS= \
	"$(INTDIR)\cnf.obj" \
	"$(INTDIR)\cnfexnt.obj" \
	"$(INTDIR)\cnfpriv.obj" \
	"$(INTDIR)\imagectn.obj" \
	"$(INTDIR)\sce.obj" \
	"$(INTDIR)\sceecho.obj" \
	"$(INTDIR)\scefind.obj" \
	"$(INTDIR)\scemove.obj" \
	"$(INTDIR)\scestore.obj" \
	".\dcmdata\libsrc\Debug\dcmdata.lib" \
	".\dcmnet\libsrc\Debug\dcmnet.lib" \
	".\imagectn\libsrc\Debug\imagectnlib.lib"

"$(OUTDIR)\imagectn.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ti - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ti\Release"
# PROP BASE Intermediate_Dir "ti\Release"
# PROP BASE Target_Dir "ti"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "imagectn\apps\Release"
# PROP Intermediate_Dir "imagectn\apps\Release"
# PROP Target_Dir "ti"
OUTDIR=.\imagectn\apps\Release
INTDIR=.\imagectn\apps\Release

ALL : "imagectnlib - Win32 Release" "dcmnet - Win32 Release"\
 "dcmdata - Win32 Release" "$(OUTDIR)\ti.exe"

CLEAN : 
	-@erase "$(INTDIR)\cnf.obj"
	-@erase "$(INTDIR)\cnfexnt.obj"
	-@erase "$(INTDIR)\cnfpriv.obj"
	-@erase "$(INTDIR)\ti.obj"
	-@erase "$(INTDIR)\tinet.obj"
	-@erase "$(INTDIR)\tiquery.obj"
	-@erase "$(INTDIR)\tiui.obj"
	-@erase "$(OUTDIR)\ti.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /I "dcmnet/include" /I "imagectn/include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "NODISPLAY" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /I\
 "dcmnet/include" /I "imagectn/include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D\
 "NODISPLAY" /Fp"$(INTDIR)/ti.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\imagectn\apps\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ti.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/ti.pdb" /machine:I386\
 /out:"$(OUTDIR)/ti.exe" 
LINK32_OBJS= \
	"$(INTDIR)\cnf.obj" \
	"$(INTDIR)\cnfexnt.obj" \
	"$(INTDIR)\cnfpriv.obj" \
	"$(INTDIR)\ti.obj" \
	"$(INTDIR)\tinet.obj" \
	"$(INTDIR)\tiquery.obj" \
	"$(INTDIR)\tiui.obj" \
	".\dcmdata\libsrc\Release\dcmdata.lib" \
	".\dcmnet\libsrc\Release\dcmnet.lib" \
	".\imagectn\libsrc\Release\imagectnlib.lib"

"$(OUTDIR)\ti.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ti - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ti\Debug"
# PROP BASE Intermediate_Dir "ti\Debug"
# PROP BASE Target_Dir "ti"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "imagectn\apps\Debug"
# PROP Intermediate_Dir "imagectn\apps\Debug"
# PROP Target_Dir "ti"
OUTDIR=.\imagectn\apps\Debug
INTDIR=.\imagectn\apps\Debug

ALL : "imagectnlib - Win32 Debug" "dcmnet - Win32 Debug"\
 "dcmdata - Win32 Debug" "$(OUTDIR)\ti.exe" "$(OUTDIR)\ti.bsc"

CLEAN : 
	-@erase "$(INTDIR)\cnf.obj"
	-@erase "$(INTDIR)\cnf.sbr"
	-@erase "$(INTDIR)\cnfexnt.obj"
	-@erase "$(INTDIR)\cnfexnt.sbr"
	-@erase "$(INTDIR)\cnfpriv.obj"
	-@erase "$(INTDIR)\cnfpriv.sbr"
	-@erase "$(INTDIR)\ti.obj"
	-@erase "$(INTDIR)\ti.sbr"
	-@erase "$(INTDIR)\tinet.obj"
	-@erase "$(INTDIR)\tinet.sbr"
	-@erase "$(INTDIR)\tiquery.obj"
	-@erase "$(INTDIR)\tiquery.sbr"
	-@erase "$(INTDIR)\tiui.obj"
	-@erase "$(INTDIR)\tiui.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\ti.bsc"
	-@erase "$(OUTDIR)\ti.exe"
	-@erase "$(OUTDIR)\ti.ilk"
	-@erase "$(OUTDIR)\ti.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "config/include" /I "dcmdata/include" /I "dcmnet/include" /I "imagectn/include" /D "_DEBUG" /D "DEBUG" /D "WIN32" /D "_CONSOLE" /D "NODISPLAY" /FR /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "config/include" /I\
 "dcmdata/include" /I "dcmnet/include" /I "imagectn/include" /D "_DEBUG" /D\
 "DEBUG" /D "WIN32" /D "_CONSOLE" /D "NODISPLAY" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/ti.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\imagectn\apps\Debug/
CPP_SBRS=.\imagectn\apps\Debug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ti.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\cnf.sbr" \
	"$(INTDIR)\cnfexnt.sbr" \
	"$(INTDIR)\cnfpriv.sbr" \
	"$(INTDIR)\ti.sbr" \
	"$(INTDIR)\tinet.sbr" \
	"$(INTDIR)\tiquery.sbr" \
	"$(INTDIR)\tiui.sbr"

"$(OUTDIR)\ti.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:yes /pdb:"$(OUTDIR)/ti.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/ti.exe" 
LINK32_OBJS= \
	"$(INTDIR)\cnf.obj" \
	"$(INTDIR)\cnfexnt.obj" \
	"$(INTDIR)\cnfpriv.obj" \
	"$(INTDIR)\ti.obj" \
	"$(INTDIR)\tinet.obj" \
	"$(INTDIR)\tiquery.obj" \
	"$(INTDIR)\tiui.obj" \
	".\dcmdata\libsrc\Debug\dcmdata.lib" \
	".\dcmnet\libsrc\Debug\dcmnet.lib" \
	".\imagectn\libsrc\Debug\imagectnlib.lib"

"$(OUTDIR)\ti.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dbregimg - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "dbregimg\Release"
# PROP BASE Intermediate_Dir "dbregimg\Release"
# PROP BASE Target_Dir "dbregimg"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "imagectn\tests\Release"
# PROP Intermediate_Dir "imagectn\tests\Release"
# PROP Target_Dir "dbregimg"
OUTDIR=.\imagectn\tests\Release
INTDIR=.\imagectn\tests\Release

ALL : "dcmnet - Win32 Release" "imagectnlib - Win32 Release"\
 "dcmdata - Win32 Release" "$(OUTDIR)\dbregimg.exe"

CLEAN : 
	-@erase "$(INTDIR)\dbregimg.obj"
	-@erase "$(OUTDIR)\dbregimg.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /I "dcmnet/include" /I "imagectn/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "config/include" /I "dcmdata/include" /I\
 "dcmnet/include" /I "imagectn/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/dbregimg.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\imagectn\tests\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dbregimg.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/dbregimg.pdb" /machine:I386 /out:"$(OUTDIR)/dbregimg.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dbregimg.obj" \
	".\dcmdata\libsrc\Release\dcmdata.lib" \
	".\dcmnet\libsrc\Release\dcmnet.lib" \
	".\imagectn\libsrc\Release\imagectnlib.lib"

"$(OUTDIR)\dbregimg.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dbregimg - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "dbregimg\Debug"
# PROP BASE Intermediate_Dir "dbregimg\Debug"
# PROP BASE Target_Dir "dbregimg"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "imagectn\tests\Debug"
# PROP Intermediate_Dir "imagectn\tests\Debug"
# PROP Target_Dir "dbregimg"
OUTDIR=.\imagectn\tests\Debug
INTDIR=.\imagectn\tests\Debug

ALL : "dcmnet - Win32 Debug" "imagectnlib - Win32 Debug"\
 "dcmdata - Win32 Debug" "$(OUTDIR)\dbregimg.exe"

CLEAN : 
	-@erase "$(INTDIR)\dbregimg.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\dbregimg.exe"
	-@erase "$(OUTDIR)\dbregimg.ilk"
	-@erase "$(OUTDIR)\dbregimg.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "config/include" /I "dcmdata/include" /I "dcmnet/include" /I "imagectn/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "DEBUG" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "config/include" /I\
 "dcmdata/include" /I "dcmnet/include" /I "imagectn/include" /D "WIN32" /D\
 "_DEBUG" /D "_CONSOLE" /D "DEBUG" /Fp"$(INTDIR)/dbregimg.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\imagectn\tests\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dbregimg.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:yes /pdb:"$(OUTDIR)/dbregimg.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/dbregimg.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dbregimg.obj" \
	".\dcmdata\libsrc\Debug\dcmdata.lib" \
	".\dcmnet\libsrc\Debug\dcmnet.lib" \
	".\imagectn\libsrc\Debug\imagectnlib.lib"

"$(OUTDIR)\dbregimg.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "dcmtk - Win32 Release"
# Name "dcmtk - Win32 Debug"

!IF  "$(CFG)" == "dcmtk - Win32 Release"

!ELSEIF  "$(CFG)" == "dcmtk - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmdata"

!IF  "$(CFG)" == "dcmtk - Win32 Release"

"dcmdata - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Release" 

!ELSEIF  "$(CFG)" == "dcmtk - Win32 Debug"

"dcmdata - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "mkdeftag"

!IF  "$(CFG)" == "dcmtk - Win32 Release"

"mkdeftag - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="mkdeftag - Win32 Release" 

!ELSEIF  "$(CFG)" == "dcmtk - Win32 Debug"

"mkdeftag - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="mkdeftag - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "mkdictbi"

!IF  "$(CFG)" == "dcmtk - Win32 Release"

"mkdictbi - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="mkdictbi - Win32 Release" 

!ELSEIF  "$(CFG)" == "dcmtk - Win32 Debug"

"mkdictbi - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="mkdictbi - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmdump"

!IF  "$(CFG)" == "dcmtk - Win32 Release"

"dcmdump - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdump - Win32 Release" 

!ELSEIF  "$(CFG)" == "dcmtk - Win32 Debug"

"dcmdump - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdump - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmconv"

!IF  "$(CFG)" == "dcmtk - Win32 Release"

"dcmconv - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmconv - Win32 Release" 

!ELSEIF  "$(CFG)" == "dcmtk - Win32 Debug"

"dcmconv - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmconv - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "dump2dcm"

!IF  "$(CFG)" == "dcmtk - Win32 Release"

"dump2dcm - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dump2dcm - Win32 Release" 

!ELSEIF  "$(CFG)" == "dcmtk - Win32 Debug"

"dump2dcm - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dump2dcm - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmftest"

!IF  "$(CFG)" == "dcmtk - Win32 Release"

"dcmftest - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmftest - Win32 Release" 

!ELSEIF  "$(CFG)" == "dcmtk - Win32 Debug"

"dcmftest - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmftest - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmgpdir"

!IF  "$(CFG)" == "dcmtk - Win32 Release"

"dcmgpdir - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmgpdir - Win32 Release" 

!ELSEIF  "$(CFG)" == "dcmtk - Win32 Debug"

"dcmgpdir - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmgpdir - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmnet"

!IF  "$(CFG)" == "dcmtk - Win32 Release"

"dcmnet - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmnet - Win32 Release" 

!ELSEIF  "$(CFG)" == "dcmtk - Win32 Debug"

"dcmnet - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmnet - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "echoscu"

!IF  "$(CFG)" == "dcmtk - Win32 Release"

"echoscu - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="echoscu - Win32 Release" 

!ELSEIF  "$(CFG)" == "dcmtk - Win32 Debug"

"echoscu - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="echoscu - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "storescp"

!IF  "$(CFG)" == "dcmtk - Win32 Release"

"storescp - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="storescp - Win32 Release" 

!ELSEIF  "$(CFG)" == "dcmtk - Win32 Debug"

"storescp - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="storescp - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "storescu"

!IF  "$(CFG)" == "dcmtk - Win32 Release"

"storescu - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="storescu - Win32 Release" 

!ELSEIF  "$(CFG)" == "dcmtk - Win32 Debug"

"storescu - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="storescu - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "findscu"

!IF  "$(CFG)" == "dcmtk - Win32 Release"

"findscu - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="findscu - Win32 Release" 

!ELSEIF  "$(CFG)" == "dcmtk - Win32 Debug"

"findscu - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="findscu - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "movescu"

!IF  "$(CFG)" == "dcmtk - Win32 Release"

"movescu - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="movescu - Win32 Release" 

!ELSEIF  "$(CFG)" == "dcmtk - Win32 Debug"

"movescu - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="movescu - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "imagectnlib"

!IF  "$(CFG)" == "dcmtk - Win32 Release"

"imagectnlib - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="imagectnlib - Win32 Release" 

!ELSEIF  "$(CFG)" == "dcmtk - Win32 Debug"

"imagectnlib - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="imagectnlib - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "assctest"

!IF  "$(CFG)" == "dcmtk - Win32 Release"

"assctest - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="assctest - Win32 Release" 

!ELSEIF  "$(CFG)" == "dcmtk - Win32 Debug"

"assctest - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="assctest - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "imagectn"

!IF  "$(CFG)" == "dcmtk - Win32 Release"

"imagectn - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="imagectn - Win32 Release" 

!ELSEIF  "$(CFG)" == "dcmtk - Win32 Debug"

"imagectn - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="imagectn - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "ti"

!IF  "$(CFG)" == "dcmtk - Win32 Release"

"ti - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="ti - Win32 Release" 

!ELSEIF  "$(CFG)" == "dcmtk - Win32 Debug"

"ti - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="ti - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "dbregimg"

!IF  "$(CFG)" == "dcmtk - Win32 Release"

"dbregimg - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dbregimg - Win32 Release" 

!ELSEIF  "$(CFG)" == "dcmtk - Win32 Debug"

"dbregimg - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dbregimg - Win32 Debug" 

!ENDIF 

# End Project Dependency
# End Target
################################################################################
# Begin Target

# Name "dcmdata - Win32 Release"
# Name "dcmdata - Win32 Debug"

!IF  "$(CFG)" == "dcmdata - Win32 Release"

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\cmdlnarg.cxx
DEP_CPP_CMDLN=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\cmdlnarg.h"\
	

"$(INTDIR)\cmdlnarg.obj" : $(SOURCE) $(DEP_CPP_CMDLN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcbuf.cxx
DEP_CPP_DCBUF=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbuf.h"\
	".\dcmdata\include\dcdefine.h"\
	

"$(INTDIR)\dcbuf.obj" : $(SOURCE) $(DEP_CPP_DCBUF) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcbytstr.cxx
DEP_CPP_DCBYT=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcbytstr.obj" : $(SOURCE) $(DEP_CPP_DCBYT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcchrstr.cxx
DEP_CPP_DCCHR=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcchrstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcchrstr.obj" : $(SOURCE) $(DEP_CPP_DCCHR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcdatset.cxx
DEP_CPP_DCDAT=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcvrus.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcdatset.obj" : $(SOURCE) $(DEP_CPP_DCDAT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcdebug.cxx
DEP_CPP_DCDEB=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	

"$(INTDIR)\dcdebug.obj" : $(SOURCE) $(DEP_CPP_DCDEB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcdicdir.cxx
DEP_CPP_DCDIC=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\cmdlnarg.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctk.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcchrstr.h"\
	".\dcmdata\include\dcdefine.h"\
	".\dcmdata\include\dcdicdir.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcdirrec.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcpixseq.h"\
	".\dcmdata\include\dcpxitem.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dcswap.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvm.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrae.h"\
	".\dcmdata\include\dcvras.h"\
	".\dcmdata\include\dcvrat.h"\
	".\dcmdata\include\dcvrcs.h"\
	".\dcmdata\include\dcvrda.h"\
	".\dcmdata\include\dcvrds.h"\
	".\dcmdata\include\dcvrdt.h"\
	".\dcmdata\include\dcvrfd.h"\
	".\dcmdata\include\dcvrfl.h"\
	".\dcmdata\include\dcvris.h"\
	".\dcmdata\include\dcvrlo.h"\
	".\dcmdata\include\dcvrlt.h"\
	".\dcmdata\include\dcvrobow.h"\
	".\dcmdata\include\dcvrpn.h"\
	".\dcmdata\include\dcvrsh.h"\
	".\dcmdata\include\dcvrsl.h"\
	".\dcmdata\include\dcvrss.h"\
	".\dcmdata\include\dcvrst.h"\
	".\dcmdata\include\dcvrtm.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcvrul.h"\
	".\dcmdata\include\dcvrulup.h"\
	".\dcmdata\include\dcvrus.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmdata\include\pix.h"\
	

"$(INTDIR)\dcdicdir.obj" : $(SOURCE) $(DEP_CPP_DCDIC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcdicent.cxx
DEP_CPP_DCDICE=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	

"$(INTDIR)\dcdicent.obj" : $(SOURCE) $(DEP_CPP_DCDICE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcdict.cxx
DEP_CPP_DCDICT=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcdefine.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\pix.h"\
	

"$(INTDIR)\dcdict.obj" : $(SOURCE) $(DEP_CPP_DCDICT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcdictzz.cxx
DEP_CPP_DCDICTZ=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\pix.h"\
	

"$(INTDIR)\dcdictzz.obj" : $(SOURCE) $(DEP_CPP_DCDICTZ) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcdirrec.cxx
DEP_CPP_DCDIR=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\cmdlnarg.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctk.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcchrstr.h"\
	".\dcmdata\include\dcdefine.h"\
	".\dcmdata\include\dcdicdir.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcdirrec.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcpixseq.h"\
	".\dcmdata\include\dcpxitem.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dcswap.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvm.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrae.h"\
	".\dcmdata\include\dcvras.h"\
	".\dcmdata\include\dcvrat.h"\
	".\dcmdata\include\dcvrcs.h"\
	".\dcmdata\include\dcvrda.h"\
	".\dcmdata\include\dcvrds.h"\
	".\dcmdata\include\dcvrdt.h"\
	".\dcmdata\include\dcvrfd.h"\
	".\dcmdata\include\dcvrfl.h"\
	".\dcmdata\include\dcvris.h"\
	".\dcmdata\include\dcvrlo.h"\
	".\dcmdata\include\dcvrlt.h"\
	".\dcmdata\include\dcvrobow.h"\
	".\dcmdata\include\dcvrpn.h"\
	".\dcmdata\include\dcvrsh.h"\
	".\dcmdata\include\dcvrsl.h"\
	".\dcmdata\include\dcvrss.h"\
	".\dcmdata\include\dcvrst.h"\
	".\dcmdata\include\dcvrtm.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcvrul.h"\
	".\dcmdata\include\dcvrulup.h"\
	".\dcmdata\include\dcvrus.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmdata\include\pix.h"\
	

"$(INTDIR)\dcdirrec.obj" : $(SOURCE) $(DEP_CPP_DCDIR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcelem.cxx
DEP_CPP_DCELE=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcdefine.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcelem.obj" : $(SOURCE) $(DEP_CPP_DCELE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcentbst.cxx
DEP_CPP_DCENT=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\pix.h"\
	

"$(INTDIR)\dcentbst.obj" : $(SOURCE) $(DEP_CPP_DCENT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcentlst.cxx
DEP_CPP_DCENTL=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\pix.h"\
	

"$(INTDIR)\dcentlst.obj" : $(SOURCE) $(DEP_CPP_DCENTL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcentset.cxx
DEP_CPP_DCENTS=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\pix.h"\
	

"$(INTDIR)\dcentset.obj" : $(SOURCE) $(DEP_CPP_DCENTS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcerror.cxx
DEP_CPP_DCERR=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcerror.h"\
	

"$(INTDIR)\dcerror.obj" : $(SOURCE) $(DEP_CPP_DCERR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcfilefo.cxx
DEP_CPP_DCFIL=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcchrstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrae.h"\
	".\dcmdata\include\dcvrobow.h"\
	".\dcmdata\include\dcvrsh.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcvrul.h"\
	".\dcmdata\include\dcvrus.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcfilefo.obj" : $(SOURCE) $(DEP_CPP_DCFIL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcitem.cxx
DEP_CPP_DCITE=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\cmdlnarg.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctk.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcchrstr.h"\
	".\dcmdata\include\dcdefine.h"\
	".\dcmdata\include\dcdicdir.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcdirrec.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcpixseq.h"\
	".\dcmdata\include\dcpxitem.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dcswap.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvm.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrae.h"\
	".\dcmdata\include\dcvras.h"\
	".\dcmdata\include\dcvrat.h"\
	".\dcmdata\include\dcvrcs.h"\
	".\dcmdata\include\dcvrda.h"\
	".\dcmdata\include\dcvrds.h"\
	".\dcmdata\include\dcvrdt.h"\
	".\dcmdata\include\dcvrfd.h"\
	".\dcmdata\include\dcvrfl.h"\
	".\dcmdata\include\dcvris.h"\
	".\dcmdata\include\dcvrlo.h"\
	".\dcmdata\include\dcvrlt.h"\
	".\dcmdata\include\dcvrobow.h"\
	".\dcmdata\include\dcvrpn.h"\
	".\dcmdata\include\dcvrsh.h"\
	".\dcmdata\include\dcvrsl.h"\
	".\dcmdata\include\dcvrss.h"\
	".\dcmdata\include\dcvrst.h"\
	".\dcmdata\include\dcvrtm.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcvrul.h"\
	".\dcmdata\include\dcvrulup.h"\
	".\dcmdata\include\dcvrus.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmdata\include\pix.h"\
	

"$(INTDIR)\dcitem.obj" : $(SOURCE) $(DEP_CPP_DCITE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dclist.cxx
DEP_CPP_DCLIS=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	

"$(INTDIR)\dclist.obj" : $(SOURCE) $(DEP_CPP_DCLIS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcmetinf.cxx
DEP_CPP_DCMET=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdefine.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcvrul.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcmetinf.obj" : $(SOURCE) $(DEP_CPP_DCMET) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcobject.cxx
DEP_CPP_DCOBJ=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dcswap.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcobject.obj" : $(SOURCE) $(DEP_CPP_DCOBJ) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcpixseq.cxx
DEP_CPP_DCPIX=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcpixseq.h"\
	".\dcmdata\include\dcpxitem.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrobow.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcpixseq.obj" : $(SOURCE) $(DEP_CPP_DCPIX) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcpxitem.cxx
DEP_CPP_DCPXI=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcpxitem.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrobow.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcpxitem.obj" : $(SOURCE) $(DEP_CPP_DCPXI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcsequen.cxx
DEP_CPP_DCSEQ=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcdirrec.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcpxitem.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrobow.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcsequen.obj" : $(SOURCE) $(DEP_CPP_DCSEQ) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcstack.cxx
DEP_CPP_DCSTA=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcstack.h"\
	

"$(INTDIR)\dcstack.obj" : $(SOURCE) $(DEP_CPP_DCSTA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcstream.cxx
DEP_CPP_DCSTR=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbuf.h"\
	".\dcmdata\include\dcdefine.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcstream.h"\
	

"$(INTDIR)\dcstream.obj" : $(SOURCE) $(DEP_CPP_DCSTR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcswap.cxx
DEP_CPP_DCSWA=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcswap.h"\
	

"$(INTDIR)\dcswap.obj" : $(SOURCE) $(DEP_CPP_DCSWA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dctag.cxx
DEP_CPP_DCTAG=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\pix.h"\
	

"$(INTDIR)\dctag.obj" : $(SOURCE) $(DEP_CPP_DCTAG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dctagkey.cxx
DEP_CPP_DCTAGK=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dctagkey.h"\
	

"$(INTDIR)\dctagkey.obj" : $(SOURCE) $(DEP_CPP_DCTAGK) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcuid.cxx
DEP_CPP_DCUID=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcuid.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\dcuid.obj" : $(SOURCE) $(DEP_CPP_DCUID) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcutils.cxx
DEP_CPP_DCUTI=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\cmdlnarg.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctk.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcchrstr.h"\
	".\dcmdata\include\dcdefine.h"\
	".\dcmdata\include\dcdicdir.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcdirrec.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcpixseq.h"\
	".\dcmdata\include\dcpxitem.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dcswap.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcutils.h"\
	".\dcmdata\include\dcvm.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrae.h"\
	".\dcmdata\include\dcvras.h"\
	".\dcmdata\include\dcvrat.h"\
	".\dcmdata\include\dcvrcs.h"\
	".\dcmdata\include\dcvrda.h"\
	".\dcmdata\include\dcvrds.h"\
	".\dcmdata\include\dcvrdt.h"\
	".\dcmdata\include\dcvrfd.h"\
	".\dcmdata\include\dcvrfl.h"\
	".\dcmdata\include\dcvris.h"\
	".\dcmdata\include\dcvrlo.h"\
	".\dcmdata\include\dcvrlt.h"\
	".\dcmdata\include\dcvrobow.h"\
	".\dcmdata\include\dcvrpn.h"\
	".\dcmdata\include\dcvrsh.h"\
	".\dcmdata\include\dcvrsl.h"\
	".\dcmdata\include\dcvrss.h"\
	".\dcmdata\include\dcvrst.h"\
	".\dcmdata\include\dcvrtm.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcvrul.h"\
	".\dcmdata\include\dcvrulup.h"\
	".\dcmdata\include\dcvrus.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmdata\include\pix.h"\
	

"$(INTDIR)\dcutils.obj" : $(SOURCE) $(DEP_CPP_DCUTI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcvm.cxx
DEP_CPP_DCVM_=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata\include\dcvm.h"\
	

"$(INTDIR)\dcvm.obj" : $(SOURCE) $(DEP_CPP_DCVM_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcvr.cxx
DEP_CPP_DCVR_=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcvr.h"\
	

"$(INTDIR)\dcvr.obj" : $(SOURCE) $(DEP_CPP_DCVR_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcvrae.cxx
DEP_CPP_DCVRA=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrae.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcvrae.obj" : $(SOURCE) $(DEP_CPP_DCVRA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcvras.cxx
DEP_CPP_DCVRAS=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvras.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcvras.obj" : $(SOURCE) $(DEP_CPP_DCVRAS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcvrat.cxx
DEP_CPP_DCVRAT=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvm.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrat.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcvrat.obj" : $(SOURCE) $(DEP_CPP_DCVRAT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcvrcs.cxx
DEP_CPP_DCVRC=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrcs.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcvrcs.obj" : $(SOURCE) $(DEP_CPP_DCVRC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcvrda.cxx
DEP_CPP_DCVRD=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrda.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcvrda.obj" : $(SOURCE) $(DEP_CPP_DCVRD) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcvrds.cxx
DEP_CPP_DCVRDS=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrds.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcvrds.obj" : $(SOURCE) $(DEP_CPP_DCVRDS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcvrdt.cxx
DEP_CPP_DCVRDT=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrdt.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcvrdt.obj" : $(SOURCE) $(DEP_CPP_DCVRDT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcvrfd.cxx
DEP_CPP_DCVRF=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvm.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrfd.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcvrfd.obj" : $(SOURCE) $(DEP_CPP_DCVRF) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcvrfl.cxx
DEP_CPP_DCVRFL=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvm.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrfl.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcvrfl.obj" : $(SOURCE) $(DEP_CPP_DCVRFL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcvris.cxx
DEP_CPP_DCVRI=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvris.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcvris.obj" : $(SOURCE) $(DEP_CPP_DCVRI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcvrlo.cxx
DEP_CPP_DCVRL=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcchrstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrlo.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcvrlo.obj" : $(SOURCE) $(DEP_CPP_DCVRL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcvrlt.cxx
DEP_CPP_DCVRLT=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcchrstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrlt.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcvrlt.obj" : $(SOURCE) $(DEP_CPP_DCVRLT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcvrobow.cxx
DEP_CPP_DCVRO=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dcswap.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvm.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrobow.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcvrobow.obj" : $(SOURCE) $(DEP_CPP_DCVRO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcvrpn.cxx
DEP_CPP_DCVRP=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcchrstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrpn.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcvrpn.obj" : $(SOURCE) $(DEP_CPP_DCVRP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcvrsh.cxx
DEP_CPP_DCVRS=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcchrstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrsh.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcvrsh.obj" : $(SOURCE) $(DEP_CPP_DCVRS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcvrsl.cxx
DEP_CPP_DCVRSL=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvm.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrsl.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcvrsl.obj" : $(SOURCE) $(DEP_CPP_DCVRSL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcvrss.cxx
DEP_CPP_DCVRSS=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvm.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrss.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcvrss.obj" : $(SOURCE) $(DEP_CPP_DCVRSS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcvrst.cxx
DEP_CPP_DCVRST=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcchrstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrst.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcvrst.obj" : $(SOURCE) $(DEP_CPP_DCVRST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcvrtm.cxx
DEP_CPP_DCVRT=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrtm.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcvrtm.obj" : $(SOURCE) $(DEP_CPP_DCVRT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcvrui.cxx
DEP_CPP_DCVRU=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcvrui.obj" : $(SOURCE) $(DEP_CPP_DCVRU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcvrul.cxx
DEP_CPP_DCVRUL=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvm.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrul.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcvrul.obj" : $(SOURCE) $(DEP_CPP_DCVRUL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcvrulup.cxx
DEP_CPP_DCVRULU=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrul.h"\
	".\dcmdata\include\dcvrulup.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcvrulup.obj" : $(SOURCE) $(DEP_CPP_DCVRULU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcvrus.cxx
DEP_CPP_DCVRUS=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvm.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrus.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcvrus.obj" : $(SOURCE) $(DEP_CPP_DCVRUS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\dcxfer.cxx
DEP_CPP_DCXFE=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcxfer.obj" : $(SOURCE) $(DEP_CPP_DCXFE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "mkdeftag - Win32 Release"
# Name "mkdeftag - Win32 Debug"

!IF  "$(CFG)" == "mkdeftag - Win32 Release"

!ELSEIF  "$(CFG)" == "mkdeftag - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmdata"

!IF  "$(CFG)" == "mkdeftag - Win32 Release"

"dcmdata - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Release" 

!ELSEIF  "$(CFG)" == "mkdeftag - Win32 Debug"

"dcmdata - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\mkdeftag.cxx

!IF  "$(CFG)" == "mkdeftag - Win32 Release"

DEP_CPP_MKDEF=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\cmdlnarg.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\pix.h"\
	

"$(INTDIR)\mkdeftag.obj" : $(SOURCE) $(DEP_CPP_MKDEF) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mkdeftag - Win32 Debug"

DEP_CPP_MKDEF=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\cmdlnarg.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\pix.h"\
	

"$(INTDIR)\mkdeftag.obj" : $(SOURCE) $(DEP_CPP_MKDEF) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "mkdictbi - Win32 Release"
# Name "mkdictbi - Win32 Debug"

!IF  "$(CFG)" == "mkdictbi - Win32 Release"

!ELSEIF  "$(CFG)" == "mkdictbi - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmdata"

!IF  "$(CFG)" == "mkdictbi - Win32 Release"

"dcmdata - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Release" 

!ELSEIF  "$(CFG)" == "mkdictbi - Win32 Debug"

"dcmdata - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\dcmdata\libsrc\mkdictbi.cxx
DEP_CPP_MKDIC=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\cmdlnarg.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\pix.h"\
	

"$(INTDIR)\mkdictbi.obj" : $(SOURCE) $(DEP_CPP_MKDIC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "dcmdump - Win32 Release"
# Name "dcmdump - Win32 Debug"

!IF  "$(CFG)" == "dcmdump - Win32 Release"

!ELSEIF  "$(CFG)" == "dcmdump - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmdata"

!IF  "$(CFG)" == "dcmdump - Win32 Release"

"dcmdata - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Release" 

!ELSEIF  "$(CFG)" == "dcmdump - Win32 Debug"

"dcmdata - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\dcmdata\apps\dcmdump.cxx
DEP_CPP_DCMDU=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\cmdlnarg.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctk.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcchrstr.h"\
	".\dcmdata\include\dcdefine.h"\
	".\dcmdata\include\dcdicdir.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcdirrec.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcpixseq.h"\
	".\dcmdata\include\dcpxitem.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dcswap.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvm.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrae.h"\
	".\dcmdata\include\dcvras.h"\
	".\dcmdata\include\dcvrat.h"\
	".\dcmdata\include\dcvrcs.h"\
	".\dcmdata\include\dcvrda.h"\
	".\dcmdata\include\dcvrds.h"\
	".\dcmdata\include\dcvrdt.h"\
	".\dcmdata\include\dcvrfd.h"\
	".\dcmdata\include\dcvrfl.h"\
	".\dcmdata\include\dcvris.h"\
	".\dcmdata\include\dcvrlo.h"\
	".\dcmdata\include\dcvrlt.h"\
	".\dcmdata\include\dcvrobow.h"\
	".\dcmdata\include\dcvrpn.h"\
	".\dcmdata\include\dcvrsh.h"\
	".\dcmdata\include\dcvrsl.h"\
	".\dcmdata\include\dcvrss.h"\
	".\dcmdata\include\dcvrst.h"\
	".\dcmdata\include\dcvrtm.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcvrul.h"\
	".\dcmdata\include\dcvrulup.h"\
	".\dcmdata\include\dcvrus.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmdata\include\pix.h"\
	

"$(INTDIR)\dcmdump.obj" : $(SOURCE) $(DEP_CPP_DCMDU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "dcmconv - Win32 Release"
# Name "dcmconv - Win32 Debug"

!IF  "$(CFG)" == "dcmconv - Win32 Release"

!ELSEIF  "$(CFG)" == "dcmconv - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmdata"

!IF  "$(CFG)" == "dcmconv - Win32 Release"

"dcmdata - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Release" 

!ELSEIF  "$(CFG)" == "dcmconv - Win32 Debug"

"dcmdata - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\dcmdata\apps\dcmconv.cxx

!IF  "$(CFG)" == "dcmconv - Win32 Release"

DEP_CPP_DCMCO=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\cmdlnarg.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctk.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcchrstr.h"\
	".\dcmdata\include\dcdefine.h"\
	".\dcmdata\include\dcdicdir.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcdirrec.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcpixseq.h"\
	".\dcmdata\include\dcpxitem.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dcswap.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvm.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrae.h"\
	".\dcmdata\include\dcvras.h"\
	".\dcmdata\include\dcvrat.h"\
	".\dcmdata\include\dcvrcs.h"\
	".\dcmdata\include\dcvrda.h"\
	".\dcmdata\include\dcvrds.h"\
	".\dcmdata\include\dcvrdt.h"\
	".\dcmdata\include\dcvrfd.h"\
	".\dcmdata\include\dcvrfl.h"\
	".\dcmdata\include\dcvris.h"\
	".\dcmdata\include\dcvrlo.h"\
	".\dcmdata\include\dcvrlt.h"\
	".\dcmdata\include\dcvrobow.h"\
	".\dcmdata\include\dcvrpn.h"\
	".\dcmdata\include\dcvrsh.h"\
	".\dcmdata\include\dcvrsl.h"\
	".\dcmdata\include\dcvrss.h"\
	".\dcmdata\include\dcvrst.h"\
	".\dcmdata\include\dcvrtm.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcvrul.h"\
	".\dcmdata\include\dcvrulup.h"\
	".\dcmdata\include\dcvrus.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmdata\include\pix.h"\
	

"$(INTDIR)\dcmconv.obj" : $(SOURCE) $(DEP_CPP_DCMCO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dcmconv - Win32 Debug"

DEP_CPP_DCMCO=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\cmdlnarg.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctk.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcchrstr.h"\
	".\dcmdata\include\dcdefine.h"\
	".\dcmdata\include\dcdicdir.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcdirrec.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcpixseq.h"\
	".\dcmdata\include\dcpxitem.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dcswap.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvm.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrae.h"\
	".\dcmdata\include\dcvras.h"\
	".\dcmdata\include\dcvrat.h"\
	".\dcmdata\include\dcvrcs.h"\
	".\dcmdata\include\dcvrda.h"\
	".\dcmdata\include\dcvrds.h"\
	".\dcmdata\include\dcvrdt.h"\
	".\dcmdata\include\dcvrfd.h"\
	".\dcmdata\include\dcvrfl.h"\
	".\dcmdata\include\dcvris.h"\
	".\dcmdata\include\dcvrlo.h"\
	".\dcmdata\include\dcvrlt.h"\
	".\dcmdata\include\dcvrobow.h"\
	".\dcmdata\include\dcvrpn.h"\
	".\dcmdata\include\dcvrsh.h"\
	".\dcmdata\include\dcvrsl.h"\
	".\dcmdata\include\dcvrss.h"\
	".\dcmdata\include\dcvrst.h"\
	".\dcmdata\include\dcvrtm.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcvrul.h"\
	".\dcmdata\include\dcvrulup.h"\
	".\dcmdata\include\dcvrus.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmdata\include\pix.h"\
	

"$(INTDIR)\dcmconv.obj" : $(SOURCE) $(DEP_CPP_DCMCO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "dump2dcm - Win32 Release"
# Name "dump2dcm - Win32 Debug"

!IF  "$(CFG)" == "dump2dcm - Win32 Release"

!ELSEIF  "$(CFG)" == "dump2dcm - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmdata"

!IF  "$(CFG)" == "dump2dcm - Win32 Release"

"dcmdata - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Release" 

!ELSEIF  "$(CFG)" == "dump2dcm - Win32 Debug"

"dcmdata - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\dcmdata\apps\dump2dcm.cxx
DEP_CPP_DUMP2=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\cmdlnarg.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctk.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcchrstr.h"\
	".\dcmdata\include\dcdefine.h"\
	".\dcmdata\include\dcdicdir.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcdirrec.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcpixseq.h"\
	".\dcmdata\include\dcpxitem.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dcswap.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvm.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrae.h"\
	".\dcmdata\include\dcvras.h"\
	".\dcmdata\include\dcvrat.h"\
	".\dcmdata\include\dcvrcs.h"\
	".\dcmdata\include\dcvrda.h"\
	".\dcmdata\include\dcvrds.h"\
	".\dcmdata\include\dcvrdt.h"\
	".\dcmdata\include\dcvrfd.h"\
	".\dcmdata\include\dcvrfl.h"\
	".\dcmdata\include\dcvris.h"\
	".\dcmdata\include\dcvrlo.h"\
	".\dcmdata\include\dcvrlt.h"\
	".\dcmdata\include\dcvrobow.h"\
	".\dcmdata\include\dcvrpn.h"\
	".\dcmdata\include\dcvrsh.h"\
	".\dcmdata\include\dcvrsl.h"\
	".\dcmdata\include\dcvrss.h"\
	".\dcmdata\include\dcvrst.h"\
	".\dcmdata\include\dcvrtm.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcvrul.h"\
	".\dcmdata\include\dcvrulup.h"\
	".\dcmdata\include\dcvrus.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmdata\include\pix.h"\
	

"$(INTDIR)\dump2dcm.obj" : $(SOURCE) $(DEP_CPP_DUMP2) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "dcmftest - Win32 Release"
# Name "dcmftest - Win32 Debug"

!IF  "$(CFG)" == "dcmftest - Win32 Release"

!ELSEIF  "$(CFG)" == "dcmftest - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmdata"

!IF  "$(CFG)" == "dcmftest - Win32 Release"

"dcmdata - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Release" 

!ELSEIF  "$(CFG)" == "dcmftest - Win32 Debug"

"dcmdata - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\dcmdata\apps\dcmftest.cxx
DEP_CPP_DCMFT=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	

"$(INTDIR)\dcmftest.obj" : $(SOURCE) $(DEP_CPP_DCMFT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "dcmgpdir - Win32 Release"
# Name "dcmgpdir - Win32 Debug"

!IF  "$(CFG)" == "dcmgpdir - Win32 Release"

!ELSEIF  "$(CFG)" == "dcmgpdir - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmdata"

!IF  "$(CFG)" == "dcmgpdir - Win32 Release"

"dcmdata - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Release" 

!ELSEIF  "$(CFG)" == "dcmgpdir - Win32 Debug"

"dcmdata - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\dcmdata\apps\dcmgpdir.cxx
DEP_CPP_DCMGP=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\cmdlnarg.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctk.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcchrstr.h"\
	".\dcmdata\include\dcdefine.h"\
	".\dcmdata\include\dcdicdir.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcdirrec.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcpixseq.h"\
	".\dcmdata\include\dcpxitem.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dcswap.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvm.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrae.h"\
	".\dcmdata\include\dcvras.h"\
	".\dcmdata\include\dcvrat.h"\
	".\dcmdata\include\dcvrcs.h"\
	".\dcmdata\include\dcvrda.h"\
	".\dcmdata\include\dcvrds.h"\
	".\dcmdata\include\dcvrdt.h"\
	".\dcmdata\include\dcvrfd.h"\
	".\dcmdata\include\dcvrfl.h"\
	".\dcmdata\include\dcvris.h"\
	".\dcmdata\include\dcvrlo.h"\
	".\dcmdata\include\dcvrlt.h"\
	".\dcmdata\include\dcvrobow.h"\
	".\dcmdata\include\dcvrpn.h"\
	".\dcmdata\include\dcvrsh.h"\
	".\dcmdata\include\dcvrsl.h"\
	".\dcmdata\include\dcvrss.h"\
	".\dcmdata\include\dcvrst.h"\
	".\dcmdata\include\dcvrtm.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcvrul.h"\
	".\dcmdata\include\dcvrulup.h"\
	".\dcmdata\include\dcvrus.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmdata\include\pix.h"\
	

!IF  "$(CFG)" == "dcmgpdir - Win32 Release"


"$(INTDIR)\dcmgpdir.obj" : $(SOURCE) $(DEP_CPP_DCMGP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dcmgpdir - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dcmgpdir.obj" : $(SOURCE) $(DEP_CPP_DCMGP) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dcmgpdir.sbr" : $(SOURCE) $(DEP_CPP_DCMGP) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "dcmnet - Win32 Release"
# Name "dcmnet - Win32 Debug"

!IF  "$(CFG)" == "dcmnet - Win32 Release"

!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\dcmnet\libsrc\dimcancl.cxx

!IF  "$(CFG)" == "dcmnet - Win32 Release"

DEP_CPP_DIMCA=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dimcond.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\dimcancl.obj" : $(SOURCE) $(DEP_CPP_DIMCA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

DEP_CPP_DIMCA=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dimcond.h"\
	

"$(INTDIR)\dimcancl.obj" : $(SOURCE) $(DEP_CPP_DIMCA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmnet\libsrc\assoc.cxx

!IF  "$(CFG)" == "dcmnet - Win32 Release"

DEP_CPP_ASSOC=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\asccond.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\assoc.obj" : $(SOURCE) $(DEP_CPP_ASSOC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

DEP_CPP_ASSOC=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\asccond.h"\
	

"$(INTDIR)\assoc.obj" : $(SOURCE) $(DEP_CPP_ASSOC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmnet\libsrc\cond.cxx

!IF  "$(CFG)" == "dcmnet - Win32 Release"

DEP_CPP_COND_=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\cond.obj" : $(SOURCE) $(DEP_CPP_COND_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

DEP_CPP_COND_=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	

"$(INTDIR)\cond.obj" : $(SOURCE) $(DEP_CPP_COND_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmnet\libsrc\dcompat.cxx

!IF  "$(CFG)" == "dcmnet - Win32 Release"

DEP_CPP_DCOMP=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\dcompat.obj" : $(SOURCE) $(DEP_CPP_DCOMP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

DEP_CPP_DCOMP=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\dcompat.obj" : $(SOURCE) $(DEP_CPP_DCOMP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmnet\libsrc\asccond.cxx

!IF  "$(CFG)" == "dcmnet - Win32 Release"

DEP_CPP_ASCCO=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\asccond.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\asccond.obj" : $(SOURCE) $(DEP_CPP_ASCCO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

DEP_CPP_ASCCO=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\asccond.h"\
	

"$(INTDIR)\asccond.obj" : $(SOURCE) $(DEP_CPP_ASCCO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmnet\libsrc\dimcmd.cxx

!IF  "$(CFG)" == "dcmnet - Win32 Release"

DEP_CPP_DIMCM=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dimcmd.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\dimcmd.obj" : $(SOURCE) $(DEP_CPP_DIMCM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

DEP_CPP_DIMCM=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dimcmd.h"\
	

"$(INTDIR)\dimcmd.obj" : $(SOURCE) $(DEP_CPP_DIMCM) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmnet\libsrc\dimcond.cxx

!IF  "$(CFG)" == "dcmnet - Win32 Release"

DEP_CPP_DIMCO=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dimcond.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\dimcond.obj" : $(SOURCE) $(DEP_CPP_DIMCO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

DEP_CPP_DIMCO=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dimcond.h"\
	

"$(INTDIR)\dimcond.obj" : $(SOURCE) $(DEP_CPP_DIMCO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmnet\libsrc\dimdump.cxx

!IF  "$(CFG)" == "dcmnet - Win32 Release"

DEP_CPP_DIMDU=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\dimdump.obj" : $(SOURCE) $(DEP_CPP_DIMDU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

DEP_CPP_DIMDU=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	

"$(INTDIR)\dimdump.obj" : $(SOURCE) $(DEP_CPP_DIMDU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmnet\libsrc\dimecho.cxx

!IF  "$(CFG)" == "dcmnet - Win32 Release"

DEP_CPP_DIMEC=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dimcond.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\dimecho.obj" : $(SOURCE) $(DEP_CPP_DIMEC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

DEP_CPP_DIMEC=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dimcond.h"\
	

"$(INTDIR)\dimecho.obj" : $(SOURCE) $(DEP_CPP_DIMEC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmnet\libsrc\dimfind.cxx

!IF  "$(CFG)" == "dcmnet - Win32 Release"

DEP_CPP_DIMFI=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dimcond.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\dimfind.obj" : $(SOURCE) $(DEP_CPP_DIMFI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

DEP_CPP_DIMFI=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dimcond.h"\
	

"$(INTDIR)\dimfind.obj" : $(SOURCE) $(DEP_CPP_DIMFI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmnet\libsrc\dimmove.cxx

!IF  "$(CFG)" == "dcmnet - Win32 Release"

DEP_CPP_DIMMO=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dimcond.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\dimmove.obj" : $(SOURCE) $(DEP_CPP_DIMMO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

DEP_CPP_DIMMO=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dimcond.h"\
	

"$(INTDIR)\dimmove.obj" : $(SOURCE) $(DEP_CPP_DIMMO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmnet\libsrc\dimse.cxx

!IF  "$(CFG)" == "dcmnet - Win32 Release"

DEP_CPP_DIMSE=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\cmdlnarg.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctk.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcchrstr.h"\
	".\dcmdata\include\dcdefine.h"\
	".\dcmdata\include\dcdicdir.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcdirrec.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcpixseq.h"\
	".\dcmdata\include\dcpxitem.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dcswap.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvm.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrae.h"\
	".\dcmdata\include\dcvras.h"\
	".\dcmdata\include\dcvrat.h"\
	".\dcmdata\include\dcvrcs.h"\
	".\dcmdata\include\dcvrda.h"\
	".\dcmdata\include\dcvrds.h"\
	".\dcmdata\include\dcvrdt.h"\
	".\dcmdata\include\dcvrfd.h"\
	".\dcmdata\include\dcvrfl.h"\
	".\dcmdata\include\dcvris.h"\
	".\dcmdata\include\dcvrlo.h"\
	".\dcmdata\include\dcvrlt.h"\
	".\dcmdata\include\dcvrobow.h"\
	".\dcmdata\include\dcvrpn.h"\
	".\dcmdata\include\dcvrsh.h"\
	".\dcmdata\include\dcvrsl.h"\
	".\dcmdata\include\dcvrss.h"\
	".\dcmdata\include\dcvrst.h"\
	".\dcmdata\include\dcvrtm.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcvrul.h"\
	".\dcmdata\include\dcvrulup.h"\
	".\dcmdata\include\dcvrus.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmdata\include\pix.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dimcmd.h"\
	".\dcmnet\libsrc\dimcond.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\dimse.obj" : $(SOURCE) $(DEP_CPP_DIMSE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

DEP_CPP_DIMSE=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\cmdlnarg.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctk.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcchrstr.h"\
	".\dcmdata\include\dcdefine.h"\
	".\dcmdata\include\dcdicdir.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcdirrec.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcpixseq.h"\
	".\dcmdata\include\dcpxitem.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dcswap.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvm.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrae.h"\
	".\dcmdata\include\dcvras.h"\
	".\dcmdata\include\dcvrat.h"\
	".\dcmdata\include\dcvrcs.h"\
	".\dcmdata\include\dcvrda.h"\
	".\dcmdata\include\dcvrds.h"\
	".\dcmdata\include\dcvrdt.h"\
	".\dcmdata\include\dcvrfd.h"\
	".\dcmdata\include\dcvrfl.h"\
	".\dcmdata\include\dcvris.h"\
	".\dcmdata\include\dcvrlo.h"\
	".\dcmdata\include\dcvrlt.h"\
	".\dcmdata\include\dcvrobow.h"\
	".\dcmdata\include\dcvrpn.h"\
	".\dcmdata\include\dcvrsh.h"\
	".\dcmdata\include\dcvrsl.h"\
	".\dcmdata\include\dcvrss.h"\
	".\dcmdata\include\dcvrst.h"\
	".\dcmdata\include\dcvrtm.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcvrul.h"\
	".\dcmdata\include\dcvrulup.h"\
	".\dcmdata\include\dcvrus.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmdata\include\pix.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dimcmd.h"\
	".\dcmnet\libsrc\dimcond.h"\
	

"$(INTDIR)\dimse.obj" : $(SOURCE) $(DEP_CPP_DIMSE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmnet\libsrc\dimstore.cxx

!IF  "$(CFG)" == "dcmnet - Win32 Release"

DEP_CPP_DIMST=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dimcond.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\dimstore.obj" : $(SOURCE) $(DEP_CPP_DIMST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

DEP_CPP_DIMST=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dimcond.h"\
	

"$(INTDIR)\dimstore.obj" : $(SOURCE) $(DEP_CPP_DIMST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmnet\libsrc\diutil.cxx

!IF  "$(CFG)" == "dcmnet - Win32 Release"

DEP_CPP_DIUTI=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\diutil.obj" : $(SOURCE) $(DEP_CPP_DIUTI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

DEP_CPP_DIUTI=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\diutil.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\diutil.obj" : $(SOURCE) $(DEP_CPP_DIUTI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmnet\libsrc\dul.cxx

!IF  "$(CFG)" == "dcmnet - Win32 Release"

DEP_CPP_DUL_C=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dulfsm.h"\
	".\dcmnet\libsrc\dulpriv.h"\
	".\dcmnet\libsrc\dulstruc.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_DUL_C=\
	".\dcmnet\libsrc\blg.h"\
	

"$(INTDIR)\dul.obj" : $(SOURCE) $(DEP_CPP_DUL_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

DEP_CPP_DUL_C=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dulfsm.h"\
	".\dcmnet\libsrc\dulpriv.h"\
	".\dcmnet\libsrc\dulstruc.h"\
	
NODEP_CPP_DUL_C=\
	".\dcmnet\libsrc\blg.h"\
	

"$(INTDIR)\dul.obj" : $(SOURCE) $(DEP_CPP_DUL_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmnet\libsrc\dulcond.cxx

!IF  "$(CFG)" == "dcmnet - Win32 Release"

DEP_CPP_DULCO=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dulstruc.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\dulcond.obj" : $(SOURCE) $(DEP_CPP_DULCO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

DEP_CPP_DULCO=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dulstruc.h"\
	

"$(INTDIR)\dulcond.obj" : $(SOURCE) $(DEP_CPP_DULCO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmnet\libsrc\dulconst.cxx

!IF  "$(CFG)" == "dcmnet - Win32 Release"

DEP_CPP_DULCON=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dulpriv.h"\
	".\dcmnet\libsrc\dulstruc.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\dulconst.obj" : $(SOURCE) $(DEP_CPP_DULCON) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

DEP_CPP_DULCON=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dulpriv.h"\
	".\dcmnet\libsrc\dulstruc.h"\
	

"$(INTDIR)\dulconst.obj" : $(SOURCE) $(DEP_CPP_DULCON) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmnet\libsrc\dulextra.cxx

!IF  "$(CFG)" == "dcmnet - Win32 Release"

DEP_CPP_DULEX=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dulpriv.h"\
	".\dcmnet\libsrc\dulstruc.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\dulextra.obj" : $(SOURCE) $(DEP_CPP_DULEX) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

DEP_CPP_DULEX=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dulpriv.h"\
	".\dcmnet\libsrc\dulstruc.h"\
	

"$(INTDIR)\dulextra.obj" : $(SOURCE) $(DEP_CPP_DULEX) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmnet\libsrc\dulfsm.cxx

!IF  "$(CFG)" == "dcmnet - Win32 Release"

DEP_CPP_DULFS=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dulfsm.h"\
	".\dcmnet\libsrc\dulpriv.h"\
	".\dcmnet\libsrc\dulstruc.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_DULFS=\
	".\dcmnet\libsrc\blg.h"\
	

"$(INTDIR)\dulfsm.obj" : $(SOURCE) $(DEP_CPP_DULFS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

DEP_CPP_DULFS=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dulfsm.h"\
	".\dcmnet\libsrc\dulpriv.h"\
	".\dcmnet\libsrc\dulstruc.h"\
	
NODEP_CPP_DULFS=\
	".\dcmnet\libsrc\blg.h"\
	

"$(INTDIR)\dulfsm.obj" : $(SOURCE) $(DEP_CPP_DULFS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmnet\libsrc\dulparse.cxx

!IF  "$(CFG)" == "dcmnet - Win32 Release"

DEP_CPP_DULPA=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dulpriv.h"\
	".\dcmnet\libsrc\dulstruc.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\dulparse.obj" : $(SOURCE) $(DEP_CPP_DULPA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

DEP_CPP_DULPA=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dulpriv.h"\
	".\dcmnet\libsrc\dulstruc.h"\
	

"$(INTDIR)\dulparse.obj" : $(SOURCE) $(DEP_CPP_DULPA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmnet\libsrc\dulpres.cxx

!IF  "$(CFG)" == "dcmnet - Win32 Release"

DEP_CPP_DULPR=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dulfsm.h"\
	".\dcmnet\libsrc\dulpriv.h"\
	".\dcmnet\libsrc\dulstruc.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\dulpres.obj" : $(SOURCE) $(DEP_CPP_DULPR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

DEP_CPP_DULPR=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\dulfsm.h"\
	".\dcmnet\libsrc\dulpriv.h"\
	".\dcmnet\libsrc\dulstruc.h"\
	

"$(INTDIR)\dulpres.obj" : $(SOURCE) $(DEP_CPP_DULPR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmnet\libsrc\lst.cxx

!IF  "$(CFG)" == "dcmnet - Win32 Release"

DEP_CPP_LST_C=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\lstpriv.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\lst.obj" : $(SOURCE) $(DEP_CPP_LST_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

DEP_CPP_LST_C=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\lstpriv.h"\
	

"$(INTDIR)\lst.obj" : $(SOURCE) $(DEP_CPP_LST_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dcmnet\libsrc\lstcond.cxx

!IF  "$(CFG)" == "dcmnet - Win32 Release"

DEP_CPP_LSTCO=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\lstpriv.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\lstcond.obj" : $(SOURCE) $(DEP_CPP_LSTCO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

DEP_CPP_LSTCO=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\lst.h"\
	".\dcmnet\libsrc\lstpriv.h"\
	

"$(INTDIR)\lstcond.obj" : $(SOURCE) $(DEP_CPP_LSTCO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "echoscu - Win32 Release"
# Name "echoscu - Win32 Debug"

!IF  "$(CFG)" == "echoscu - Win32 Release"

!ELSEIF  "$(CFG)" == "echoscu - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmdata"

!IF  "$(CFG)" == "echoscu - Win32 Release"

"dcmdata - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Release" 

!ELSEIF  "$(CFG)" == "echoscu - Win32 Debug"

"dcmdata - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmnet"

!IF  "$(CFG)" == "echoscu - Win32 Release"

"dcmnet - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmnet - Win32 Release" 

!ELSEIF  "$(CFG)" == "echoscu - Win32 Debug"

"dcmnet - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmnet - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\dcmnet\apps\echoscu.cxx

!IF  "$(CFG)" == "echoscu - Win32 Release"

DEP_CPP_ECHOS=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\cmdlnarg.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmdata\include\pix.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\echoscu.obj" : $(SOURCE) $(DEP_CPP_ECHOS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "echoscu - Win32 Debug"

DEP_CPP_ECHOS=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\cmdlnarg.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmdata\include\pix.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\echoscu.obj" : $(SOURCE) $(DEP_CPP_ECHOS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "storescp - Win32 Release"
# Name "storescp - Win32 Debug"

!IF  "$(CFG)" == "storescp - Win32 Release"

!ELSEIF  "$(CFG)" == "storescp - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmdata"

!IF  "$(CFG)" == "storescp - Win32 Release"

"dcmdata - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Release" 

!ELSEIF  "$(CFG)" == "storescp - Win32 Debug"

"dcmdata - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmnet"

!IF  "$(CFG)" == "storescp - Win32 Release"

"dcmnet - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmnet - Win32 Release" 

!ELSEIF  "$(CFG)" == "storescp - Win32 Debug"

"dcmnet - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmnet - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\dcmnet\apps\storescp.cxx

!IF  "$(CFG)" == "storescp - Win32 Release"

DEP_CPP_STORE=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\cmdlnarg.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmdata\include\pix.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\storescp.obj" : $(SOURCE) $(DEP_CPP_STORE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "storescp - Win32 Debug"

DEP_CPP_STORE=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\cmdlnarg.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmdata\include\pix.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\storescp.obj" : $(SOURCE) $(DEP_CPP_STORE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "storescu - Win32 Release"
# Name "storescu - Win32 Debug"

!IF  "$(CFG)" == "storescu - Win32 Release"

!ELSEIF  "$(CFG)" == "storescu - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmdata"

!IF  "$(CFG)" == "storescu - Win32 Release"

"dcmdata - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Release" 

!ELSEIF  "$(CFG)" == "storescu - Win32 Debug"

"dcmdata - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmnet"

!IF  "$(CFG)" == "storescu - Win32 Release"

"dcmnet - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmnet - Win32 Release" 

!ELSEIF  "$(CFG)" == "storescu - Win32 Debug"

"dcmnet - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmnet - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\dcmnet\apps\storescu.cxx
DEP_CPP_STORES=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\cmdlnarg.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmdata\include\pix.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\storescu.obj" : $(SOURCE) $(DEP_CPP_STORES) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "findscu - Win32 Release"
# Name "findscu - Win32 Debug"

!IF  "$(CFG)" == "findscu - Win32 Release"

!ELSEIF  "$(CFG)" == "findscu - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmdata"

!IF  "$(CFG)" == "findscu - Win32 Release"

"dcmdata - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Release" 

!ELSEIF  "$(CFG)" == "findscu - Win32 Debug"

"dcmdata - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmnet"

!IF  "$(CFG)" == "findscu - Win32 Release"

"dcmnet - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmnet - Win32 Release" 

!ELSEIF  "$(CFG)" == "findscu - Win32 Debug"

"dcmnet - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmnet - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\dcmnet\apps\findscu.cxx
DEP_CPP_FINDS=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\cmdlnarg.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmdata\include\pix.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\findscu.obj" : $(SOURCE) $(DEP_CPP_FINDS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "movescu - Win32 Release"
# Name "movescu - Win32 Debug"

!IF  "$(CFG)" == "movescu - Win32 Release"

!ELSEIF  "$(CFG)" == "movescu - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmdata"

!IF  "$(CFG)" == "movescu - Win32 Release"

"dcmdata - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Release" 

!ELSEIF  "$(CFG)" == "movescu - Win32 Debug"

"dcmdata - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmnet"

!IF  "$(CFG)" == "movescu - Win32 Release"

"dcmnet - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmnet - Win32 Release" 

!ELSEIF  "$(CFG)" == "movescu - Win32 Debug"

"dcmnet - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmnet - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\dcmnet\apps\movescu.cxx

!IF  "$(CFG)" == "movescu - Win32 Release"

DEP_CPP_MOVES=\
	".\config/include\config.h"\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\cmdlnarg.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmdata\include\pix.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\movescu.obj" : $(SOURCE) $(DEP_CPP_MOVES) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "movescu - Win32 Debug"

DEP_CPP_MOVES=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\cmdlnarg.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmdata\include\pix.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\movescu.obj" : $(SOURCE) $(DEP_CPP_MOVES) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\movescu.sbr" : $(SOURCE) $(DEP_CPP_MOVES) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "imagectnlib - Win32 Release"
# Name "imagectnlib - Win32 Debug"

!IF  "$(CFG)" == "imagectnlib - Win32 Release"

!ELSEIF  "$(CFG)" == "imagectnlib - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\imagectn\libsrc\dbcond.cxx
DEP_CPP_DBCON=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\imagectn/include\imagedb.h"\
	".\imagectn\libsrc\dbcond.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

!IF  "$(CFG)" == "imagectnlib - Win32 Release"


"$(INTDIR)\dbcond.obj" : $(SOURCE) $(DEP_CPP_DBCON) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "imagectnlib - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dbcond.obj" : $(SOURCE) $(DEP_CPP_DBCON) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dbcond.sbr" : $(SOURCE) $(DEP_CPP_DBCON) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\imagectn\libsrc\dbfind.cxx
DEP_CPP_DBFIN=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\imagectn/include\imagedb.h"\
	".\imagectn\libsrc\dbpriv.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

!IF  "$(CFG)" == "imagectnlib - Win32 Release"


"$(INTDIR)\dbfind.obj" : $(SOURCE) $(DEP_CPP_DBFIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "imagectnlib - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dbfind.obj" : $(SOURCE) $(DEP_CPP_DBFIN) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dbfind.sbr" : $(SOURCE) $(DEP_CPP_DBFIN) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\imagectn\libsrc\dbindex.cxx
DEP_CPP_DBIND=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\imagectn/include\imagedb.h"\
	".\imagectn\libsrc\dbpriv.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

!IF  "$(CFG)" == "imagectnlib - Win32 Release"


"$(INTDIR)\dbindex.obj" : $(SOURCE) $(DEP_CPP_DBIND) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "imagectnlib - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dbindex.obj" : $(SOURCE) $(DEP_CPP_DBIND) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dbindex.sbr" : $(SOURCE) $(DEP_CPP_DBIND) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\imagectn\libsrc\dbmove.cxx
DEP_CPP_DBMOV=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\imagectn/include\imagedb.h"\
	".\imagectn\libsrc\dbpriv.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

!IF  "$(CFG)" == "imagectnlib - Win32 Release"


"$(INTDIR)\dbmove.obj" : $(SOURCE) $(DEP_CPP_DBMOV) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "imagectnlib - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dbmove.obj" : $(SOURCE) $(DEP_CPP_DBMOV) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dbmove.sbr" : $(SOURCE) $(DEP_CPP_DBMOV) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\imagectn\libsrc\dbstore.cxx
DEP_CPP_DBSTO=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\imagectn/include\imagedb.h"\
	".\imagectn\libsrc\dbpriv.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

!IF  "$(CFG)" == "imagectnlib - Win32 Release"


"$(INTDIR)\dbstore.obj" : $(SOURCE) $(DEP_CPP_DBSTO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "imagectnlib - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dbstore.obj" : $(SOURCE) $(DEP_CPP_DBSTO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dbstore.sbr" : $(SOURCE) $(DEP_CPP_DBSTO) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\imagectn\libsrc\dbutils.cxx
DEP_CPP_DBUTI=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\imagectn/include\imagedb.h"\
	".\imagectn\libsrc\dbpriv.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

!IF  "$(CFG)" == "imagectnlib - Win32 Release"


"$(INTDIR)\dbutils.obj" : $(SOURCE) $(DEP_CPP_DBUTI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "imagectnlib - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\dbutils.obj" : $(SOURCE) $(DEP_CPP_DBUTI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\dbutils.sbr" : $(SOURCE) $(DEP_CPP_DBUTI) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "assctest - Win32 Release"
# Name "assctest - Win32 Debug"

!IF  "$(CFG)" == "assctest - Win32 Release"

!ELSEIF  "$(CFG)" == "assctest - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmdata"

!IF  "$(CFG)" == "assctest - Win32 Release"

"dcmdata - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Release" 

!ELSEIF  "$(CFG)" == "assctest - Win32 Debug"

"dcmdata - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmnet"

!IF  "$(CFG)" == "assctest - Win32 Release"

"dcmnet - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmnet - Win32 Release" 

!ELSEIF  "$(CFG)" == "assctest - Win32 Debug"

"dcmnet - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmnet - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\dcmnet\tests\assctest.cxx
DEP_CPP_ASSCT=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\cmdlnarg.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmdata\include\pix.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\assctest.obj" : $(SOURCE) $(DEP_CPP_ASSCT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "imagectn - Win32 Release"
# Name "imagectn - Win32 Debug"

!IF  "$(CFG)" == "imagectn - Win32 Release"

!ELSEIF  "$(CFG)" == "imagectn - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmdata"

!IF  "$(CFG)" == "imagectn - Win32 Release"

"dcmdata - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Release" 

!ELSEIF  "$(CFG)" == "imagectn - Win32 Debug"

"dcmdata - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmnet"

!IF  "$(CFG)" == "imagectn - Win32 Release"

"dcmnet - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmnet - Win32 Release" 

!ELSEIF  "$(CFG)" == "imagectn - Win32 Debug"

"dcmnet - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmnet - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "imagectnlib"

!IF  "$(CFG)" == "imagectn - Win32 Release"

"imagectnlib - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="imagectnlib - Win32 Release" 

!ELSEIF  "$(CFG)" == "imagectn - Win32 Debug"

"imagectnlib - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="imagectnlib - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\imagectn\apps\scestore.cxx

!IF  "$(CFG)" == "imagectn - Win32 Release"

DEP_CPP_SCEST=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\imagectn/include\imagedb.h"\
	".\imagectn\apps\cnf.h"\
	".\imagectn\apps\imagectn.h"\
	".\imagectn\apps\sce.h"\
	".\imagectn\apps\scestore.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_SCEST=\
	".\imagectn\apps\dispuser.h"\
	

"$(INTDIR)\scestore.obj" : $(SOURCE) $(DEP_CPP_SCEST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "imagectn - Win32 Debug"

DEP_CPP_SCEST=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcfilefo.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcmetinf.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcsequen.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\imagectn/include\imagedb.h"\
	".\imagectn\apps\cnf.h"\
	".\imagectn\apps\imagectn.h"\
	".\imagectn\apps\sce.h"\
	".\imagectn\apps\scestore.h"\
	
NODEP_CPP_SCEST=\
	".\imagectn\apps\dispuser.h"\
	

"$(INTDIR)\scestore.obj" : $(SOURCE) $(DEP_CPP_SCEST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\imagectn\apps\cnfexnt.cxx
DEP_CPP_CNFEX=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\imagectn\apps\cnf.h"\
	".\imagectn\apps\cnfpriv.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_CNFEX=\
	".\imagectn\apps\osconfig.h"\
	

"$(INTDIR)\cnfexnt.obj" : $(SOURCE) $(DEP_CPP_CNFEX) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\imagectn\apps\cnfpriv.cxx
DEP_CPP_CNFPR=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\imagectn\apps\cnfpriv.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_CNFPR=\
	".\imagectn\apps\osconfig.h"\
	

"$(INTDIR)\cnfpriv.obj" : $(SOURCE) $(DEP_CPP_CNFPR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\imagectn\apps\imagectn.cxx
DEP_CPP_IMAGE=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\cmdlnarg.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdebug.h"\
	".\dcmdata/include\dcdict.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcentbst.h"\
	".\dcmdata\include\dcentdef.h"\
	".\dcmdata\include\dcentlst.h"\
	".\dcmdata\include\dcentset.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmdata\include\pix.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\imagectn/include\imagedb.h"\
	".\imagectn\apps\cnf.h"\
	".\imagectn\apps\imagectn.h"\
	".\imagectn\apps\sce.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_IMAGE=\
	".\imagectn\apps\dispuser.h"\
	

"$(INTDIR)\imagectn.obj" : $(SOURCE) $(DEP_CPP_IMAGE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\imagectn\apps\sce.cxx

!IF  "$(CFG)" == "imagectn - Win32 Release"

DEP_CPP_SCE_C=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\imagectn/include\imagedb.h"\
	".\imagectn\apps\cnf.h"\
	".\imagectn\apps\imagectn.h"\
	".\imagectn\apps\sce.h"\
	".\imagectn\apps\sceecho.h"\
	".\imagectn\apps\scefind.h"\
	".\imagectn\apps\scemove.h"\
	".\imagectn\apps\scestore.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_SCE_C=\
	".\imagectn\apps\dispuser.h"\
	

"$(INTDIR)\sce.obj" : $(SOURCE) $(DEP_CPP_SCE_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "imagectn - Win32 Debug"

DEP_CPP_SCE_C=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\imagectn/include\imagedb.h"\
	".\imagectn\apps\cnf.h"\
	".\imagectn\apps\imagectn.h"\
	".\imagectn\apps\sce.h"\
	".\imagectn\apps\sceecho.h"\
	".\imagectn\apps\scefind.h"\
	".\imagectn\apps\scemove.h"\
	".\imagectn\apps\scestore.h"\
	
NODEP_CPP_SCE_C=\
	".\imagectn\apps\dispuser.h"\
	

"$(INTDIR)\sce.obj" : $(SOURCE) $(DEP_CPP_SCE_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\imagectn\apps\sceecho.cxx

!IF  "$(CFG)" == "imagectn - Win32 Release"

DEP_CPP_SCEEC=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\imagectn\apps\cnf.h"\
	".\imagectn\apps\imagectn.h"\
	".\imagectn\apps\sceecho.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	

"$(INTDIR)\sceecho.obj" : $(SOURCE) $(DEP_CPP_SCEEC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "imagectn - Win32 Debug"

DEP_CPP_SCEEC=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\imagectn\apps\cnf.h"\
	".\imagectn\apps\imagectn.h"\
	".\imagectn\apps\sceecho.h"\
	

"$(INTDIR)\sceecho.obj" : $(SOURCE) $(DEP_CPP_SCEEC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\imagectn\apps\scefind.cxx

!IF  "$(CFG)" == "imagectn - Win32 Release"

DEP_CPP_SCEFI=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\imagectn/include\imagedb.h"\
	".\imagectn\apps\cnf.h"\
	".\imagectn\apps\imagectn.h"\
	".\imagectn\apps\sce.h"\
	".\imagectn\apps\scefind.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_SCEFI=\
	".\imagectn\apps\dispuser.h"\
	

"$(INTDIR)\scefind.obj" : $(SOURCE) $(DEP_CPP_SCEFI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "imagectn - Win32 Debug"

DEP_CPP_SCEFI=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\imagectn/include\imagedb.h"\
	".\imagectn\apps\cnf.h"\
	".\imagectn\apps\imagectn.h"\
	".\imagectn\apps\sce.h"\
	".\imagectn\apps\scefind.h"\
	
NODEP_CPP_SCEFI=\
	".\imagectn\apps\dispuser.h"\
	

"$(INTDIR)\scefind.obj" : $(SOURCE) $(DEP_CPP_SCEFI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\imagectn\apps\scemove.cxx

!IF  "$(CFG)" == "imagectn - Win32 Release"

DEP_CPP_SCEMO=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\imagectn/include\imagedb.h"\
	".\imagectn\apps\cnf.h"\
	".\imagectn\apps\imagectn.h"\
	".\imagectn\apps\sce.h"\
	".\imagectn\apps\scemove.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_SCEMO=\
	".\imagectn\apps\dispuser.h"\
	

"$(INTDIR)\scemove.obj" : $(SOURCE) $(DEP_CPP_SCEMO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "imagectn - Win32 Debug"

DEP_CPP_SCEMO=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\imagectn/include\imagedb.h"\
	".\imagectn\apps\cnf.h"\
	".\imagectn\apps\imagectn.h"\
	".\imagectn\apps\sce.h"\
	".\imagectn\apps\scemove.h"\
	
NODEP_CPP_SCEMO=\
	".\imagectn\apps\dispuser.h"\
	

"$(INTDIR)\scemove.obj" : $(SOURCE) $(DEP_CPP_SCEMO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\imagectn\apps\cnf.cxx
DEP_CPP_CNF_C=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\imagectn\apps\cnf.h"\
	".\imagectn\apps\cnfpriv.h"\
	
NODEP_CPP_CNF_C=\
	".\imagectn\apps\osconfig.h"\
	

"$(INTDIR)\cnf.obj" : $(SOURCE) $(DEP_CPP_CNF_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "ti - Win32 Release"
# Name "ti - Win32 Debug"

!IF  "$(CFG)" == "ti - Win32 Release"

!ELSEIF  "$(CFG)" == "ti - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmdata"

!IF  "$(CFG)" == "ti - Win32 Release"

"dcmdata - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Release" 

!ELSEIF  "$(CFG)" == "ti - Win32 Debug"

"dcmdata - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmnet"

!IF  "$(CFG)" == "ti - Win32 Release"

"dcmnet - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmnet - Win32 Release" 

!ELSEIF  "$(CFG)" == "ti - Win32 Debug"

"dcmnet - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmnet - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "imagectnlib"

!IF  "$(CFG)" == "ti - Win32 Release"

"imagectnlib - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="imagectnlib - Win32 Release" 

!ELSEIF  "$(CFG)" == "ti - Win32 Debug"

"imagectnlib - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="imagectnlib - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\imagectn\apps\tiui.cxx

!IF  "$(CFG)" == "ti - Win32 Release"

DEP_CPP_TIUI_=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dcdeftag.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\imagectn/include\imagedb.h"\
	".\imagectn\apps\cnf.h"\
	".\imagectn\apps\ti.h"\
	".\imagectn\apps\tinet.h"\
	".\imagectn\apps\tiquery.h"\
	".\imagectn\apps\tiui.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_TIUI_=\
	".\imagectn\apps\dispuser.h"\
	

"$(INTDIR)\tiui.obj" : $(SOURCE) $(DEP_CPP_TIUI_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ti - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\tiui.obj" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\tiui.sbr" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\imagectn\apps\cnfexnt.cxx

!IF  "$(CFG)" == "ti - Win32 Release"

DEP_CPP_CNFEX=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\imagectn\apps\cnf.h"\
	".\imagectn\apps\cnfpriv.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_CNFEX=\
	".\imagectn\apps\osconfig.h"\
	

"$(INTDIR)\cnfexnt.obj" : $(SOURCE) $(DEP_CPP_CNFEX) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ti - Win32 Debug"

DEP_CPP_CNFEX=\
	".\imagectn\apps\cnf.h"\
	".\imagectn\apps\cnfpriv.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_CNFEX=\
	".\imagectn\apps\osconfig.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\cnfexnt.obj" : $(SOURCE) $(DEP_CPP_CNFEX) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\cnfexnt.sbr" : $(SOURCE) $(DEP_CPP_CNFEX) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\imagectn\apps\cnfpriv.cxx
DEP_CPP_CNFPR=\
	".\imagectn\apps\cnfpriv.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_CNFPR=\
	".\imagectn\apps\osconfig.h"\
	

!IF  "$(CFG)" == "ti - Win32 Release"


"$(INTDIR)\cnfpriv.obj" : $(SOURCE) $(DEP_CPP_CNFPR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ti - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\cnfpriv.obj" : $(SOURCE) $(DEP_CPP_CNFPR) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\cnfpriv.sbr" : $(SOURCE) $(DEP_CPP_CNFPR) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\imagectn\apps\ti.cxx

!IF  "$(CFG)" == "ti - Win32 Release"

DEP_CPP_TI_CX=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\imagectn/include\imagedb.h"\
	".\imagectn\apps\cnf.h"\
	".\imagectn\apps\ti.h"\
	".\imagectn\apps\tinet.h"\
	".\imagectn\apps\tiquery.h"\
	".\imagectn\apps\tiui.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_TI_CX=\
	".\imagectn\apps\assoc.h"\
	".\imagectn\apps\dcdebug.h"\
	".\imagectn\apps\dcdict.h"\
	".\imagectn\apps\dcuid.h"\
	".\imagectn\apps\dimse.h"\
	".\imagectn\apps\dispuser.h"\
	".\imagectn\apps\osconfig.h"\
	

"$(INTDIR)\ti.obj" : $(SOURCE) $(DEP_CPP_TI_CX) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ti - Win32 Debug"

DEP_CPP_TI_CX=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\imagectn\apps\cnf.h"\
	".\imagectn\apps\ti.h"\
	".\imagectn\apps\tinet.h"\
	".\imagectn\apps\tiquery.h"\
	".\imagectn\apps\tiui.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_TI_CX=\
	".\imagectn\apps\assoc.h"\
	".\imagectn\apps\dcdebug.h"\
	".\imagectn\apps\dcdict.h"\
	".\imagectn\apps\dcuid.h"\
	".\imagectn\apps\dicom.h"\
	".\imagectn\apps\dimse.h"\
	".\imagectn\apps\dispuser.h"\
	".\imagectn\apps\diutil.h"\
	".\imagectn\apps\imagedb.h"\
	".\imagectn\apps\osconfig.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ti.obj" : $(SOURCE) $(DEP_CPP_TI_CX) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ti.sbr" : $(SOURCE) $(DEP_CPP_TI_CX) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\imagectn\apps\tinet.cxx

!IF  "$(CFG)" == "ti - Win32 Release"

DEP_CPP_TINET=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\imagectn/include\imagedb.h"\
	".\imagectn\apps\cnf.h"\
	".\imagectn\apps\ti.h"\
	".\imagectn\apps\tinet.h"\
	".\imagectn\apps\tiquery.h"\
	".\imagectn\apps\tiui.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_TINET=\
	".\imagectn\apps\assoc.h"\
	".\imagectn\apps\dcfilefo.h"\
	".\imagectn\apps\dcuid.h"\
	".\imagectn\apps\dimse.h"\
	".\imagectn\apps\dispuser.h"\
	".\imagectn\apps\diutil.h"\
	".\imagectn\apps\osconfig.h"\
	

"$(INTDIR)\tinet.obj" : $(SOURCE) $(DEP_CPP_TINET) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ti - Win32 Debug"

DEP_CPP_TINET=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\imagectn\apps\cnf.h"\
	".\imagectn\apps\ti.h"\
	".\imagectn\apps\tinet.h"\
	".\imagectn\apps\tiquery.h"\
	".\imagectn\apps\tiui.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_TINET=\
	".\imagectn\apps\assoc.h"\
	".\imagectn\apps\dcfilefo.h"\
	".\imagectn\apps\dcuid.h"\
	".\imagectn\apps\dicom.h"\
	".\imagectn\apps\dimse.h"\
	".\imagectn\apps\dispuser.h"\
	".\imagectn\apps\diutil.h"\
	".\imagectn\apps\imagedb.h"\
	".\imagectn\apps\osconfig.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\tinet.obj" : $(SOURCE) $(DEP_CPP_TINET) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\tinet.sbr" : $(SOURCE) $(DEP_CPP_TINET) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\imagectn\apps\tiquery.cxx

!IF  "$(CFG)" == "ti - Win32 Release"

DEP_CPP_TIQUE=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dcdatset.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmdata/include\dcuid.h"\
	".\dcmdata\include\dcbytstr.h"\
	".\dcmdata\include\dcdicent.h"\
	".\dcmdata\include\dcelem.h"\
	".\dcmdata\include\dcerror.h"\
	".\dcmdata\include\dcitem.h"\
	".\dcmdata\include\dclist.h"\
	".\dcmdata\include\dcobject.h"\
	".\dcmdata\include\dcstack.h"\
	".\dcmdata\include\dcstream.h"\
	".\dcmdata\include\dctag.h"\
	".\dcmdata\include\dctagkey.h"\
	".\dcmdata\include\dcvr.h"\
	".\dcmdata\include\dcvrui.h"\
	".\dcmdata\include\dcxfer.h"\
	".\dcmnet/include\assoc.h"\
	".\dcmnet/include\cond.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\dcmnet/include\dimse.h"\
	".\dcmnet/include\diutil.h"\
	".\dcmnet/include\dul.h"\
	".\dcmnet/include\lst.h"\
	".\imagectn/include\imagedb.h"\
	".\imagectn\apps\cnf.h"\
	".\imagectn\apps\ti.h"\
	".\imagectn\apps\tinet.h"\
	".\imagectn\apps\tiquery.h"\
	".\imagectn\apps\tiui.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_TIQUE=\
	".\imagectn\apps\dcdeftag.h"\
	".\imagectn\apps\dcfilefo.h"\
	".\imagectn\apps\dcuid.h"\
	".\imagectn\apps\dicom.h"\
	".\imagectn\apps\dispuser.h"\
	".\imagectn\apps\diutil.h"\
	".\imagectn\apps\imagedb.h"\
	".\imagectn\apps\osconfig.h"\
	

"$(INTDIR)\tiquery.obj" : $(SOURCE) $(DEP_CPP_TIQUE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ti - Win32 Debug"

DEP_CPP_TIQUE=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\dcmdata/include\dctypes.h"\
	".\dcmnet/include\dcompat.h"\
	".\dcmnet/include\dicom.h"\
	".\imagectn\apps\cnf.h"\
	".\imagectn\apps\ti.h"\
	".\imagectn\apps\tinet.h"\
	".\imagectn\apps\tiquery.h"\
	".\imagectn\apps\tiui.h"\
	{$(INCLUDE)}"\sys\STAT.H"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	
NODEP_CPP_TIQUE=\
	".\imagectn\apps\assoc.h"\
	".\imagectn\apps\dcdeftag.h"\
	".\imagectn\apps\dcfilefo.h"\
	".\imagectn\apps\dcuid.h"\
	".\imagectn\apps\dicom.h"\
	".\imagectn\apps\dimse.h"\
	".\imagectn\apps\dispuser.h"\
	".\imagectn\apps\diutil.h"\
	".\imagectn\apps\imagedb.h"\
	".\imagectn\apps\osconfig.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\tiquery.obj" : $(SOURCE) $(DEP_CPP_TIQUE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\tiquery.sbr" : $(SOURCE) $(DEP_CPP_TIQUE) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\imagectn\apps\cnf.cxx

!IF  "$(CFG)" == "ti - Win32 Release"

DEP_CPP_CNF_C=\
	".\config/include\osconfig.h"\
	".\config\include\cfmwerks.h"\
	".\config\include\cfwin32.h"\
	".\imagectn\apps\cnf.h"\
	".\imagectn\apps\cnfpriv.h"\
	
NODEP_CPP_CNF_C=\
	".\imagectn\apps\osconfig.h"\
	

"$(INTDIR)\cnf.obj" : $(SOURCE) $(DEP_CPP_CNF_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ti - Win32 Debug"

DEP_CPP_CNF_C=\
	".\imagectn\apps\cnf.h"\
	".\imagectn\apps\cnfpriv.h"\
	
NODEP_CPP_CNF_C=\
	".\imagectn\apps\osconfig.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\cnf.obj" : $(SOURCE) $(DEP_CPP_CNF_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\cnf.sbr" : $(SOURCE) $(DEP_CPP_CNF_C) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "dbregimg - Win32 Release"
# Name "dbregimg - Win32 Debug"

!IF  "$(CFG)" == "dbregimg - Win32 Release"

!ELSEIF  "$(CFG)" == "dbregimg - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmdata"

!IF  "$(CFG)" == "dbregimg - Win32 Release"

"dcmdata - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Release" 

!ELSEIF  "$(CFG)" == "dbregimg - Win32 Debug"

"dcmdata - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmdata - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "imagectnlib"

!IF  "$(CFG)" == "dbregimg - Win32 Release"

"imagectnlib - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="imagectnlib - Win32 Release" 

!ELSEIF  "$(CFG)" == "dbregimg - Win32 Debug"

"imagectnlib - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="imagectnlib - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\imagectn\tests\dbregimg.cxx
NODEP_CPP_DBREG=\
	".\imagectn\tests\cmdlnarg.h"\
	".\imagectn\tests\dcdebug.h"\
	".\imagectn\tests\dcdict.h"\
	".\imagectn\tests\dcompat.h"\
	".\imagectn\tests\dcuid.h"\
	".\imagectn\tests\dicom.h"\
	".\imagectn\tests\diutil.h"\
	".\imagectn\tests\imagedb.h"\
	".\imagectn\tests\osconfig.h"\
	

"$(INTDIR)\dbregimg.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "dcmnet"

!IF  "$(CFG)" == "dbregimg - Win32 Release"

"dcmnet - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmnet - Win32 Release" 

!ELSEIF  "$(CFG)" == "dbregimg - Win32 Debug"

"dcmnet - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\msvc4.mak" CFG="dcmnet - Win32 Debug" 

!ENDIF 

# End Project Dependency
# End Target
# End Project
################################################################################
