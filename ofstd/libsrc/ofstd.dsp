# Microsoft Developer Studio Project File - Name="ofstd" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# CMAKE DSP Header file
# This file is read by the CMAKE, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit in that case.

# variables to REPLACE
# 
#  /I "../../config\include" /I "../../ofstd\include" == include path
#   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB == compiler defines
#  == override in output directory
# ofstd  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ofstd - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ofstd.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ofstd.mak" CFG="ofstd - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ofstd - Win32 MinSizeRel" (based on "Win32 (x86) Static Library")
!MESSAGE "ofstd - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ofstd - Win32 RelWithDebInfo" (based on "Win32 (x86) Static Library")
!MESSAGE "ofstd - Win32 Debug" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ofstd - Win32 Release"

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
# ADD CPP  /I "../../config\include" /I "../../ofstd\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "ofstd_EXPORTS"
# ADD CPP  /D "_MBCS"
# ADD CPP /nologo /MT /W3 /GX /O2 /GR- /YX -DCMAKE_INTDIR=\"Release\" 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "../../config\include" /I "../../ofstd\include" /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo



!ELSEIF  "$(CFG)" == "ofstd - Win32 Debug"

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
# ADD CPP  /I "../../config\include" /I "../../ofstd\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "ofstd_EXPORTS"
# ADD CPP  /D "_MBCS"
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /GR- /YX -DCMAKE_INTDIR=\"Debug\" 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC  /I "../../config\include" /I "../../ofstd\include" /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo



!ELSEIF  "$(CFG)" == "ofstd - Win32 MinSizeRel"

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
# ADD CPP  /I "../../config\include" /I "../../ofstd\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "ofstd_EXPORTS"
# ADD CPP  /D "_MBCS"
# ADD CPP /MD /O1 -DCMAKE_INTDIR=\"MinSizeRel\" 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "../../config\include" /I "../../ofstd\include" /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo



!ELSEIF  "$(CFG)" == "ofstd - Win32 RelWithDebInfo"

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
# ADD CPP  /I "../../config\include" /I "../../ofstd\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "ofstd_EXPORTS"
# ADD CPP  /D "_MBCS"
# ADD CPP /MD /Zi /O2 -DCMAKE_INTDIR=\"RelWithDebInfo\" 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "../../config\include" /I "../../ofstd\include" /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo



!ENDIF 

# Begin Target

# Name "ofstd - Win32 Release"
# Name "ofstd - Win32 Debug"
# Name "ofstd - Win32 MinSizeRel"
# Name "ofstd - Win32 RelWithDebInfo"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=../../ofstd\libsrc\ofcmdln.cxx

!IF  "$(CFG)" == "ofstd - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../ofstd\libsrc\ofconapp.cxx

!IF  "$(CFG)" == "ofstd - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../ofstd\libsrc\ofcond.cxx

!IF  "$(CFG)" == "ofstd - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../ofstd\libsrc\ofconfig.cxx

!IF  "$(CFG)" == "ofstd - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../ofstd\libsrc\ofconsol.cxx

!IF  "$(CFG)" == "ofstd - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../ofstd\libsrc\ofcrc32.cxx

!IF  "$(CFG)" == "ofstd - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../ofstd\libsrc\ofdate.cxx

!IF  "$(CFG)" == "ofstd - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../ofstd\libsrc\ofdatime.cxx

!IF  "$(CFG)" == "ofstd - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../ofstd\libsrc\offname.cxx

!IF  "$(CFG)" == "ofstd - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../ofstd\libsrc\oflist.cxx

!IF  "$(CFG)" == "ofstd - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../ofstd\libsrc\oflogfil.cxx

!IF  "$(CFG)" == "ofstd - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../ofstd\libsrc\ofstd.cxx

!IF  "$(CFG)" == "ofstd - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../ofstd\libsrc\ofstring.cxx

!IF  "$(CFG)" == "ofstd - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../ofstd\libsrc\ofthread.cxx

!IF  "$(CFG)" == "ofstd - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../ofstd\libsrc\oftime.cxx

!IF  "$(CFG)" == "ofstd - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "ofstd - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
