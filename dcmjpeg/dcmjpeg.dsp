# Microsoft Developer Studio Project File - Name="dcmjpeg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=dcmjpeg - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit\
 NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den\
 Befehl
!MESSAGE 
!MESSAGE NMAKE /f "dcmjpeg.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "dcmjpeg.mak" CFG="dcmjpeg - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "dcmjpeg - Win32 Release" (basierend auf\
  "Win32 (x86) Static Library")
!MESSAGE "dcmjpeg - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "dcmjpeg - Win32 Release OpenSSL" (basierend auf\
  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "include" /I "..\config\include" /I "..\ofstd\include" /I "..\dcmdata\include" /I "..\dcmimgle\include" /I "..\dcmimage\include" /I "libijg8" /I "libijg12" /I "libijg16" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_REENTRANT" /YX /FD /c
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
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /I "include" /I "..\config\include" /I "..\ofstd\include" /I "..\dcmdata\include" /I "..\dcmimgle\include" /I "..\dcmimage\include" /I "libijg8" /I "libijg12" /I "libijg16" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_REENTRANT" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "dcmjpeg - Win32 Release OpenSSL"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "include" /I "..\config\include" /I "..\ofstd\include" /I "..\dcmdata\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_REENTRANT" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "include" /I "..\config\include" /I "..\ofstd\include" /I "..\dcmdata\include" /I "..\dcmimgle\include" /I "..\dcmimage\include" /I "libijg8" /I "libijg12" /I "libijg16" /D "_WINDOWS" /D "WITH_OPENSSL" /D "WIN32" /D "NDEBUG" /D "_REENTRANT" /YX /FD /c
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
# Name "dcmjpeg - Win32 Release OpenSSL"
# Begin Source File

SOURCE=.\libsrc\dipijpeg.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djcodecd.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djcodece.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djcparam.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djdecbas.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djdecext.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djdeclol.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djdecode.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djdecpro.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djdecsps.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djdecsv1.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djdijg12.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djdijg16.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djdijg8.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djeijg12.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djeijg16.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djeijg8.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djencbas.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djencext.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djenclol.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djencode.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djencpro.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djencsps.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djencsv1.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djrplol.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djrploss.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\djutils.cxx
# End Source File
# End Target
# End Project
