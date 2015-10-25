/******************************************************************************/
/*              Copyright (C) 2005, CII Technologies, Inc.                    */
/*                            All Rights Reserved                             */
/******************************************************************************/
/* Filename:    emu_xml.h                                                     */
/* Description:                                                               */
/*           all types or constant definitions.                               */
/*                                                                            */
/* Revision history:                                                          */
/*   Name            Date             Remaks                                  */
/*                                                                            */
/*   Shen Hankun      2003/03/12         version 1.0                          */
/*                    Define all Utility interface.                           */
/******************************************************************************/

#ifndef _EMU_XML_
#define _EMU_XML_

#include <its_types.h>

typedef char* PCHAR;

#undef EXPORT 
#define EXPORT ITSAPI 

START_HEADER

//
// Read attribute value from skin file specified in emulator.xml.
// return 0: success, otherwise, error code. 
// 
//
// <EMULATOR>
//    <WINDOW DISPLAYPOSX="100" DISPLAYPOSY="80" SMALLSHAPE="0" TOPMOST="0" ABOUTINFO="Coolsand Device Emulator Copyright (C) 2002-2007" SUPPORT_ROTATE="1"/>
//    <SKINSERIAL SERIAL="1">
//      <SKIN NAME="C100 Solution" FILE="skin_128x160_25keys_s1.xml"/>
//      <SKIN NAME="C101 Solution" FILE="skin_128x160_27keys_s0.xml"/>
//      <SKIN NAME="C200 Solution" FILE="skin_176x220_27keys_s0.xml"/>
//
// PCSTR pszSkinFile = UTL_EmuXmlGetAttributeTextValue("EMULATOR:SKINSERIAL:SKIN:NAME", FALSE, (LONG*)&idx, 1);
// 
EXPORT DWORD UTL_EmuXmlGetAttributeIntValue(PCSTR pszNodeStream, // input "EMULATOR:SKINSERIAL:SERIAL"
                                            BOOL bSkinFile, // TRUE is skin file, FALSE is 'emulator.xml' file
                                            LONG* plValue, // return value
                                            LONG nOffset); // input offset, 1 indicate first item.

EXPORT DWORD UTL_EmuXmlGetAttributeIntValueEx(PCSTR pszNodeStream,
                                              BOOL bSkinFile,
                                              LONG* plValue,
                                              LONG nSkinFileIndex,
                                              LONG nOffset);

EXPORT DWORD UTL_EmuXmlSetAttributeIntValue(PCSTR pszNodeStream,
                                            BOOL bSkinFile,
                                            LONG lValue);

EXPORT PCSTR UTL_EmuXmlGetAttributeTextValue(PCSTR pszNodeStream, // input 
                                             BOOL bSkinFile, // TRUE is skin file, FALSE is 'emulator.xml' file
                                             LONG nOffset);// input offset, 1 indicate first item.

EXPORT PCSTR UTL_EmuXmlGetLastErrorDesc();
EXPORT LONG UTL_EmuXmlGetLastErrorId();

EXPORT HANDLE UTL_EmuXmlGetFileHandle(LONG nIndex, 
                                      BOOL bSkinFile); // XML_GetAttributeIntValue(hXml, pszNodeStream, &val, nOffset);, XML_FreeFile(hXml); 



EXPORT HANDLE XML_LoadFile(PCSTR pszXmlFile);
EXPORT BOOL XML_SaveFile(HANDLE hXmlFile);
EXPORT BOOL XML_FreeFile(HANDLE hXmlFile);
EXPORT DWORD XML_GetAttributeIntValue(HANDLE hXmlFile, PCSTR pszNodeStream, LONG* plValue,LONG nOffset);
EXPORT PCSTR XML_GetAttributeTextValue(HANDLE hXmlFile, PCSTR pszNodeStream,LONG nOffset);
//EXPORT PCSTR XML_GetElementTextValue(HANDLE hXmlFile, PCSTR pszNodeStream,LONG nOffset);
EXPORT DWORD XML_SetElementTextValue(HANDLE hXmlFile, PCSTR pszNodeStream, PCSTR pszNewElementName);
EXPORT DWORD XML_SetAttributeIntValue(HANDLE hXmlFile, PCSTR pszNodeStream, LONG lAttrValue);
EXPORT DWORD XML_SetAttributeTextValue(HANDLE hXmlFile, PCSTR pszNodeStream, PCSTR pszAttrText);
EXPORT PCSTR XML_GetLastErrorDesc(HANDLE hXmlFile);
EXPORT LONG XML_GetLastErrorId(HANDLE hXmlFile);


EXPORT BOOL UTL_MessageBox(HWND hWnd, PCSTR pszTitle, PCSTR pFormat, ...);
EXPORT DWORD UTL_RunSilentProcess(PCSTR pszCmd, PCSTR pAppCmd, DWORD dwTimeOut);

EXPORT BOOL UTL_Initialization(BOOL bInitEnv);
EXPORT BOOL UTL_Initialization2(VOID);
EXPORT BOOL UTL_IsFileExist(PCSTR pszFileName);
EXPORT PCSTR UTL_GetInstallPath();
EXPORT PSTR UTL_GetValidFilePath(PCSTR pszRelFilePath);
EXPORT BOOL UTL_CopyFiles(HWND hWnd, PCSTR pszFrom, PCSTR pszTo, BOOL bSilent); // support *.* "c:\\tst";"C:\\eside\\etools\\bak\\codefmtbak";
EXPORT BOOL  UTL_DeleteFiles(HWND hWnd,PCSTR pszSource, BOOL bSilent); // support *.*
EXPORT BOOL UTL_IsWin32ImageFile(PCSTR pszFile);
EXPORT DWORD UTL_RunSilentProcess(PCSTR pszCmd, PCSTR pAppCmd, DWORD dwTimeOut);
EXPORT DWORD UTL_RunSilentBatProcess(PCSTR pszCmd, DWORD dwTimeOut);
EXPORT BOOL UTL_OpenMagnify();
EXPORT BOOL UTL_ExcecuteFile(PCSTR pszPath);
EXPORT BOOL UTL_OpenFile(PCSTR pszPath);
EXPORT BOOL UTL_ExploreFolder(PCSTR pszFolderPath);
EXPORT BOOL UTL_OpenChmFile(HWND hWndParent, PCSTR pszChmFile, PCSTR pszCurWordToFind);
EXPORT BOOL UTL_OpenXmlFile(PCSTR pszXmlFile);
EXPORT BOOL UTL_OpenPlainFile(PCSTR pszFile);
EXPORT BOOL UTL_OpenPdfFile(PCSTR pszPdfFile);


END_HEADER

#endif // _H 
