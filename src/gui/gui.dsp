# Microsoft Developer Studio Project File - Name="gui3220" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=gui3220 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gui.mak" CFG="gui3220 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gui3220 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gui3220 - Win32 Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gui3220 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gui3220 - Win32 Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gui3220 - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Obj\Release"
# PROP Intermediate_Dir "Obj\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp4 /MD /W3 /GX /O2 /I "../../deploy/inc" /I "../inc" /I "./" /I "scintilla176\include\\" /I "scintilla176\src\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "HAVE_EXPORT_DLL_I" /D "HAVE_GUI3220_SRC" /D "STATIC_BUILD" /D "SCI_LEXER" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 uuid.lib imm32.lib comctl32.lib /nologo /subsystem:windows /dll /pdb:"../../deploy/bin/debug/gui3220.pdb" /machine:I386 /out:"../../deploy/bin/gui3220.dll" /implib:"../../deploy/lib/gui3220.lib" /libpath:"../../deploy/lib" /libpath:"../lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "gui3220 - Win32 Debug Unicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_Unicode"
# PROP BASE Intermediate_Dir "Debug_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Obj\Debug_Unicode"
# PROP Intermediate_Dir "Obj\Debug_Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../include" /I "../../../deploy/include" /I "../../deploy/inc" /I "../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "HAVE_EXPORT_DLL_I" /D "HAVE_gui3220_EXPORT_DLL" /D "UNICODE" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /GZ /GZ /c
# ADD CPP /nologo /Zp4 /MDd /W3 /GX /Od /I "../../deploy/inc" /I "../inc" /I "./" /I "scintilla176\include\\" /I "scintilla176\src\\" /D "HAVE_NO_DELIVERY_VER" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "HAVE_EXPORT_DLL_I" /D "HAVE_GUI3220_SRC" /D "UNICODE" /D "_UNICODE" /D "STATIC_BUILD" /D "SCI_LEXER" /Yu"stdafx.h" /FD /GZ /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"../lib" /libpath:"../../deploy/lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 uuid.lib imm32.lib comctl32.lib /nologo /subsystem:windows /dll /pdb:"../../deploy/debug/gui3220UD.pdb" /machine:I386 /out:"../../deploy/bin/gui3220UD.dll" /implib:"../../deploy/lib/gui3220UD.lib" /pdbtype:sept /libpath:"../../deploy/lib" /libpath:"../lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "gui3220 - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Obj\Debug"
# PROP Intermediate_Dir "Obj\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../deploy/inc" /I "../inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "HAVE_EXPORT_DLL_I" /D "HAVE_gui3220_EXPORT_DLL" /D "UNICODE" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /GZ /GZ /c
# ADD CPP /nologo /Zp4 /MDd /W3 /GX /ZI /Od /I "../../deploy/inc" /I "../inc" /I "./" /I "scintilla176\include\\" /I "scintilla176\src\\" /D "HAVE_NO_DELIVERY_VER" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "HAVE_EXPORT_DLL_I" /D "HAVE_GUI3220_SRC" /D "STATIC_BUILD" /D "SCI_LEXER" /FR /Yu"stdafx.h" /FD /GZ /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"../../../deploy/lib" /libpath:"../lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 uuid.lib imm32.lib comctl32.lib /nologo /subsystem:windows /dll /pdb:"../../deploy/debug/gui3220D.pdb" /debug /machine:I386 /out:"../../deploy/bin/gui3220D.dll" /implib:"../../deploy/lib/gui3220D.lib" /pdbtype:sept /libpath:"../../deploy/lib" /libpath:"../lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "gui3220 - Win32 Release Unicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_Unicode"
# PROP BASE Intermediate_Dir "Release_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Obj\Release_Unicode"
# PROP Intermediate_Dir "Obj\Release_Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "../../../deploy/inc" /I "../inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "HAVE_GUI3220_SRC" /D "HAVE_gui3220_EXPORT_DLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp4 /MD /W3 /GX /O2 /I "../../deploy/inc" /I "../inc" /I "./" /I "scintilla176\include\\" /I "scintilla176\src\\" /D "HAVE_NO_DELIVERY_VER" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "UNICODE" /D "_UNICODE" /D "HAVE_EXPORT_DLL_I" /D "HAVE_GUI3220_SRC" /D "STATIC_BUILD" /D "SCI_LEXER" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386 /libpath:"../../../deploy/lib" /libpath:"../lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 uuid.lib imm32.lib comctl32.lib /nologo /subsystem:windows /dll /pdb:"../../deploy/debug/gui3220U.pdb" /machine:I386 /out:"../../deploy/bin/gui3220U.dll" /implib:"../../deploy/lib/gui3220U.lib" /libpath:"../../deploy/lib" /libpath:"../lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "gui3220 - Win32 Release"
# Name "gui3220 - Win32 Debug Unicode"
# Name "gui3220 - Win32 Debug"
# Name "gui3220 - Win32 Release Unicode"
# Begin Group "aui"

# PROP Default_Filter ""
# Begin Group "SciLexer"

# PROP Default_Filter ""
# Begin Group "src"

# PROP Default_Filter ""
# Begin Group "lex"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\scintilla176\src\LexAbaqus.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexAda.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexAPDL.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexAsm.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexAsn1.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexASY.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexAU3.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexAVE.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexBaan.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexBash.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexBasic.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexBullant.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexCaml.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexCLW.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexCmake.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexConf.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexCPP.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexCrontab.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexCsound.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexCSS.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexD.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexEiffel.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexErlang.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexEScript.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexFlagship.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexForth.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexFortran.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexGAP.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexGui4Cli.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexHaskell.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexHTML.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexInno.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexKix.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexLisp.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexLout.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexLua.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexMagik.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexMatlab.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexMetapost.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexMMIXAL.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexMPT.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexMSSQL.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexNsis.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexOpal.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexOthers.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexPascal.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexPB.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexPerl.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexPLM.cxx

!IF  "$(CFG)" == "gui3220 - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "gui3220 - Win32 Debug Unicode"

!ELSEIF  "$(CFG)" == "gui3220 - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "gui3220 - Win32 Release Unicode"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexPOV.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexPowerShell.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexProgress.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexPS.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexPython.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexR.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexRebol.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexRuby.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexScriptol.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexSmalltalk.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexSpecman.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexSpice.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexSQL.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexTADS3.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexTCL.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexTeX.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexVB.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexVerilog.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexVHDL.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LexYAML.cxx
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Source File

SOURCE=.\scintilla176\src\AutoComplete.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\CallTip.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\CellBuffer.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\CharClassify.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\ContractionState.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\Decoration.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\Document.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\DocumentAccessor.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\Editor.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\ExternalLexer.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\Indicator.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\KeyMap.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\KeyWords.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\LineMarker.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\win32\PlatWin.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\PositionCache.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\PropSet.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\RESearch.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\RunStyles.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\ScintillaBase.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\win32\ScintillaWin.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\win32\ScintRes.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\Style.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\StyleContext.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\UniConversion.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\ViewStyle.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\WindowAccessor.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\XPM.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "inc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\scintilla176\include\Platform.h
# End Source File
# End Group
# Begin Group "queue"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\scintilla176\src\cmd_queue.cpp

!IF  "$(CFG)" == "gui3220 - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "gui3220 - Win32 Debug Unicode"

!ELSEIF  "$(CFG)" == "gui3220 - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "gui3220 - Win32 Release Unicode"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\scintilla176\src\cmd_queue.h
# End Source File
# End Group
# End Group
# Begin Group "CEditCtl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\clc\ClWnd.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\common.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "CCmdCtl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\clc\clc_api.cpp
# End Source File
# End Group
# End Group
# Begin Group "Interface"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\deploy\inc\gui_i.h
# End Source File
# End Group
# Begin Group "sgc"

# PROP Default_Filter ""
# End Group
# Begin Group "pfw"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pfw\CPluginApp.cpp
# End Source File
# Begin Source File

SOURCE=.\pfw\CPluginEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\pfw\CPluginManager.cpp
# End Source File
# Begin Source File

SOURCE=.\pfw\DllMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\pfw\DllMgr.h
# End Source File
# End Group
# Begin Group "resource"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Bmicon0.xpm
# End Source File
# Begin Source File

SOURCE=.\Bmicon1.xpm
# End Source File
# Begin Source File

SOURCE=.\Bmicon2.xpm
# End Source File
# Begin Source File

SOURCE=.\Bmicon3.xpm
# End Source File
# Begin Source File

SOURCE=.\Bmicon4.xpm
# End Source File
# Begin Source File

SOURCE=.\Bmicon5.xpm
# End Source File
# Begin Source File

SOURCE=.\Bmicon6.xpm
# End Source File
# Begin Source File

SOURCE=.\Bmicon7.xpm
# End Source File
# Begin Source File

SOURCE=.\Bmicon8.xpm
# End Source File
# Begin Source File

SOURCE=.\scintilla176\win32\Margin.cur
# End Source File
# End Group
# Begin Group "main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\gui.rc
# End Source File
# Begin Source File

SOURCE=.\Main.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "ctrl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ctl\messagebox.cpp
# End Source File
# Begin Source File

SOURCE=.\ctl\utls.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
