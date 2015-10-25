# Microsoft Developer Studio Project File - Name="lib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=lib - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lib.mak" CFG="lib - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lib - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "lib - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE "lib - Win32 Unicode Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "lib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f lib.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "lib.exe"
# PROP BASE Bsc_Name "lib.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "combine.bat"
# PROP Rebuild_Opt "/a"
# PROP Target_File "lib.exe"
# PROP Bsc_Name "lib.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "lib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f lib.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "lib.exe"
# PROP BASE Bsc_Name "lib.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "combine.bat ../../../../delivery/lib/MFCUtl.lib  consoleoutput.lib"
# PROP Rebuild_Opt ""
# PROP Target_File "../../../../delivery/lib/MFCUtl.lib"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "lib - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "lib___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "lib___Win32_Unicode_Debug"
# PROP BASE Cmd_Line "combine.bat"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "../../../../delivery/lib/MFCUtl.lib"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "lib___Win32_Unicode_Debug"
# PROP Intermediate_Dir "lib___Win32_Unicode_Debug"
# PROP Cmd_Line "combine.bat  ..\..\..\..\delivery\lib\MFCUtlU.lib consoleoutputU.lib"
# PROP Rebuild_Opt "/a"
# PROP Target_File "..\..\..\..\delivery\lib\MFCUtlU.lib"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "lib - Win32 Release"
# Name "lib - Win32 Debug"
# Name "lib - Win32 Unicode Debug"

!IF  "$(CFG)" == "lib - Win32 Release"

!ELSEIF  "$(CFG)" == "lib - Win32 Debug"

!ELSEIF  "$(CFG)" == "lib - Win32 Unicode Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\combine.bat

!IF  "$(CFG)" == "lib - Win32 Release"

!ELSEIF  "$(CFG)" == "lib - Win32 Debug"

!ELSEIF  "$(CFG)" == "lib - Win32 Unicode Debug"

# PROP Intermediate_Dir "Unicode_Debug"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
