# Microsoft Developer Studio Project File - Name="dcmjpeg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# CMAKE DSP Header file
# This file is read by the CMAKE, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit in that case.

# variables to REPLACE
# 
#  /I "../../config\include" /I "../../dcmjpeg\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmimgle\include" /I "../../dcmimage\include" /I "../../dcmjpeg\libijg8" /I "../../dcmjpeg\libijg12" /I "../../dcmjpeg\libijg16" /I "../../../zlib-1.2.1\include" == include path
#   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB == compiler defines
#  == override in output directory
# dcmjpeg  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=dcmjpeg - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dcmjpeg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dcmjpeg.mak" CFG="dcmjpeg - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dcmjpeg - Win32 MinSizeRel" (based on "Win32 (x86) Static Library")
!MESSAGE "dcmjpeg - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "dcmjpeg - Win32 RelWithDebInfo" (based on "Win32 (x86) Static Library")
!MESSAGE "dcmjpeg - Win32 Debug" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

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
# ADD CPP  /I "../../config\include" /I "../../dcmjpeg\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmimgle\include" /I "../../dcmimage\include" /I "../../dcmjpeg\libijg8" /I "../../dcmjpeg\libijg12" /I "../../dcmjpeg\libijg16" /I "../../../zlib-1.2.1\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "dcmjpeg_EXPORTS"
# ADD CPP  /D "_MBCS"
# ADD CPP /nologo /MT /W3 /GX /O2 /GR- /YX -DCMAKE_INTDIR=\"Release\" 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "../../config\include" /I "../../dcmjpeg\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmimgle\include" /I "../../dcmimage\include" /I "../../dcmjpeg\libijg8" /I "../../dcmjpeg\libijg12" /I "../../dcmjpeg\libijg16" /I "../../../zlib-1.2.1\include" /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo



!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

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
# ADD CPP  /I "../../config\include" /I "../../dcmjpeg\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmimgle\include" /I "../../dcmimage\include" /I "../../dcmjpeg\libijg8" /I "../../dcmjpeg\libijg12" /I "../../dcmjpeg\libijg16" /I "../../../zlib-1.2.1\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "dcmjpeg_EXPORTS"
# ADD CPP  /D "_MBCS"
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /GR- /YX -DCMAKE_INTDIR=\"Debug\" 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC  /I "../../config\include" /I "../../dcmjpeg\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmimgle\include" /I "../../dcmimage\include" /I "../../dcmjpeg\libijg8" /I "../../dcmjpeg\libijg12" /I "../../dcmjpeg\libijg16" /I "../../../zlib-1.2.1\include" /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo



!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

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
# ADD CPP  /I "../../config\include" /I "../../dcmjpeg\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmimgle\include" /I "../../dcmimage\include" /I "../../dcmjpeg\libijg8" /I "../../dcmjpeg\libijg12" /I "../../dcmjpeg\libijg16" /I "../../../zlib-1.2.1\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "dcmjpeg_EXPORTS"
# ADD CPP  /D "_MBCS"
# ADD CPP /MD /O1 -DCMAKE_INTDIR=\"MinSizeRel\" 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "../../config\include" /I "../../dcmjpeg\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmimgle\include" /I "../../dcmimage\include" /I "../../dcmjpeg\libijg8" /I "../../dcmjpeg\libijg12" /I "../../dcmjpeg\libijg16" /I "../../../zlib-1.2.1\include" /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo



!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

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
# ADD CPP  /I "../../config\include" /I "../../dcmjpeg\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmimgle\include" /I "../../dcmimage\include" /I "../../dcmjpeg\libijg8" /I "../../dcmjpeg\libijg12" /I "../../dcmjpeg\libijg16" /I "../../../zlib-1.2.1\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "dcmjpeg_EXPORTS"
# ADD CPP  /D "_MBCS"
# ADD CPP /MD /Zi /O2 -DCMAKE_INTDIR=\"RelWithDebInfo\" 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "../../config\include" /I "../../dcmjpeg\include" /I "../../ofstd\include" /I "../../dcmdata\include" /I "../../dcmimgle\include" /I "../../dcmimage\include" /I "../../dcmjpeg\libijg8" /I "../../dcmjpeg\libijg12" /I "../../dcmjpeg\libijg16" /I "../../../zlib-1.2.1\include" /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo



!ENDIF 

# Begin Target

# Name "dcmjpeg - Win32 Release"
# Name "dcmjpeg - Win32 Debug"
# Name "dcmjpeg - Win32 MinSizeRel"
# Name "dcmjpeg - Win32 RelWithDebInfo"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\ddpiimpl.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\dipijpeg.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djcodecd.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djcodece.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djcparam.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djdecbas.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djdecext.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djdeclol.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djdecode.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djdecpro.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djdecsps.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djdecsv1.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djdijg12.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djdijg8.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djdijg16.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djeijg12.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djeijg8.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djeijg16.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djencbas.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djencext.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djenclol.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djencode.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djencpro.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djencsps.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djencsv1.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djrplol.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djrploss.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libsrc\djutils.cxx

!IF  "$(CFG)" == "dcmjpeg - Win32 Release"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Debug"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 MinSizeRel"

# ADD CPP  /TP 

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 RelWithDebInfo"

# ADD CPP  /TP 

!ENDIF

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
