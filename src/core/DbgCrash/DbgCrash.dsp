# Microsoft Developer Studio Project File - Name="dbgcrash3220" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=dbgcrash3220 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DbgCrash.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DbgCrash.mak" CFG="dbgcrash3220 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dbgcrash3220 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dbgcrash3220 - Win32 Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dbgcrash3220 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dbgcrash3220 - Win32 Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dbgcrash3220 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Obj\Release"
# PROP Intermediate_Dir "Obj\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ITS_DBGEXP_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp4 /MT /W3 /GX /O2 /I "../../../../deploy/inc" /I "../inc" /D "HAVE_NO_DELIVERY_VER" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ITS_DBGEXP_EXPORTS" /D "HAVE_NO_CPP_APIS" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /def:".\dbgexp.def" /out:"../../../deploy/bin/dbgcrash3220.dll" /libpath:"../../../deploy/lib" /libpath:"../lib" /libpath:"../../lib"

!ELSEIF  "$(CFG)" == "dbgcrash3220 - Win32 Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Unicode Debug"
# PROP BASE Intermediate_Dir "Unicode Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Obj\Debug_Unicode"
# PROP Intermediate_Dir "Obj\Debug_Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ITS_DBGEXP_EXPORTS" /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /Zp4 /MTd /W3 /GX /Od /I "../../../../deploy/inc" /I "../inc" /D "HAVE_NO_DELIVERY_VER" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ITS_DBGEXP_EXPORTS" /D "_UNICODE" /D "UNICODE" /D "HAVE_NO_CPP_APIS" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../../../../output/bin/its_dbgexp.dll" /pdbtype:sept
# ADD LINK32 Kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:"../../../../deploy/dbg/pdb/its_dbgexpUD.pdb" /machine:I386 /out:"../../../deploy/bin/dbgcrash3220UD.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none /debug

!ELSEIF  "$(CFG)" == "dbgcrash3220 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "dbgcrash3220___Win32_Debug"
# PROP BASE Intermediate_Dir "dbgcrash3220___Win32_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Obj\Debug"
# PROP Intermediate_Dir "Obj\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../../deploy/inc" /I "../inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ITS_DBGEXP_EXPORTS" /D "_UNICODE" /D "UNICODE" /D "HAVE_NO_CPP_APIS" /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /Zp4 /MTd /W3 /GX /ZI /Od /I "../../../../deploy/inc" /I "../inc" /D "HAVE_NO_DELIVERY_VER" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ITS_DBGEXP_EXPORTS" /D "HAVE_NO_CPP_APIS" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:"../../../../deploy/dbg/pdb/its_dbgexpUD.pdb" /debug /machine:I386 /out:"../../../../deploy/bin/its_dbgexpUD.dll" /pdbtype:sept /libpath:"../../../../deploy/lib" /libpath:"../lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 Kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../../../deploy/bin/dbgcrash3220D.dll" /pdbtype:sept /libpath:"../../../deploy/lib" /libpath:"../lib"

!ELSEIF  "$(CFG)" == "dbgcrash3220 - Win32 Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "dbgcrash3220___Win32_Release_Unicode"
# PROP BASE Intermediate_Dir "dbgcrash3220___Win32_Release_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Obj\Release_Unicode"
# PROP Intermediate_Dir "Obj\Release_Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /O2 /I "../../../../deploy/inc" /I "../inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ITS_DBGEXP_EXPORTS" /D "HAVE_NO_CPP_APIS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp4 /MT /W3 /GX /O2 /I "../../../../deploy/inc" /I "../inc" /D "HAVE_NO_DELIVERY_VER" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ITS_DBGEXP_EXPORTS" /D "HAVE_NO_CPP_APIS" /D "UNICODE" /D "_UNICODE" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:"./../../../deploy/bin/its_dbgexp.pdb" /machine:I386 /out:"../../../../deploy/bin/its_dbgexp.dll" /libpath:"../../../../deploy/lib" /libpath:"../lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../../../deploy/bin/dbgcrash3220U.dll"

!ENDIF 

# Begin Target

# Name "dbgcrash3220 - Win32 Release"
# Name "dbgcrash3220 - Win32 Debug Unicode"
# Name "dbgcrash3220 - Win32 Debug"
# Name "dbgcrash3220 - Win32 Release Unicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\dbgexp.def

!IF  "$(CFG)" == "dbgcrash3220 - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "dbgcrash3220 - Win32 Debug Unicode"

!ELSEIF  "$(CFG)" == "dbgcrash3220 - Win32 Debug"

!ELSEIF  "$(CFG)" == "dbgcrash3220 - Win32 Release Unicode"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc_CrashDumpFile.cpp
# End Source File
# Begin Source File

SOURCE=.\dc_CrashDumpFile.h
# End Source File
# Begin Source File

SOURCE=.\dc_CrashLog.cpp
# End Source File
# Begin Source File

SOURCE=.\dc_CrashLog.h
# End Source File
# Begin Source File

SOURCE=.\dc_DbgExp.cpp
# End Source File
# Begin Source File

SOURCE=.\dc_DbgExp.h
# End Source File
# Begin Source File

SOURCE=.\dc_GetVersion.cpp
# End Source File
# Begin Source File

SOURCE=.\dc_LogFile.cpp
# End Source File
# Begin Source File

SOURCE=.\dc_LogFile.h
# End Source File
# Begin Source File

SOURCE=.\dc_util.cpp
# End Source File
# Begin Source File

SOURCE=.\dc_utl.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\core20\WalkStack_20.cpp
# End Source File
# Begin Source File

SOURCE=..\core20\WalkStack_20.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CrashLog.h
# End Source File
# Begin Source File

SOURCE=.\CrashMiniDump.h
# End Source File
# Begin Source File

SOURCE=.\DbgExp.h
# End Source File
# Begin Source File

SOURCE=.\LogFile.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\utl.h
# End Source File
# Begin Source File

SOURCE=.\walkstatck.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=..\lib\Kernel32.Lib
# End Source File
# End Target
# End Project
