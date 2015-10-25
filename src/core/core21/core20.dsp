# Microsoft Developer Studio Project File - Name="tp_core20" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=tp_core20 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "core20.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "core20.mak" CFG="tp_core20 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tp_core20 - Win32 Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tp_core20 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tp_core20 - Win32 Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tp_core20 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tp_core20 - Win32 Release Unicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_Unicode"
# PROP BASE Intermediate_Dir "Release_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_Unicode"
# PROP Intermediate_Dir "Release_Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "../../../../deploy/inc" /I "../inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "HAVE_NO_ITS_ACORE_LIB" /D "ITS_HAVE_EXPORT_DLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp4 /MD /W3 /GX /O2 /I "../../../../deploy/inc" /I "../inc" /D "ITS_HAVE_NO_NEW" /D "HAVE_NO_DELIVERY_VER" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "UNICODE" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "HAVE_NO_ITS_ACORE_LIB" /D "ITS_HAVE_EXPORT_DLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /pdb:"../../../../deploy/dbg/pdb/its_acore.pdb" /machine:I386 /out:"../../../../deploy/bin/its_acore.dll" /implib:"../../../../deploy/lib/its_acore.lib" /libpath:"../../../../deploy/lib" /libpath:"../lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /subsystem:windows /dll /pdb:"../../../../deploy/dbg/pdb/its_acoreU.pdb" /machine:I386 /out:"../../../../deploy/bin/its_acoreU.dll" /implib:"../../../../deploy/lib/its_acoreU.lib" /libpath:"../../../../deploy/lib" /libpath:"../lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "tp_core20 - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp4 /MD /W3 /GX /O2 /I "../../../../deploy/inc" /I "../inc" /D "ITS_HAVE_NO_NEW" /D "HAVE_NO_DELIVERY_VER" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "HAVE_NO_ITS_ACORE_LIB" /D "ITS_HAVE_EXPORT_DLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /pdb:"../../../deploy/dbg/pdb/its_core20.pdb" /machine:I386 /out:"../../../deploy/bin/its_core20.dll" /implib:"../../../deploy/lib/its_core20.lib" /libpath:"../../../deploy/lib" /libpath:"../lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "tp_core20 - Win32 Debug Unicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Unicode_Debug"
# PROP BASE Intermediate_Dir "Unicode_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_Unicode"
# PROP Intermediate_Dir "Debug_Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../../deploy/inc" /I "../inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "HAVE_NO_ITS_ACORE_LIB" /D "ITS_HAVE_EXPORT_DLL" /Yu"stdafx.h" /FD /GZ /GZ /c
# ADD CPP /nologo /Zp4 /MDd /W3 /GX /Od /I "../../../../deploy/inc" /I "../inc" /D "ITS_HAVE_NO_NEW" /D "HAVE_NO_DELIVERY_VER" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "UNICODE" /D "_UNICODE" /D "HAVE_NO_ITS_ACORE_LIB" /D "ITS_HAVE_EXPORT_DLL" /Yu"stdafx.h" /FD /GZ /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /pdb:"../../../../deploy/dbg/pdb/its_acoreD.pdb" /debug /machine:I386 /out:"../../../../deploy/bin/its_acoreD.dll" /implib:"./../../../deploy/lib/its_acoreD.lib" /pdbtype:sept /libpath:"../../../../deploy/lib" /libpath:"../lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /subsystem:windows /dll /pdb:"../../../../deploy/dbg/pdb/its_acoreUD.pdb" /machine:I386 /out:"../../../../deploy/bin/its_acoreUD.dll" /implib:"../../../../deploy/lib/its_acoreUD.lib" /pdbtype:sept /libpath:"../../../../deploy/lib" /libpath:"../lib"
# SUBTRACT LINK32 /pdb:none /debug

!ELSEIF  "$(CFG)" == "tp_core20 - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../../deploy/inc" /I "../inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "UNICODE" /D "_UNICODE" /D "HAVE_NO_ITS_ACORE_LIB" /D "ITS_HAVE_EXPORT_DLL" /Yu"stdafx.h" /FD /GZ /GZ /c
# ADD CPP /nologo /Zp4 /MDd /W3 /GX /ZI /Od /I "../../../deploy/inc" /I "../inc" /D "HAVE_NO_DELIVERY_VER" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "HAVE_NO_ITS_CORE_LIB" /D "ITS_HAVE_EXPORT_DLL" /D "ITS_HAVE_CORE10_API" /D "ITS_HAVE_CORE20_API" /FR /Yu"stdafx.h" /FD /GZ /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /pdb:"../../../../deploy/dbg/pdb/its_acoreUD.pdb" /debug /machine:I386 /out:"../../../../deploy/bin/its_acoreUD.dll" /implib:"../../../../deploy/lib/its_acoreUD.lib" /pdbtype:sept /libpath:"../../../../deploy/lib" /libpath:"../lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 core10D.lib /nologo /subsystem:windows /dll /pdb:"../../../deploy/dbg/pdb/its_acoreD.pdb" /debug /machine:I386 /out:"../../../deploy/bin/its_core20D.dll" /implib:"../../../deploy/lib/its_acoreD.lib" /pdbtype:sept /libpath:"../../../deploy/lib" /libpath:"../lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "tp_core20 - Win32 Release Unicode"
# Name "tp_core20 - Win32 Release"
# Name "tp_core20 - Win32 Debug Unicode"
# Name "tp_core20 - Win32 Debug"
# Begin Group "main20"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\core20.cpp
# End Source File
# Begin Source File

SOURCE=.\core20.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "header20"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CPath.h
# End Source File
# Begin Source File

SOURCE=.\ItsAboutDlg.h
# End Source File
# Begin Source File

SOURCE=.\ModuleApiMgr.h
# End Source File
# Begin Source File

SOURCE=.\Path.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "resource20"

# PROP Default_Filter ""
# End Group
# Begin Group "interface20"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\deploy\inc\its_core.h
# End Source File
# End Group
# Begin Group "WorkerThread20"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WorkerThread.cpp
# End Source File
# End Group
# Begin Group "CmdLineParser20"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CmdLineParser.cpp
# End Source File
# End Group
# Begin Group "Dev20"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DevInfo.cpp
# End Source File
# End Group
# Begin Group "Network20"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ping.cpp
# End Source File
# End Group
# Begin Group "File20"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CPath.cpp
# End Source File
# Begin Source File

SOURCE=.\FilePath.cpp
# End Source File
# Begin Source File

SOURCE=.\Folder.cpp

!IF  "$(CFG)" == "tp_core20 - Win32 Release Unicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tp_core20 - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tp_core20 - Win32 Debug Unicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tp_core20 - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "ui20"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ItsAboutDlg.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\MsgBox.cpp
# End Source File
# End Group
# Begin Group "sys20"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CModule.cpp
# End Source File
# Begin Source File

SOURCE=.\ResString.cpp
# End Source File
# Begin Source File

SOURCE=.\Utl.cpp
# End Source File
# End Group
# Begin Group "dbg20"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DbgInfo.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
