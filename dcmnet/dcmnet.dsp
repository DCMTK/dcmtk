# Microsoft Developer Studio Project File - Name="dcmnet" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=dcmnet - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit\
 NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den\
 Befehl
!MESSAGE 
!MESSAGE NMAKE /f "dcmnet.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "dcmnet.mak" CFG="dcmnet - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "dcmnet - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "dcmnet - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "dcmnet - Win32 Release OpenSSL" (basierend auf\
  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "dcmnet - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "include" /I "..\config\include" /I "..\ofstd\include" /I "..\dcmdata\include" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_REENTRANT" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "dcmnet - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /I "include" /I "..\config\include" /I "..\ofstd\include" /I "..\dcmdata\include" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "DEBUG" /D "_REENTRANT" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "dcmnet - Win32 Release OpenSSL"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "include" /I "..\config\include" /I "..\ofstd\include" /I "..\dcmdata\include" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_REENTRANT" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "include" /I "..\config\include" /I "..\ofstd\include" /I "..\dcmdata\include" /D "_WINDOWS" /D "WITH_OPENSSL" /D "WIN32" /D "NDEBUG" /D "_REENTRANT" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "dcmnet - Win32 Release"
# Name "dcmnet - Win32 Debug"
# Name "dcmnet - Win32 Release OpenSSL"
# Begin Source File

SOURCE=.\libsrc\assoc.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\cond.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcmlayer.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcmtrans.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcompat.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dimcancl.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dimcmd.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dimdump.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dimecho.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dimfind.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dimget.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dimmove.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dimse.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dimstore.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\diutil.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dul.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dulconst.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dulextra.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dulfsm.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dulparse.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dulpres.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\extneg.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\lst.cxx
# End Source File
# End Target
# End Project
