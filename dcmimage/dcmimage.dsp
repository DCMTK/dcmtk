# Microsoft Developer Studio Project File - Name="dcmimage" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=dcmimage - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit\
 NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den\
 Befehl
!MESSAGE 
!MESSAGE NMAKE /f "dcmimage.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "dcmimage.mak" CFG="dcmimage - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "dcmimage - Win32 Release" (basierend auf\
  "Win32 (x86) Static Library")
!MESSAGE "dcmimage - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "dcmimage - Win32 Release OpenSSL" (basierend auf\
  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "dcmimage - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "include" /I "..\config\include" /I "..\ofstd\include" /I "..\dcmdata\include" /I "..\dcmimgle\include" /I "..\..\zlib-1.1.4\include" /I "..\..\tiff-3.5.7\include" /I "..\..\libpng-1.2.5\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_REENTRANT" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "dcmimage - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /I "include" /I "..\config\include" /I "..\ofstd\include" /I "..\dcmdata\include" /I "..\dcmimgle\include" /I "..\..\zlib-1.1.4\include" /I "..\..\tiff-3.5.7\include" /I "..\..\libpng-1.2.5\include" /D "_DEBUG" /D "DEBUG" /D "WIN32" /D "_WINDOWS" /D "_REENTRANT" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "dcmimage - Win32 Release OpenSSL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "OpenSSL"
# PROP BASE Intermediate_Dir "OpenSSL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "OpenSSL"
# PROP Intermediate_Dir "OpenSSL"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "include" /I "..\config\include" /I "..\ofstd\include" /I "..\dcmdata\include" /I "..\dcmimgle\include" /I "..\..\zlib-1.1.4\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "include" /I "..\config\include" /I "..\ofstd\include" /I "..\dcmdata\include" /I "..\dcmimgle\include" /I "..\..\zlib-1.1.4\include" /I "..\..\tiff-3.5.7\include" /I "..\..\libpng-1.2.5\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_REENTRANT" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "dcmimage - Win32 Release"
# Name "dcmimage - Win32 Debug"
# Name "dcmimage - Win32 Release OpenSSL"
# Begin Source File

SOURCE=.\libsrc\diargimg.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dicmyimg.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dicoimg.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dicoopx.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dicopx.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dihsvimg.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dipalimg.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dipipng.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dipitiff.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\diqtctab.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\diqtfs.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\diqthash.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\diqthitl.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\diqtpbox.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\diquant.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\diregist.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dirgbimg.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\diybrimg.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\diyf2img.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\diyp2img.cxx
# End Source File
# End Target
# End Project
