# Microsoft Developer Studio Project File - Name="core3220" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=core3220 - Win32 Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "core3220.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "core3220.mak" CFG="core3220 - Win32 Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "core3220 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "core3220 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "core3220 - Win32 Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "core3220 - Win32 Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "core3220 - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Obj\Release"
# PROP BASE Intermediate_Dir "Obj\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Obj\Release"
# PROP Intermediate_Dir "Obj\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../deploy/inc" /I "../inc" /I "../core/inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "HAVE_CORE3220_SRC" /D "HAVE_EXPORT_DLL_I" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"..\..\deploy\bin\core3220.dll" /implib:"..\..\deploy\lib\core3220.lib" /libpath:"../lib" /libpath:"./lib" /libpath:"..\..\deploy\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "core3220 - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Obj\Debug"
# PROP BASE Intermediate_Dir "Obj\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Obj\Debug"
# PROP Intermediate_Dir "Obj\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../deploy/inc" /I "../inc" /I "../core/inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "HAVE_CORE3220_SRC" /D "HAVE_EXPORT_DLL_I" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\..\deploy\bin\core3220D.dll" /implib:"..\..\deploy\lib\core3220D.lib" /pdbtype:sept /libpath:"../lib" /libpath:"./lib" /libpath:"..\..\deploy\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "core3220 - Win32 Release Unicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "core3220___Win32_Release_Unicode"
# PROP BASE Intermediate_Dir "core3220___Win32_Release_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "core3220___Win32_Release_Unicode"
# PROP Intermediate_Dir "core3220___Win32_Release_Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "../../deploy/inc" /I "../inc" /I "../core/inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "HAVE_CORE3220_SRC" /D "HAVE_EXPORT_DLL_I" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../deploy/inc" /I "../inc" /I "../core/inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "UNICODE" /D "_UNICODE_" /D "HAVE_CORE3220_SRC" /D "HAVE_EXPORT_DLL_I" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"..\..\deploy\bin\core3220.dll" /implib:"..\..\deploy\lib\core3220.lib" /libpath:"../lib" /libpath:"./lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"..\..\deploy\bin\core3220.dll" /implib:"..\..\deploy\lib\core3220.lib" /libpath:"../lib" /libpath:"./lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "core3220 - Win32 Debug Unicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "core3220___Win32_Debug_Unicode"
# PROP BASE Intermediate_Dir "core3220___Win32_Debug_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "core3220___Win32_Debug_Unicode"
# PROP Intermediate_Dir "core3220___Win32_Debug_Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../deploy/inc" /I "../inc" /I "../core/inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "HAVE_CORE3220_SRC" /D "HAVE_EXPORT_DLL_I" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../deploy/inc" /I "../inc" /I "../core/inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "UNICODE" /D "_UNICODE_" /D "HAVE_CORE3220_SRC" /D "HAVE_EXPORT_DLL_I" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\..\deploy\bin\core3220D.dll" /implib:"..\..\deploy\lib\core3220D.lib" /pdbtype:sept /libpath:"../lib" /libpath:"./lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\..\deploy\bin\core3220UD.dll" /implib:"..\..\deploy\lib\core3220D.lib" /pdbtype:sept /libpath:"../lib" /libpath:"./lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "core3220 - Win32 Release"
# Name "core3220 - Win32 Debug"
# Name "core3220 - Win32 Release Unicode"
# Name "core3220 - Win32 Debug Unicode"
# Begin Group "core21"

# PROP Default_Filter ""
# Begin Group "inc21"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core21\AboutDlg_21.h
# End Source File
# Begin Source File

SOURCE=.\core21\CPath_21.h
# End Source File
# Begin Source File

SOURCE=.\core21\ModuleApiMgr_21.h
# End Source File
# Begin Source File

SOURCE=.\core21\Path_21.h
# End Source File
# Begin Source File

SOURCE=.\core21\resource.h
# End Source File
# Begin Source File

SOURCE=.\core21\StdAfx.h
# End Source File
# End Group
# Begin Group "WorkThread21"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core21\WorkerThread_21.cpp
# End Source File
# End Group
# Begin Group "CommandToken21"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core21\CmdLineParser_21.cpp
# End Source File
# End Group
# Begin Group "DbgService21"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core21\DbgInfo_21.cpp
# End Source File
# End Group
# Begin Group "DeviceMgr21"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core21\DevInfo_21.cpp
# End Source File
# End Group
# Begin Group "FilePathService21"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core21\CFileFinder_21.cpp
# End Source File
# Begin Source File

SOURCE=.\core21\CPath_21.cpp

!IF  "$(CFG)" == "core3220 - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "core3220 - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "core3220 - Win32 Release Unicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "core3220 - Win32 Debug Unicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\core21\FilePath_21.cpp

!IF  "$(CFG)" == "core3220 - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "core3220 - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "core3220 - Win32 Release Unicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "core3220 - Win32 Debug Unicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\core21\Folder_21.cpp
# End Source File
# Begin Source File

SOURCE=.\core21\Path_21.cpp

!IF  "$(CFG)" == "core3220 - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "core3220 - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "core3220 - Win32 Release Unicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "core3220 - Win32 Debug Unicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Utl21"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core21\CCrypt_21.cpp
# End Source File
# Begin Source File

SOURCE=.\core21\ResString_21.cpp
# End Source File
# End Group
# Begin Group "UiDlgService21"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core21\AboutDlg_21.cpp

!IF  "$(CFG)" == "core3220 - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "core3220 - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "core3220 - Win32 Release Unicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "core3220 - Win32 Debug Unicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\core21\MsgBox_21.cpp
# End Source File
# End Group
# Begin Group "Newwork21"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core20\ping21.cpp
# End Source File
# Begin Source File

SOURCE=.\core21\ping_21.cpp

!IF  "$(CFG)" == "core3220 - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "core3220 - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "core3220 - Win32 Release Unicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "core3220 - Win32 Debug Unicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "SystemService21"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core21\CModule_21.cpp
# End Source File
# Begin Source File

SOURCE=.\core21\core20.cpp
# End Source File
# Begin Source File

SOURCE=.\core21\ModuleApiMgr_21.cpp

!IF  "$(CFG)" == "core3220 - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "core3220 - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "core3220 - Win32 Release Unicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "core3220 - Win32 Debug Unicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "HookFunction21"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core21\HookImportFunction.cpp
# End Source File
# Begin Source File

SOURCE=.\core21\ViewExportFunction_21.cpp
# End Source File
# End Group
# Begin Group "SystemInfo_21"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core21\SystemInfo_21.cpp
# End Source File
# End Group
# Begin Group "cpdh"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core21\cpdh\cpdh.cpp
# End Source File
# Begin Source File

SOURCE=.\core21\cpdh\cpdh.h
# End Source File
# Begin Source File

SOURCE=.\core21\cpdh\cpdhimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\core21\cpdh\cpdhimpl.h
# End Source File
# End Group
# Begin Group "Serial"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core21\serial_21.h
# End Source File
# Begin Source File

SOURCE=.\core21\serial_base_21.cpp
# End Source File
# End Group
# Begin Group "Monitor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core21\MonitorThread_21.cpp
# End Source File
# End Group
# End Group
# Begin Group "core20"

# PROP Default_Filter ""
# Begin Group "inc20"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core20\Base64Coder.h
# End Source File
# Begin Source File

SOURCE=.\core20\common_20.h
# End Source File
# Begin Source File

SOURCE=.\core20\ConsoleOutput_20.h
# End Source File
# Begin Source File

SOURCE=.\core20\DynArray_20.h
# End Source File
# Begin Source File

SOURCE=.\core20\EnumProc_20.h
# End Source File
# Begin Source File

SOURCE=.\core20\resource.h
# End Source File
# Begin Source File

SOURCE=.\core20\stdafx.h
# End Source File
# End Group
# Begin Group "DevcieMgr20"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core20\AdapterIpV4V6Info_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\DriveInfo_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\GetHwInfo_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\GetMacAddr_20.cpp
# End Source File
# End Group
# Begin Group "FileInfoService20"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core20\association_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\BackupFile_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\FileOperations_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\FilePath_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\FileTypeOpen_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\MakeDir_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\Path_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\Shortcut_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core21\StdioFileEx.cpp
# End Source File
# End Group
# Begin Group "DgbServcie20"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core20\CallSequence_20.cpp

!IF  "$(CFG)" == "core3220 - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "core3220 - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "core3220 - Win32 Release Unicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "core3220 - Win32 Debug Unicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\core20\ConsoleAdapter_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\ConsoleOutput_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\DbgInfo_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\DbgThreadTime_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\DebugDir_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\Debugger_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\exception_exit_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\Log_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\Trace_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\TraceDbg_20.cpp
# End Source File
# End Group
# Begin Group "SysService20"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core20\Base64Coder.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\cipher_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\Clipboard_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\CygwinCheck_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\EnumProc_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\Heap_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\Help_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\Keyboard_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\Lock_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\ProcessInfo_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\shwapi_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\symbol_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\sys_assert_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\ThisCommon_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\Thread_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\Time_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\TransparentMode_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\Win32ModuleImage_20.cpp
# End Source File
# End Group
# Begin Group "DlgBoxService"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core20\MessageBox_20.cpp
# End Source File
# End Group
# Begin Group "Utl20"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core20\FormattedBytes_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\String_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\StringUtl_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\UnicodeConv_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\UnicodeUtf_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\Utl_20.cpp
# End Source File
# End Group
# Begin Group "Queue20"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core20\ipc\ipc.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\ipc\XMemmap.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\ipc\XMemmap.h
# End Source File
# Begin Source File

SOURCE=.\core20\ipc\XQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\ipc\XQueue.h
# End Source File
# End Group
# Begin Group "CmdLine20"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core20\CmdLine_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\CmdLineA_20.cpp
# End Source File
# End Group
# Begin Group "Common20"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core20\common_20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\Init_20.cpp
# End Source File
# End Group
# Begin Group "VerInfo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core20\VersionInfo\FileVersionInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\VersionInfo\WinVersion.cpp
# End Source File
# End Group
# Begin Group "dbgcrash"

# PROP Default_Filter ""
# End Group
# Begin Group "xml"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core21\xmlapi_21.cpp
# End Source File
# End Group
# Begin Group "TcpApi"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core20\TcpClient.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\TcpClient.h
# End Source File
# Begin Source File

SOURCE=.\core20\TcpServer.cpp
# End Source File
# Begin Source File

SOURCE=.\core20\TcpServer.h
# End Source File
# End Group
# End Group
# Begin Group "interface"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\deploy\inc\core_i.h
# End Source File
# Begin Source File

SOURCE=..\..\deploy\inc\errno_i.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core3220.cpp
# End Source File
# Begin Source File

SOURCE=.\core3220.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# End Target
# End Project
