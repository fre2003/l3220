# Microsoft Developer Studio Project File - Name="tp_core10S" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=tp_core10S - Win32 Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "core10S.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "core10S.mak" CFG="tp_core10S - Win32 Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tp_core10S - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "tp_core10S - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "tp_core10S - Win32 Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "tp_core10S - Win32 Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tp_core10S - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../../../deploy/inc" /I "../inc" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "ITS_HAVE_NO_MFC" /D "HAVE_NO_ITS_CORE_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/core10.lib"

!ELSEIF  "$(CFG)" == "tp_core10S - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../../deploy/inc" /I "../inc" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "HAVE_NO_ITS_CORE_LIB" /D "ITS_HAVE_STATIC_LIB" /D "ITS_HAVE_NO_MFC" /D "ITS_HAVE_CORE10_API" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/core10D.lib"

!ELSEIF  "$(CFG)" == "tp_core10S - Win32 Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_Unicode"
# PROP BASE Intermediate_Dir "Debug_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_Unicode"
# PROP Intermediate_Dir "Debug_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /Gm /GX /ZI /Od /I "../../../../deploy/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "RTS_HAVE_STATIC_LIB" /D "HAVE_NO_WIN32SHL_LIB" /FR /YX /FD /GZ /c
# ADD CPP /nologo /W4 /Gm /GX /ZI /Od /I "../../../../deploy/include" /I "../../../deploy/inc" /I "../inc" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /D "_UNICODE" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../lib/core10D.lib"
# ADD LIB32 /nologo /out:"../lib/core10D.lib"

!ELSEIF  "$(CFG)" == "tp_core10S - Win32 Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_Unicode"
# PROP BASE Intermediate_Dir "Release_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_Unicode"
# PROP Intermediate_Dir "Release_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../../../deploy/inc" /I "../inc" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /D "_UNICODE" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../lib/core10.lib"
# ADD LIB32 /nologo /out:"../lib/core10.lib"

!ENDIF 

# Begin Target

# Name "tp_core10S - Win32 Release"
# Name "tp_core10S - Win32 Debug"
# Name "tp_core10S - Win32 Debug Unicode"
# Name "tp_core10S - Win32 Release Unicode"
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

SOURCE=.\DbgInfo.cpp
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

SOURCE=.\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
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

SOURCE=.\Utl.cpp
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

!IF  "$(CFG)" == "tp_core10S - Win32 Release"

!ELSEIF  "$(CFG)" == "tp_core10S - Win32 Debug"

!ELSEIF  "$(CFG)" == "tp_core10S - Win32 Debug Unicode"

!ELSEIF  "$(CFG)" == "tp_core10S - Win32 Release Unicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\String.cpp

!IF  "$(CFG)" == "tp_core10S - Win32 Release"

!ELSEIF  "$(CFG)" == "tp_core10S - Win32 Debug"

!ELSEIF  "$(CFG)" == "tp_core10S - Win32 Debug Unicode"

!ELSEIF  "$(CFG)" == "tp_core10S - Win32 Release Unicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

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

!IF  "$(CFG)" == "tp_core10S - Win32 Release"

!ELSEIF  "$(CFG)" == "tp_core10S - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tp_core10S - Win32 Debug Unicode"

!ELSEIF  "$(CFG)" == "tp_core10S - Win32 Release Unicode"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
