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
#  /I "../../config\include" /I "../../dcmpstat\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmnet\include" /I "../../dcmimgle\include" /I "../../imagectn\include" /I "../../dcmsr\include" /I "../../dcmsign\include" /I "../../dcmtls\include" /I "../../../zlib-1.2.1\include" == include path
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
# ADD CPP  /I "../../config\include" /I "../../dcmpstat\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmnet\include" /I "../../dcmimgle\include" /I "../../imagectn\include" /I "../../dcmsr\include" /I "../../dcmsign\include" /I "../../dcmtls\include" /I "../../../zlib-1.2.1\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "dcmpstat_EXPORTS"
# ADD CPP  /D "_MBCS"
# ADD CPP /nologo /MT /W3 /GX /O2 /GR- /YX -DCMAKE_INTDIR=\"Release\" 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "../../config\include" /I "../../dcmpstat\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmnet\include" /I "../../dcmimgle\include" /I "../../imagectn\include" /I "../../dcmsr\include" /I "../../dcmsign\include" /I "../../dcmtls\include" /I "../../../zlib-1.2.1\include" /l 0x409 /d "NDEBUG"
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
# ADD CPP  /I "../../config\include" /I "../../dcmpstat\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmnet\include" /I "../../dcmimgle\include" /I "../../imagectn\include" /I "../../dcmsr\include" /I "../../dcmsign\include" /I "../../dcmtls\include" /I "../../../zlib-1.2.1\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "dcmpstat_EXPORTS"
# ADD CPP  /D "_MBCS"
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /GR- /YX -DCMAKE_INTDIR=\"Debug\" 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC  /I "../../config\include" /I "../../dcmpstat\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmnet\include" /I "../../dcmimgle\include" /I "../../imagectn\include" /I "../../dcmsr\include" /I "../../dcmsign\include" /I "../../dcmtls\include" /I "../../../zlib-1.2.1\include" /l 0x409 /d "_DEBUG"
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
# ADD CPP  /I "../../config\include" /I "../../dcmpstat\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmnet\include" /I "../../dcmimgle\include" /I "../../imagectn\include" /I "../../dcmsr\include" /I "../../dcmsign\include" /I "../../dcmtls\include" /I "../../../zlib-1.2.1\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "dcmpstat_EXPORTS"
# ADD CPP  /D "_MBCS"
# ADD CPP /MD /O1 -DCMAKE_INTDIR=\"MinSizeRel\" 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "../../config\include" /I "../../dcmpstat\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmnet\include" /I "../../dcmimgle\include" /I "../../imagectn\include" /I "../../dcmsr\include" /I "../../dcmsign\include" /I "../../dcmtls\include" /I "../../../zlib-1.2.1\include" /l 0x409 /d "NDEBUG"
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
# ADD CPP  /I "../../config\include" /I "../../dcmpstat\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmnet\include" /I "../../dcmimgle\include" /I "../../imagectn\include" /I "../../dcmsr\include" /I "../../dcmsign\include" /I "../../dcmtls\include" /I "../../../zlib-1.2.1\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "dcmpstat_EXPORTS"
# ADD CPP  /D "_MBCS"
# ADD CPP /MD /Zi /O2 -DCMAKE_INTDIR=\"RelWithDebInfo\" 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "../../config\include" /I "../../dcmpstat\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmnet\include" /I "../../dcmimgle\include" /I "../../imagectn\include" /I "../../dcmsr\include" /I "../../dcmsign\include" /I "../../dcmtls\include" /I "../../../zlib-1.2.1\include" /l 0x409 /d "NDEBUG"
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

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dviface.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsab.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsabl.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsal.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsall.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpscf.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpscu.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpscul.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsda.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsdal.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsfs.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsga.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsgal.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsgl.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsgll.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsgr.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsgrl.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpshlp.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsib.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsibl.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsmsg.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsov.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsovl.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpspl.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpspl2.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpspll.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpspr.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsprt.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsri.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsril.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsrs.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsrsl.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpssp.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsspl.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpssv.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpssvl.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpstat.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpstx.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpstxl.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsvl.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsvll.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsvw.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvpsvwl.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmpstat\libsrc\dvsighdl.cxx

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
