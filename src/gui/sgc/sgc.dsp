# Microsoft Developer Studio Project File - Name="tp_sgc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=tp_sgc - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sgc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sgc.mak" CFG="tp_sgc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tp_sgc - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tp_sgc - Win32 Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tp_sgc - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tp_sgc - Win32 Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tp_sgc - Win32 Release"

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
# ADD CPP /nologo /Zp4 /MD /W3 /GX /O2 /I "../../../deploy/inc" /I "../inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "HAVE_NO_ITS_PIENG_LIB" /D "ITS_HAVE_EXPORT_DLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /pdb:"../../../deploy/dbg/pdb/its_pieng.pdb" /machine:I386 /out:"../../../deploy/bin/its_pieng.dll" /libpath:"../../../deploy/lib" /libpath:"../lib" /implib:../../../deploy/lib/its_pieng.lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "tp_sgc - Win32 Debug Unicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_Unicode"
# PROP BASE Intermediate_Dir "Debug_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_Unicode"
# PROP Intermediate_Dir "Debug_Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../include" /I "../../../deploy/include" /I "../../deploy/inc" /I "../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "HAVE_NO_ITS_PIENG_LIB" /D "RTS_HAVE_EXPORT_DLL" /D "UNICODE" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /GZ /GZ /c
# ADD CPP /nologo /Zp4 /MDd /W3 /GX /Od /I "../../../deploy/inc" /I "../inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "HAVE_NO_ITS_PIENG_LIB" /D "ITS_HAVE_EXPORT_DLL" /D "UNICODE" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /GZ /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /pdb:"../../dbg/pdb/its_piengUD.pdb" /debug /machine:I386 /out:"../../../deploy/bin/its_piengUD.dll" /implib:"../../deploy/lib/its_piengUD.lib" /pdbtype:sept /libpath:"../lib" /libpath:"../../deploy/lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /subsystem:windows /dll /pdb:"../../../deploy/dbg/pdb/its_piengUD.pdb" /machine:I386 /out:"../../../deploy/bin/its_piengUD.dll" /implib:"../../../deploy/lib/its_piengUD.lib" /pdbtype:sept /libpath:"../../../deploy/lib" /libpath:"../lib"
# SUBTRACT LINK32 /pdb:none /debug

!ELSEIF  "$(CFG)" == "tp_sgc - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../deploy/inc" /I "../inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "HAVE_NO_ITS_PIENG_LIB" /D "ITS_HAVE_EXPORT_DLL" /D "UNICODE" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /GZ /GZ /c
# ADD CPP /nologo /Zp4 /MDd /W3 /GX /ZI /Od /I "../../../deploy/inc" /I "../inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "HAVE_NO_ITS_PIENG_LIB" /D "ITS_HAVE_EXPORT_DLL" /FR /Yu"stdafx.h" /FD /GZ /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /pdb:"../../../deploy/dbg/pdb/its_piengUD.pdb" /debug /machine:I386 /out:"../../../deploy/bin/its_piengUD.dll" /implib:"../../../deploy/lib/its_piengUD.lib" /pdbtype:sept /libpath:"../../../deploy/lib" /libpath:"../lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /subsystem:windows /dll /pdb:"../../../deploy/dbg/pdb/its_piengD.pdb" /debug /machine:I386 /out:"../../../deploy/bin/its_piengD.dll" /implib:"../../../deploy/lib/its_piengD.lib" /pdbtype:sept /libpath:"../../../deploy/lib" /libpath:"../lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "tp_sgc - Win32 Release Unicode"

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
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "../../../deploy/inc" /I "../inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "HAVE_NO_ITS_PIENG_LIB" /D "ITS_HAVE_EXPORT_DLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp4 /MD /W3 /GX /O2 /I "../../../deploy/inc" /I "../inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "UNICODE" /D "_UNICODE" /D "HAVE_NO_ITS_PIENG_LIB" /D "ITS_HAVE_EXPORT_DLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /pdb:"../../../deploy/dbg/pdb/its_pieng.pdb" /machine:I386 /out:"../../../deploy/bin/its_pieng.dll" /libpath:"../../../deploy/lib" /libpath:"../lib" /implib:../../../deploy/lib/its_pieng.lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /subsystem:windows /dll /pdb:"../../../deploy/dbg/pdb/its_piengU.pdb" /machine:I386 /out:"../../../deploy/bin/its_piengU.dll" /libpath:"../../../deploy/lib" /libpath:"../lib" /implib:../../../deploy/lib/its_piengU.lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "tp_sgc - Win32 Release"
# Name "tp_sgc - Win32 Debug Unicode"
# Name "tp_sgc - Win32 Debug"
# Name "tp_sgc - Win32 Release Unicode"
# Begin Group "Main Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Main.cpp
# End Source File
# Begin Source File

SOURCE=.\pieng.def

!IF  "$(CFG)" == "tp_sgc - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tp_sgc - Win32 Debug Unicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tp_sgc - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tp_sgc - Win32 Release Unicode"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pieng.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DllMgr.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Interface Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\deploy\inc\its_pieng.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DllMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\pieng.cpp
# End Source File
# Begin Source File

SOURCE=.\pientry.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
