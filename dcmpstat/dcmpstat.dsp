# Microsoft Developer Studio Project File - Name="dcmpstat" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=dcmpstat - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit\
 NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den\
 Befehl
!MESSAGE 
!MESSAGE NMAKE /f "dcmpstat.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "dcmpstat.mak" CFG="dcmpstat - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "dcmpstat - Win32 Release" (basierend auf\
  "Win32 (x86) Static Library")
!MESSAGE "dcmpstat - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "dcmpstat - Win32 Release OpenSSL" (basierend auf\
  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "dcmpstat - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "include" /I "..\config\include" /I "..\ofstd\include" /I "..\dcmdata\include" /I "..\dcmimgle\include" /I "..\dcmnet\include" /I "..\imagectn\include" /I "..\dcmsr\include" /I "..\dcmsign\include" /I "..\..\..\zlib-1.1.4\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "_REENTRANT" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /I "include" /I "..\config\include" /I "..\ofstd\include" /I "..\dcmdata\include" /I "..\dcmimgle\include" /I "..\dcmnet\include" /I "..\imagectn\include" /I "..\dcmsr\include" /I "..\dcmsign\include" /I "..\..\..\zlib-1.1.4\include" /D "_DEBUG" /D "DEBUG" /D "_WINDOWS" /D "WIN32" /D "_REENTRANT" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "dcmpstat - Win32 Release OpenSSL"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "include" /I "..\config\include" /I "..\ofstd\include" /I "..\dcmdata\include" /I "..\dcmimgle\include" /I "..\dcmnet\include" /I "..\imagectn\include" /I "..\dcmsr\include" /I "..\..\..\zlib-1.1.4\include" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_REENTRANT" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\openssl-0.9.6g\include" /I "include" /I "..\config\include" /I "..\ofstd\include" /I "..\dcmdata\include" /I "..\dcmimgle\include" /I "..\dcmnet\include" /I "..\imagectn\include" /I "..\dcmsr\include" /I "..\dcmsign\include" /I "..\..\..\zlib-1.1.4\include" /D "NDEBUG" /D "WITH_OPENSSL" /D "_WINDOWS" /D "WIN32" /D "_REENTRANT" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "dcmpstat - Win32 Release"
# Name "dcmpstat - Win32 Debug"
# Name "dcmpstat - Win32 Release OpenSSL"
# Begin Source File

SOURCE=.\libsrc\dviface.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsab.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsabl.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsal.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsall.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpscf.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsconf.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpscu.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpscul.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsda.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsdal.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsfs.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsga.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsgal.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsgl.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsgll.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsgr.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsgrl.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpshlp.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsib.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsibl.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsmsg.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsov.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsovl.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpspl.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpspll.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpspr.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsprt.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsri.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsril.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsrs.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsrsl.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpssp.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsspl.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpssv.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpssvl.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpstat.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpstx.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpstxl.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsvl.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsvll.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsvw.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvpsvwl.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dvsighdl.cxx
# End Source File
# End Target
# End Project
