# Microsoft Developer Studio Project File - Name="dcmpstat" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# CMAKE DSP Header file
# This file is read by the CMAKE, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit in that case.

# variables to REPLACE
# 
#  /I "../../config\include" /I "../../dcmpstat\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmnet\include" /I "../../dcmimgle\include" /I "../../imagectn\include" /I "../../dcmsr\include" /I "../../dcmsign\include" /I "../../../zlib-1.1.4\include" == include path
#   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB == compiler defines
#  == override in output directory
# dcmpstat  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=dcmpstat - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dcmpstat.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dcmpstat.mak" CFG="dcmpstat - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dcmpstat - Win32 MinSizeRel" (based on "Win32 (x86) Static Library")
!MESSAGE "dcmpstat - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "dcmpstat - Win32 RelWithDebInfo" (based on "Win32 (x86) Static Library")
!MESSAGE "dcmpstat - Win32 Debug" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

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
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP  /I "../../config\include" /I "../../dcmpstat\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmnet\include" /I "../../dcmimgle\include" /I "../../imagectn\include" /I "../../dcmsr\include" /I "../../dcmsign\include" /I "../../../zlib-1.1.4\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "dcmpstat_EXPORTS"
# ADD CPP   /W3 /Zm1000 /GX /GR 
# ADD CPP /nologo /MT /W3 /GX /O2 /GR- /YX -DCMAKE_INTDIR=\"Release\" 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "../../config\include" /I "../../dcmpstat\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmnet\include" /I "../../dcmimgle\include" /I "../../imagectn\include" /I "../../dcmsr\include" /I "../../dcmsign\include" /I "../../../zlib-1.1.4\include" /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo



!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

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
# ADD BASE CPP /nologo /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /GZ /c
# ADD CPP  /I "../../config\include" /I "../../dcmpstat\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmnet\include" /I "../../dcmimgle\include" /I "../../imagectn\include" /I "../../dcmsr\include" /I "../../dcmsign\include" /I "../../../zlib-1.1.4\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "dcmpstat_EXPORTS"
# ADD CPP   /W3 /Zm1000 /GX /GR 
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /GR- /YX -DCMAKE_INTDIR=\"Debug\" 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC  /I "../../config\include" /I "../../dcmpstat\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmnet\include" /I "../../dcmimgle\include" /I "../../imagectn\include" /I "../../dcmsr\include" /I "../../dcmsign\include" /I "../../../zlib-1.1.4\include" /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo



!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MinSizeRel"
# PROP BASE Intermediate_Dir "MinSizeRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MinSizeRel"
# PROP Intermediate_Dir "MinSizeRel"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_DLL" /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "_MBCS" /D "_ATL_DLL"  /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /FD /c
# ADD CPP  /I "../../config\include" /I "../../dcmpstat\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmnet\include" /I "../../dcmimgle\include" /I "../../imagectn\include" /I "../../dcmsr\include" /I "../../dcmsign\include" /I "../../../zlib-1.1.4\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "dcmpstat_EXPORTS"
# ADD CPP   /W3 /Zm1000 /GX /GR 
# ADD CPP /MD /O1 -DCMAKE_INTDIR=\"MinSizeRel\" 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "../../config\include" /I "../../dcmpstat\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmnet\include" /I "../../dcmimgle\include" /I "../../imagectn\include" /I "../../dcmsr\include" /I "../../dcmsign\include" /I "../../../zlib-1.1.4\include" /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo



!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "RelWithDebInfo"
# PROP BASE Intermediate_Dir "RelWithDebInfo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "RelWithDebInfo"
# PROP Intermediate_Dir "RelWithDebInfo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP  /I "../../config\include" /I "../../dcmpstat\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmnet\include" /I "../../dcmimgle\include" /I "../../imagectn\include" /I "../../dcmsr\include" /I "../../dcmsign\include" /I "../../../zlib-1.1.4\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "dcmpstat_EXPORTS"
# ADD CPP   /W3 /Zm1000 /GX /GR 
# ADD CPP /MD /Zi /O2 -DCMAKE_INTDIR=\"RelWithDebInfo\" 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "../../config\include" /I "../../dcmpstat\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmnet\include" /I "../../dcmimgle\include" /I "../../imagectn\include" /I "../../dcmsr\include" /I "../../dcmsign\include" /I "../../../zlib-1.1.4\include" /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo



!ENDIF 

# Begin Target

# Name "dcmpstat - Win32 Release"
# Name "dcmpstat - Win32 Debug"
# Name "dcmpstat - Win32 MinSizeRel"
# Name "dcmpstat - Win32 RelWithDebInfo"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dcmpstat.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dviface.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsab.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsabl.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsal.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsall.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpscf.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpscu.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpscul.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsda.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsdal.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsfs.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsga.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsgal.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsgl.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsgll.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsgr.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsgrl.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpshlp.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsib.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsibl.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsmsg.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsov.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsovl.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpspl.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpspl2.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpspll.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpspr.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsprt.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsri.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsril.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsrs.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsrsl.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpssp.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsspl.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpssv.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpssvl.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpstat.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpstx.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpstxl.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsvl.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsvll.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsvw.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsvwl.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvsighdl.cxx

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
