# Microsoft Developer Studio Project File - Name="dcmsr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=dcmsr - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit\
 NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den\
 Befehl
!MESSAGE 
!MESSAGE NMAKE /f "dcmsr.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "dcmsr.mak" CFG="dcmsr - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "dcmsr - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "dcmsr - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "dcmsr - Win32 Release OpenSSL" (basierend auf\
  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "dcmsr - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "include" /I "..\config\include" /I "..\ofstd\include" /I "..\dcmdata\include" /I "..\..\zlib-1.1.4\include" /I "..\..\libxml2-2.5.7\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_REENTRANT" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "dcmsr - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /I "include" /I "..\config\include" /I "..\ofstd\include" /I "..\dcmdata\include" /I "..\..\zlib-1.1.4\include" /I "..\..\libxml2-2.5.7\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_REENTRANT" /D "DEBUG" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "dcmsr - Win32 Release OpenSSL"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "include" /I "..\config\include" /I "..\ofstd\include" /I "..\dcmdata\include" /I "..\..\zlib-1.1.4\include" /I "..\..\libxml2-2.5.7\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_REENTRANT" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "include" /I "..\config\include" /I "..\ofstd\include" /I "..\dcmdata\include" /I "..\..\zlib-1.1.4\include" /I "..\..\libxml2-2.5.7\include" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_REENTRANT" /D "WITH_OPENSSL" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "dcmsr - Win32 Release"
# Name "dcmsr - Win32 Debug"
# Name "dcmsr - Win32 Release OpenSSL"
# Begin Source File

SOURCE=.\libsrc\dsrcitem.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrcodtn.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrcodvl.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrcomtn.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrcomvl.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrcontn.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrdattn.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrdoc.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrdoctn.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrdoctr.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrdtitn.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrimgfr.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrimgtn.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrimgvl.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrnumtn.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrnumvl.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrpnmtn.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrreftn.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrscogr.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrscotn.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrscovl.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrsoprf.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrstrvl.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrtcodt.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrtcosp.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrtcotn.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrtcoto.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrtcovl.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrtextn.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrtimtn.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrtncsr.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrtree.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrtypes.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsruidtn.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrwavch.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrwavtn.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrwavvl.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrxmlc.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dsrxmld.cxx
# End Source File
# End Target
# End Project
