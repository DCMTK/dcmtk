# Microsoft Developer Studio Project File - Name="dcmdata" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=dcmdata - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit\
 NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den\
 Befehl
!MESSAGE
!MESSAGE NMAKE /f "dcmdata.mak".
!MESSAGE
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE
!MESSAGE NMAKE /f "dcmdata.mak" CFG="dcmdata - Win32 Debug"
!MESSAGE
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE
!MESSAGE "dcmdata - Win32 Release" (basierend auf\
  "Win32 (x86) Static Library")
!MESSAGE "dcmdata - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "dcmdata - Win32 Release OpenSSL" (basierend auf\
  "Win32 (x86) Static Library")
!MESSAGE

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "dcmdata - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "include" /I "..\ofstd\include" /I "..\config\include" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_REENTRANT" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /I "include" /I "..\ofstd\include" /I "..\config\include" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "DEBUG" /D "_REENTRANT" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "dcmdata - Win32 Release OpenSSL"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "include" /I "..\ofstd\include" /I "..\config\include" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_REENTRANT" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "include" /I "..\ofstd\include" /I "..\config\include" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_REENTRANT" /D "WITH_OPENSSL" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF

# Begin Target

# Name "dcmdata - Win32 Release"
# Name "dcmdata - Win32 Debug"
# Name "dcmdata - Win32 Release OpenSSL"
# Begin Source File

SOURCE=.\libsrc\cmdlnarg.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcbuf.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcbytstr.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcchrstr.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dccodec.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcdatset.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcdebug.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcdicdir.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcdicent.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcdict.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcdictzz.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcdirrec.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcelem.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcerror.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcfilefo.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dchashdi.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcitem.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dclist.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcmetinf.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcobject.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcpcache.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcpixel.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcpixseq.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcpxitem.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcrleccd.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcrlecce.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcrlecp.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcrledrg.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcrleerg.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcrlerp.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcsequen.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcstack.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcstream.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcswap.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dctag.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dctagkey.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dctypes.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcuid.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcutils.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvm.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvr.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvrae.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvras.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvrat.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvrcs.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvrda.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvrds.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvrdt.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvrfd.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvrfl.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvris.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvrlo.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvrlt.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvrobow.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvrpn.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvrpobw.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvrsh.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvrsl.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvrss.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvrst.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvrtm.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvrui.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvrul.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvrulup.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvrus.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcvrut.cxx
# End Source File
# Begin Source File

SOURCE=.\libsrc\dcxfer.cxx
# End Source File
# End Target
# End Project
