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
#  /I "../../config\include" /I "../../dcmdata\include" /I "../../ofstd\include" /I "../../../zlib-1.2.1\include" == include path
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
# ADD CPP  /I "../../config\include" /I "../../dcmdata\include" /I "../../ofstd\include" /I "../../../zlib-1.2.1\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "dcmdata_EXPORTS"
# ADD CPP  /D "_MBCS"
# ADD CPP /nologo /MT /W3 /GX /O2 /GR- /YX -DCMAKE_INTDIR=\"Release\" 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "../../config\include" /I "../../dcmdata\include" /I "../../ofstd\include" /I "../../../zlib-1.2.1\include" /l 0x409 /d "NDEBUG"
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
# ADD CPP  /I "../../config\include" /I "../../dcmdata\include" /I "../../ofstd\include" /I "../../../zlib-1.2.1\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "dcmdata_EXPORTS"
# ADD CPP  /D "_MBCS"
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /GR- /YX -DCMAKE_INTDIR=\"Debug\" 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC  /I "../../config\include" /I "../../dcmdata\include" /I "../../ofstd\include" /I "../../../zlib-1.2.1\include" /l 0x409 /d "_DEBUG"
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
# ADD CPP  /I "../../config\include" /I "../../dcmdata\include" /I "../../ofstd\include" /I "../../../zlib-1.2.1\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "dcmdata_EXPORTS"
# ADD CPP  /D "_MBCS"
# ADD CPP /MD /O1 -DCMAKE_INTDIR=\"MinSizeRel\" 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "../../config\include" /I "../../dcmdata\include" /I "../../ofstd\include" /I "../../../zlib-1.2.1\include" /l 0x409 /d "NDEBUG"
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
# ADD CPP  /I "../../config\include" /I "../../dcmdata\include" /I "../../ofstd\include" /I "../../../zlib-1.2.1\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "dcmdata_EXPORTS"
# ADD CPP  /D "_MBCS"
# ADD CPP /MD /Zi /O2 -DCMAKE_INTDIR=\"RelWithDebInfo\" 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "../../config\include" /I "../../dcmdata\include" /I "../../ofstd\include" /I "../../../zlib-1.2.1\include" /l 0x409 /d "NDEBUG"
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

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcbytstr.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcchrstr.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dccodec.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcdatset.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcddirif.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcdebug.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcdicdir.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcdicent.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcdict.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcdictzz.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcdirrec.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcelem.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcerror.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcfilefo.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dchashdi.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcistrma.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcistrmb.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcistrmf.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcistrmz.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcitem.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dclist.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcmetinf.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcobject.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcostrma.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcostrmb.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcostrmf.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcostrmz.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcpcache.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcpixel.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcpixseq.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcpxitem.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcrleccd.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcrlecce.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcrlecp.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcrledrg.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcrleerg.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcrlerp.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcsequen.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcstack.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcswap.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dctag.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dctagkey.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dctypes.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcuid.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvm.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvr.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrae.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvras.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrat.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrcs.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrda.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrds.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrdt.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrfd.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrfl.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvris.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrlo.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrlt.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrobow.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrof.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrpn.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrpobw.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrsh.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrsl.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrss.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrst.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrtm.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrui.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrul.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrulup.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrus.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcvrut.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmdata\libsrc\dcxfer.cxx

!IF  "$(CFG)" == "dcmdata - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmdata - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
