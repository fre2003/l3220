# Microsoft Developer Studio Project File - Name="tp_core10" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=tp_core10 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "core10.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "core10.mak" CFG="tp_core10 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tp_core10 - Win32 Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tp_core10 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tp_core10 - Win32 Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tp_core10 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tp_core10 - Win32 Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_Unicode"
# PROP BASE Intermediate_Dir "Release_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_Unicode"
# PROP Intermediate_Dir "Release_Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "../../../../deploy/inc" /I "../inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HAVE_NO_ITS_BCORE_LIB" /D "ITS_HAVE_EXPORT_DLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp4 /MT /W3 /GX /O2 /I "../../../../deploy/inc" /I "../inc" /D "HAVE_NO_DELIVERY_VER" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNICODE" /D "_UNICODE" /D "HAVE_NO_ITS_BCORE_LIB" /D "ITS_HAVE_EXPORT_DLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../../../../deploy/bin/its_bcore.dll" /implib:"../../../../deploy/lib/its_bcore.lib" /libpath:"../../../../../deploy/lib" /libpath:"../lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:"../../../../deploy/dbg/pdb/its_bcoreU.pdb" /machine:I386 /out:"../../../../deploy/bin/its_bcoreU.dll" /implib:"../../../../deploy/lib/its_bcoreU.lib" /libpath:"../../../../deploy/lib" /libpath:"../lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "tp_core10 - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ITS_BCORE_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp4 /MT /W3 /GX /O2 /I "../../../../deploy/inc" /I "../inc" /D "HAVE_NO_DELIVERY_VER" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HAVE_NO_ITS_BASSE_LIB" /D "HAVE_NO_ITS_BCORE_LIB" /D "ITS_HAVE_EXPORT_DLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../../../../deploy/bin/its_bcore.dll" /implib:"../../../../deploy/lib/its_bcore.lib" /libpath:"../../../../deploy/lib" /libpath:"../lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "tp_core10 - Win32 Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Unicode_Debug"
# PROP BASE Intermediate_Dir "Unicode_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_Unicode"
# PROP Intermediate_Dir "Debug_Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../../deploy/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HAVE_NO_ITS_BCORE_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /Zp4 /MTd /W3 /GX /Od /I "../../../../deploy/inc" /I "../inc" /D "HAVE_NO_DELIVERY_VER" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNICODE" /D "_UNICODE" /D "HAVE_NO_ITS_BCORE_LIB" /D "ITS_HAVE_EXPORT_DLL" /D "ITS_HAVE_NO_NEW" /Yu"StdAfx.h" /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../../../../delivery/bin/its_bcoreD.dll" /implib:"../../../../delivery/lib/its_bcoreD.lib" /pdbtype:sept /libpath:"../../../../delivery/lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:"../../../../deploy/dbg/pdb/its_bcoreUD.pdb" /machine:I386 /out:"../../../../deploy/bin/its_bcoreUD.dll" /implib:"../../../../deploy/lib/its_bcoreUD.lib" /pdbtype:sept /libpath:"../../../../deploy/lib" /libpath:"../lib" /COMMENT:"RISS''Copyright''2009''Core''File"
# SUBTRACT LINK32 /pdb:none /debug
# Begin Custom Build
OutDir=.\Debug_Unicode
TargetPath=\RISS\deploy\bin\its_bcoreUD.dll
TargetName=its_bcoreUD
InputPath=\RISS\deploy\bin\its_bcoreUD.dll
SOURCE="$(InputPath)"

"$(TargetPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(OutDir)\$(TargetName) ../../../../../ise/bin

# End Custom Build

!ELSEIF  "$(CFG)" == "tp_core10 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../../deploy/inc" /I "../inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_UNICODE" /D "UNICODE" /D "HAVE_NO_ITS_BCORE_LIB" /D "ITS_HAVE_EXPORT_DLL" /FR /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /Zp4 /MTd /W3 /GX /ZI /Od /I "../../../deploy/inc" /I "../inc" /D "HAVE_NO_DELIVERY_VER" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ITS_HAVE_NO_MFC" /D "HAVE_NO_ITS_CORE_LIB" /D "ITS_HAVE_EXPORT_DLL" /D "ITS_HAVE_CORE10_API" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:"../../../../deploy/dbg/pdb/its_bcoreUD.pdb" /debug /machine:I386 /out:"../../../../deploy/bin/its_bcoreUD.dll" /implib:"../../../../deploy/lib/its_bcoreUD.lib" /pdbtype:sept /libpath:"../../../../../deploy/lib" /libpath:"../lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:"../../../../deploy/dbg/pdb/its_bcoreD.pdb" /debug /machine:I386 /out:"../../../deploy/bin/its_core10D.dll" /implib:"../../../deploy/lib/its_core10D.lib" /pdbtype:sept /libpath:"../../../deploy/lib" /libpath:"../lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "tp_core10 - Win32 Release Unicode"
# Name "tp_core10 - Win32 Release"
# Name "tp_core10 - Win32 Debug Unicode"
# Name "tp_core10 - Win32 Debug"
# Begin Group "header10"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CCmdLine\CmdLine.h
# End Source File
# Begin Source File

SOURCE=.\common.h
# End Source File
# Begin Source File

SOURCE=.\ConsoleOutput.h
# End Source File
# Begin Source File

SOURCE=.\DynArray.h
# End Source File
# Begin Source File

SOURCE=.\EnumProc.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# End Group
# Begin Group "interface10"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\deploy\inc\its_core.h
# End Source File
# End Group
# Begin Group "VersinoInfo10"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\VersionInfo\FileVersionInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\VersionInfo\WinVersion.cpp
# End Source File
# End Group
# Begin Group "HwInfo10"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DriveInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\GetHwInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\GetMacAddr.cpp
# End Source File
# End Group
# Begin Group "Dbg10"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CallSequence.cpp
# End Source File
# Begin Source File

SOURCE=.\ConsoleOutput.cpp
# End Source File
# Begin Source File

SOURCE=.\DbgInfo10.cpp
# End Source File
# Begin Source File

SOURCE=.\Debugger.cpp
# End Source File
# Begin Source File

SOURCE=.\Log.cpp
# End Source File
# Begin Source File

SOURCE=.\symbol.cpp
# End Source File
# Begin Source File

SOURCE=.\Trace.cpp
# End Source File
# Begin Source File

SOURCE=.\TraceDbg.cpp
# End Source File
# Begin Source File

SOURCE=.\WalkStack.cpp
# End Source File
# Begin Source File

SOURCE=.\WalkStack.h
# End Source File
# End Group
# Begin Group "ProcessHandler10"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CygwinCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\EnumProc.cpp
# End Source File
# End Group
# Begin Group "System10"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Associate.cpp
# End Source File
# Begin Source File

SOURCE=.\Clipboard.cpp
# End Source File
# Begin Source File

SOURCE=.\common.cpp
# End Source File
# Begin Source File

SOURCE=.\Heap.cpp
# End Source File
# Begin Source File

SOURCE=.\Help.cpp
# End Source File
# Begin Source File

SOURCE=.\Init.cpp
# End Source File
# Begin Source File

SOURCE=.\Keyboard.cpp
# End Source File
# Begin Source File

SOURCE=.\MakeDir.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\sys_assert.cpp
# End Source File
# Begin Source File

SOURCE=.\ThisCommon.cpp
# End Source File
# Begin Source File

SOURCE=.\Time.cpp
# End Source File
# Begin Source File

SOURCE=.\Win32ModuleImage.cpp
# End Source File
# End Group
# Begin Group "StrUtls10"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FormattedBytes.cpp
# End Source File
# Begin Source File

SOURCE=.\shwapi.cpp
# End Source File
# Begin Source File

SOURCE=.\String.cpp
# End Source File
# Begin Source File

SOURCE=.\StringUtl.cpp
# End Source File
# End Group
# Begin Group "Unicode10"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\UnicodeConv.cpp
# End Source File
# Begin Source File

SOURCE=.\UnicodeUtf.cpp
# End Source File
# End Group
# Begin Group "File10"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BackupFile.cpp
# End Source File
# Begin Source File

SOURCE=.\FileOperations.cpp
# End Source File
# Begin Source File

SOURCE=.\FilePath10.cpp
# End Source File
# Begin Source File

SOURCE=.\FileTypeOpen.cpp
# End Source File
# Begin Source File

SOURCE=.\Path.cpp
# End Source File
# End Group
# Begin Group "ipc10"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ipc\ipc.cpp
# End Source File
# Begin Source File

SOURCE=.\ipc\XMemmap.cpp
# End Source File
# Begin Source File

SOURCE=.\ipc\XMemmap.h
# End Source File
# Begin Source File

SOURCE=.\ipc\XQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\ipc\XQueue.h
# End Source File
# End Group
# Begin Group "Thread10"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Lock.cpp
# End Source File
# Begin Source File

SOURCE=.\Thread.cpp
# End Source File
# End Group
# Begin Group "Codec10"

# PROP Default_Filter ""
# End Group
# Begin Group "Exit10"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\exception_exit.cpp
# End Source File
# End Group
# Begin Group "CmdRedirector10"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CmdLine.cpp
# End Source File
# Begin Source File

SOURCE=.\CmdLineA.cpp
# End Source File
# End Group
# Begin Group "ui10"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MessageBox.cpp
# End Source File
# Begin Source File

SOURCE=.\TransparentMode.cpp
# End Source File
# End Group
# Begin Group "resource10"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core10.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# End Source File
# End Target
# End Project
