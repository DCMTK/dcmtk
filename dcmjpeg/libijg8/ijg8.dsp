# Microsoft Developer Studio Project File - Name="ijg8" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# CMAKE DSP Header file
# This file is read by the CMAKE, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit in that case.

# variables to REPLACE
# 
#  /I "../../config\include" == include path
#   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB == compiler defines
#  == override in output directory
# ijg8  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ijg8 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ijg8.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ijg8.mak" CFG="ijg8 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ijg8 - Win32 MinSizeRel" (based on "Win32 (x86) Static Library")
!MESSAGE "ijg8 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ijg8 - Win32 RelWithDebInfo" (based on "Win32 (x86) Static Library")
!MESSAGE "ijg8 - Win32 Debug" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ijg8 - Win32 Release"

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
# ADD CPP  /I "../../config\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "ijg8_EXPORTS"
# ADD CPP  /D "_MBCS"
# ADD CPP  /nologo /MT /W3 /GX /O2 /GR- /YX -DCMAKE_INTDIR=\"Release\"
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "../../config\include" /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo



!ELSEIF  "$(CFG)" == "ijg8 - Win32 Debug"

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
# ADD CPP  /I "../../config\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "ijg8_EXPORTS"
# ADD CPP  /D "_MBCS"
# ADD CPP  /nologo /MTd /W3 /GX /Z7 /Od /GR- /YX -DCMAKE_INTDIR=\"Debug\"
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC  /I "../../config\include" /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo



!ELSEIF  "$(CFG)" == "ijg8 - Win32 MinSizeRel"

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
# ADD CPP  /I "../../config\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "ijg8_EXPORTS"
# ADD CPP  /D "_MBCS"
# ADD CPP  /MD /O1 -DCMAKE_INTDIR=\"MinSizeRel\"
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "../../config\include" /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo



!ELSEIF  "$(CFG)" == "ijg8 - Win32 RelWithDebInfo"

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
# ADD CPP  /I "../../config\include"   -D_REENTRANT -DWITH_LIBPNG -DWITH_LIBTIFF -DWITH_ZLIB /D "ijg8_EXPORTS"
# ADD CPP  /D "_MBCS"
# ADD CPP  /MD /Zi /O2 -DCMAKE_INTDIR=\"RelWithDebInfo\"
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "../../config\include" /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo



!ENDIF 

# Begin Target

# Name "ijg8 - Win32 Release"
# Name "ijg8 - Win32 Debug"
# Name "ijg8 - Win32 MinSizeRel"
# Name "ijg8 - Win32 RelWithDebInfo"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jaricom.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jcapimin.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jcapistd.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jcarith.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jccoefct.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jccolor.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jcdctmgr.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jcdiffct.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jchuff.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jcinit.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jclhuff.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jclossls.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jclossy.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jcmainct.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jcmarker.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jcmaster.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jcodec.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jcomapi.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jcparam.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jcphuff.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jcpred.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jcprepct.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jcsample.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jcscale.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jcshuff.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jctrans.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jdapimin.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jdapistd.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jdarith.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jdatadst.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jdatasrc.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jdcoefct.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jdcolor.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jddctmgr.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jddiffct.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jdhuff.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jdinput.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jdlhuff.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jdlossls.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jdlossy.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jdmainct.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jdmarker.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jdmaster.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jdmerge.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jdphuff.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jdpostct.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jdpred.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jdsample.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jdscale.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jdshuff.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jdtrans.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jerror.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jfdctflt.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jfdctfst.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jfdctint.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jidctflt.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jidctfst.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jidctint.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jidctred.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jmemmgr.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jmemnobs.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jquant1.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jquant2.c

# End Source File
# Begin Source File

SOURCE=../../dcmjpeg\libijg8\jutils.c

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
