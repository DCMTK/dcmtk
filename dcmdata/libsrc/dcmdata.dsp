# Microsoft Developer Studio Project File - Name="dcmdata" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# CMAKE DSP Header file
# This file is read by the CMAKE, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit in that case.

# variables to REPLACE
# 
#  /I "../../config\include" /I "../../dcmdata\include" /I "../../ofstd\include" /I "../../../zlib-1.1.4\include" == include path
#   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB == compiler defines
#  == override in output directory
# dcmdata  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=dcmdata - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dcmdata.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dcmdata.mak" CFG="dcmdata - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dcmdata - Win32 MinSizeRel" (based on "Win32 (x86) Static Library")
!MESSAGE "dcmdata - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "dcmdata - Win32 RelWithDebInfo" (based on "Win32 (x86) Static Library")
!MESSAGE "dcmdata - Win32 Debug" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dcmdata - Win32 Release"

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
# ADD CPP  /I "../../config\include" /I "../../dcmdata\include" /I "../../ofstd\include" /I "../../../zlib-1.1.4\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "dcmdata_EXPORTS"
# ADD CPP  
# ADD CPP /nologo /MT /W3 /GX /O2 /GR- /YX -DCMAKE_INTDIR=\"Release\" 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "../../config\include" /I "../../dcmdata\include" /I "../../ofstd\include" /I "../../../zlib-1.1.4\include" /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo



!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

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
# ADD CPP  /I "../../config\include" /I "../../dcmdata\include" /I "../../ofstd\include" /I "../../../zlib-1.1.4\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "dcmdata_EXPORTS"
# ADD CPP  
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /GR- /YX -DCMAKE_INTDIR=\"Debug\" 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC  /I "../../config\include" /I "../../dcmdata\include" /I "../../ofstd\include" /I "../../../zlib-1.1.4\include" /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo



!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

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
# ADD CPP  /I "../../config\include" /I "../../dcmdata\include" /I "../../ofstd\include" /I "../../../zlib-1.1.4\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "dcmdata_EXPORTS"
# ADD CPP  
# ADD CPP /MD /O1 -DCMAKE_INTDIR=\"MinSizeRel\" 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "../../config\include" /I "../../dcmdata\include" /I "../../ofstd\include" /I "../../../zlib-1.1.4\include" /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo



!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

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
# ADD CPP  /I "../../config\include" /I "../../dcmdata\include" /I "../../ofstd\include" /I "../../../zlib-1.1.4\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "dcmdata_EXPORTS"
# ADD CPP  
# ADD CPP /MD /Zi /O2 -DCMAKE_INTDIR=\"RelWithDebInfo\" 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "../../config\include" /I "../../dcmdata\include" /I "../../ofstd\include" /I "../../../zlib-1.1.4\include" /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo



!ENDIF 

# Begin Target

# Name "dcmdata - Win32 Release"
# Name "dcmdata - Win32 Debug"
# Name "dcmdata - Win32 MinSizeRel"
# Name "dcmdata - Win32 RelWithDebInfo"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=../../dcmdata\libsrc\cmdlnarg.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcbytstr.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcchrstr.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dccodec.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcdatset.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcddirif.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcdebug.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcdicdir.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcdicent.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcdict.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcdictzz.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcdirrec.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcelem.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcerror.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcfilefo.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dchashdi.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcistrma.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcistrmb.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcistrmf.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcistrmz.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcitem.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dclist.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcmetinf.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcobject.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcostrma.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcostrmb.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcostrmf.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcostrmz.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcpcache.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcpixel.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcpixseq.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcpxitem.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcrleccd.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcrlecce.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcrlecp.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcrledrg.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcrleerg.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcrlerp.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcsequen.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcstack.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcswap.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dctag.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dctagkey.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dctypes.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcuid.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvm.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvr.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrae.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvras.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrat.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrcs.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrda.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrds.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrdt.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrfd.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrfl.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvris.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrlo.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrlt.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrobow.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrof.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrpn.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrpobw.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrsh.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrsl.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrss.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrst.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrtm.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrui.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrul.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrulup.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrus.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrut.cxx

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcxfer.cxx

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
