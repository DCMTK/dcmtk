# Microsoft Developer Studio Project File - Name="dcmmklut" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=dcmmklut - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit\
 NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den\
 Befehl
!MESSAGE 
!MESSAGE NMAKE /f "dcmmklut.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "dcmmklut.mak" CFG="dcmmklut - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "dcmmklut - Win32 Release" (basierend auf\
  "Win32 (x86) Console Application")
!MESSAGE "dcmmklut - Win32 Debug" (basierend auf\
  "Win32 (x86) Console Application")
!MESSAGE "dcmmklut - Win32 Release OpenSSL" (basierend auf\
  "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dcmmklut - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\dcmimgle\include" /I "..\..\imagectn\include" /I "..\include" /I "..\..\config\include" /I "..\..\ofstd\include" /I "..\..\dcmdata\include" /I "..\..\dcmnet\include" /I "..\..\dcmsr\include" /I "..\..\..\zlib-1.1.4\include" /D "_CONSOLE" /D "_MBCS" /D "WIN32" /D "NDEBUG" /D "_REENTRANT" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 netapi32.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ..\..\..\zlib-1.1.4\release\zlib.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "dcmmklut - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\..\dcmimgle\include" /I "..\..\imagectn\include" /I "..\include" /I "..\..\config\include" /I "..\..\ofstd\include" /I "..\..\dcmdata\include" /I "..\..\dcmnet\include" /I "..\..\dcmsr\include" /I "..\..\..\zlib-1.1.4\include" /D "_CONSOLE" /D "_MBCS" /D "WIN32" /D "_DEBUG" /D "DEBUG" /D "_REENTRANT" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 netapi32.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ..\..\..\zlib-1.1.4\debug\zlib.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "dcmmklut - Win32 Release OpenSSL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "OpenSSL"
# PROP BASE Intermediate_Dir "OpenSSL"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "OpenSSL"
# PROP Intermediate_Dir "OpenSSL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\..\dcmimgle\include" /I "..\..\imagectn\include" /I "..\include" /I "..\..\config\include" /I "..\..\ofstd\include" /I "..\..\dcmdata\include" /I "..\..\dcmnet\include" /I "..\..\dcmsr\include" /I "..\..\..\zlib-1.1.4\include" /D "_CONSOLE" /D "_MBCS" /D "WIN32" /D "NDEBUG" /D "_REENTRANT" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\dcmimgle\include" /I "..\..\imagectn\include" /I "..\include" /I "..\..\config\include" /I "..\..\ofstd\include" /I "..\..\dcmdata\include" /I "..\..\dcmnet\include" /I "..\..\dcmsr\include" /I "..\..\..\zlib-1.1.4\include" /D "_CONSOLE" /D "_MBCS" /D "WIN32" /D "NDEBUG" /D "_REENTRANT" /D "WITH_OPENSSL" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 netapi32.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ..\..\..\zlib-1.1.4\release\zlib.lib /nologo /subsystem:console /machine:I386

!ENDIF 

# Begin Target

# Name "dcmmklut - Win32 Release"
# Name "dcmmklut - Win32 Debug"
# Name "dcmmklut - Win32 Release OpenSSL"
# Begin Source File

SOURCE=.\dcmmklut.cxx
# End Source File
# End Target
# End Project
