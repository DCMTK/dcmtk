# Microsoft Developer Studio Project File - Name="dcmsign" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=dcmsign - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit\
 NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den\
 Befehl
!MESSAGE 
!MESSAGE NMAKE /f "dcmsign.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "dcmsign.mak" CFG="dcmsign - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "dcmsign - Win32 Release" (basierend auf\
  "Win32 (x86) Static Library")
!MESSAGE "dcmsign - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "dcmsign - Win32 Release OpenSSL" (basierend auf\
  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "dcmsign - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "include" /I "..\config\include" /I "..\ofstd\include" /I "..\dcmdata\include" /I "..\..\zlib-1.1.4\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_REENTRANT" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "dcmsign - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /I "include" /I "..\config\include" /I "..\ofstd\include" /I "..\dcmdata\include" /I "..\..\zlib-1.1.4\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_REENTRANT" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "dcmsign - Win32 Release OpenSSL"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "include" /I "..\config\include" /I "..\ofstd\include" /I "..\dcmdata\include" /I "..\..\zlib-1.1.4\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_REENTRANT" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\openssl-0.9.6g\include" /I "include" /I "..\config\include" /I "..\ofstd\include" /I "..\dcmdata\include" /I "..\..\zlib-1.1.4\include" /D "_WINDOWS" /D "WITH_OPENSSL" /D "WIN32" /D "NDEBUG" /D "_REENTRANT" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "dcmsign - Win32 Release"
# Name "dcmsign - Win32 Debug"
# Name "dcmsign - Win32 Release OpenSSL"
# Begin Source File

SOURCE=.\libsrc\dcsignat.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\siautopr.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\sibrsapr.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\sicert.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\sicertvf.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\sicreapr.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\sidsa.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\simaccon.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\simd5.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\sinullpr.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\siprivat.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\siripemd.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\sirsa.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\sisha1.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\sisprof.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\sitypes.cxx
# End Source File
# End Target
# End Project
