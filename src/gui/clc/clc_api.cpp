//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a Core Service C++ Library.
// Copyright (C) 2002-2008 RISS Ltd.
// All rights reserved.
// 
// This source code or binary libary can be used, distributed or modified
// with or without modification, are permitted including commercial applications.
// -------------- 
//*******************************************************************************
#include "StdAfx.h"
#include "common.h"
#include "Platform.h"
#include "Scintilla.h"
#include "shlwapi.h"

#define STR_SCINTILLAWND _T("Scintilla")

#ifndef HAVE_SUPPORT_UNICODE 
#undef StrChr
#undef lstrlen
#define StrChr  StrChrA 
#define lstrlen lstrlenA 
#endif

CCmdCtl_i::CCmdCtl_i()
{
   m_bLinenumbers = FALSE;
   m_nSearchflags = 0;
   m_bBookMark    = TRUE;
   m_bCtrlcvxzy   = TRUE;
}

CCmdCtl_i::~CCmdCtl_i()
{

}

//  Create the window
bool CCmdCtl_i::Create (LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
  if (!CWnd::CreateEx(WS_EX_CLIENTEDGE, STR_SCINTILLAWND, lpszWindowName, dwStyle, rect, pParentWnd,(UINT)nID))
  {
    return FALSE;
  }
  
  Init();
  
  return TRUE;
}

//
//  LoadXPM() - Load XPM Resource, must be freed by LocalFree()
//
LPSTR LoadXPM(UINT uidRes)
{
  HRSRC hResInfo;
  HGLOBAL hResData;
  LPVOID lpvResData;
  DWORD dwResSize;  
  LPSTR lpszXPM = NULL;

#ifdef UNICODE
#ifdef _DEBUG
  HMODULE hModule = GetModuleHandle(_T("gui3220UD.dll"));
#else
  HMODULE hModule = GetModuleHandle(_T("gui3220U.dll"));
#endif
#else
  #ifdef _DEBUG
  HMODULE hModule = GetModuleHandle(_T("gui3220D.dll"));
#else
  HMODULE hModule = GetModuleHandle(_T("gui3220.dll"));
#endif
#endif 

  if(hModule == NULL)
  {
#ifdef UNICODE
#ifdef _DEBUG
  HMODULE hModule = LoadLibrary(_T("gui3220UD.dll"));
#else
  HMODULE hModule = LoadLibrary(_T("gui3220U.dll"));
#endif
#else
  #ifdef _DEBUG
  HMODULE hModule = LoadLibrary(_T("gui3220D.dll"));
#else
  HMODULE hModule = LoadLibrary(_T("gui3220.dll"));
#endif
#endif
  }
  
  if (!(hResInfo = FindResource(hModule, MAKEINTRESOURCE(uidRes), _T("xpm"))))
    return NULL;
  
  if (hResData = LoadResource(hModule,hResInfo))
  {
    dwResSize = SizeofResource(hModule,hResInfo);
    if (lpvResData = LockResource(hResData))
    {
      lpszXPM = (LPSTR)LocalAlloc(LPTR,dwResSize + 16);
      CopyMemory(lpszXPM ,lpvResData,dwResSize);
    }
  }
  
  return lpszXPM;
}

bool CCmdCtl_i::SetLeftBorder(DWORD dwFolder, DWORD dwMargin)
{
  // Tell scintilla to draw folding lines UNDER the folded line
  SendMessage(SCI_SETFOLDFLAGS, dwFolder,0);
  
  // Set margin 2 = folding margin to display folding symbols
  SendMessage(SCI_SETMARGINMASKN,dwMargin, SC_MASK_FOLDERS);

  return TRUE;
}

void CCmdCtl_i::SetEdgeColor(int foreRgb, int backRgb)
{  
  if (SendMessage(SCI_GETEDGEMODE, 0, 0) == EDGE_LINE)
  {
    SendMessage(SCI_SETEDGECOLOUR, foreRgb, 0);   // edge fore
    // SendMessage(SCI_SETEDGECOLOUR, GetSysColor(COLOR_3DLIGHT), 0);
  }
  else
  {
    SendMessage(SCI_SETEDGECOLOUR, backRgb, 0);   // edge back
    // SendMessage(SCI_SETEDGECOLOUR, GetSysColor(COLOR_3DLIGHT), 0);
  }
}

U32 CCmdCtl_i::SetStyle(int style, int foreRgb, int backRgb, int fontSize)
{
  U32 ret = 0;
  
  if(style == 0xFF)
    style = STYLE_DEFAULT;
  
  if(style < STYLE_MAX)
  {
    if(fontSize > 0 && fontSize < 1000)
    {
      ret |= SendMessage(SCI_STYLESETSIZE, style, fontSize);
    }

    if(foreRgb != -1)
    {
      ret |= SendMessage(SCI_STYLESETFORE,  style, foreRgb);
    }
    
    if(backRgb != -1)
    {
      ret |= SendMessage(SCI_STYLESETBACK, style, backRgb);
    }
  }
  else 
  {
    return ERR_INVALID_PARAM_I;
  }

  //SendMessage(SCI_SETSELFORE,0,0);
  //SendMessage(SCI_SETSELBACK,TRUE,RGB(0xC0,0xC0,0xC0)); // use a default value...
  
  return ret;
}

void CCmdCtl_i::SelectionFore(int foreRgb, bool bUseColor)
{
    SendMessage(SCI_SETSELFORE,bUseColor,foreRgb);
    //SendMessage(SCI_SETSELFORE,0,0);
    //SendMessage(SCI_SETSELBACK,TRUE,RGB(0xC0,0xC0,0xC0)); // use a default value...
}

void CCmdCtl_i::SelectionBack(int BackRgb, char alpha, bool bUseColor)
{
    SendMessage(SCI_SETSELBACK,bUseColor,BackRgb);
    SendMessage(SCI_SETSELALPHA,alpha,0); 
    //SendMessage(SCI_SETSELFORE,0,0);
    //SendMessage(SCI_SETSELBACK,TRUE,RGB(0xC0,0xC0,0xC0)); // use a default value...
}

void CCmdCtl_i::AddMarker(int line, int markerNumber)
{
    SendMessage(SCI_MARKERADD,line,markerNumber);
}

// GetSysColor(COLOR_WINDOWTEXT)
void CCmdCtl_i::CurrentLineStyle(int foreRgb, int backRgb) // RGB((iValue&0xFF0000) >> 16,(iValue&0xFF00) >> 8,iValue&0xFF);
{
    SendMessage(SCI_SETCARETLINEVISIBLE,TRUE,0);// caret line back
    SendMessage(SCI_SETCARETLINEBACK,backRgb,0);
    //SendMessage(SCI_SETCARETLINEVISIBLE,FALSE,0);
    
    SendMessage(SCI_SETCARETFORE,foreRgb,0);
    // SendMessage(SCI_SETCARETFORE,GetSysColor(COLOR_WINDOWTEXT),0); // default value
}

void CCmdCtl_i::CaretforeColor(int foreRgb)
{
  SendMessage(SCI_SETCARETFORE,foreRgb,0);
}

void CCmdCtl_i::BookMarkerStyle(UINT iValue)
{
  LPSTR pszXPM;
  if(iValue > 9)
    return ;

  iValue = max(min(iValue + 99, IDX_BM8), IDX_BM0);
  pszXPM = LoadXPM(iValue);
  if(pszXPM)
  {
    SendMessage(SCI_MARKERDEFINE, 0, SC_MARK_PIXMAP);
    SendMessage(SCI_MARKERDEFINEPIXMAP, 0, (LPARAM) pszXPM);
    SendMessage(SCI_COLOURISE,0,(LPARAM)-1);
    LocalFree(pszXPM);
  }
}

void CCmdCtl_i::CaretBlinkingTime(DWORD dwMilliseconds) // caret blinking time 
{
  SendMessage(SCI_SETCARETPERIOD,dwMilliseconds,0);
}

static long LongFromTwoShorts(short a, short b)
{
  return (a) | ((b) << 16);
}

void CCmdCtl_i::AssignKeyCmd(U32 iVKey, U32 iKeyMode, CMD_ID CmdId)
{
  int mod = 0; 
  int cmd = 0;
  
  if(iKeyMode == VK_CONTROL)
    mod = SCMOD_CTRL;
  if(iKeyMode == VK_MENU)
    mod = SCMOD_ALT;
  if(iKeyMode == VK_SHIFT)
    mod = SCMOD_SHIFT;
  
  if(CmdId == CMD_NEWLINE)
    cmd = SCI_NEWLINE;
  if(CmdId == CMD_NULL)
    cmd = SCI_NULL;
  if(CmdId == CMD_COPY)
    cmd = SCI_COPY;
  if(CmdId == CMD_PASTE)
    cmd = SCI_PASTE;
  
  AssignKey(iVKey, mod, cmd);
}


void CCmdCtl_i::AssignKey(int key, int mods, int cmd)
{
  SendMessage(SCI_ASSIGNCMDKEY, LongFromTwoShorts(static_cast <short> (key), static_cast <short> (mods)), cmd);
}

void CCmdCtl_i::Init()
{
  // clear all text styles
  SendMessage(SCI_CLEARDOCUMENTSTYLE, 0, 0);

  // set the number of styling bits to 7 - the asp/html views need a lot of styling - default is 5
  // If you leave the default you will see twiggle lines instead of ASP code
  SendMessage(SCI_SETSTYLEBITS, 5, 0);
  
  // set the display for indetation guides to on - this displays virtical dotted lines from the beginning of 
  // a code block to the end of the block
  //SendMessage(SCI_SETINDENTATIONGUIDES, TRUE, 0);

  // set tabwidth to 4
  SendMessage(SCI_SETTABWIDTH,4,0);

  // set the caret blinking time to 500 milliseconds
  SendMessage(SCI_SETCARETPERIOD,500,0);

  // source folding section  
  SendMessage(SCI_STYLESETFORE, STYLE_DEFAULT, (LPARAM) GetSysColor(COLOR_WINDOWTEXT)); // default text color
  SendMessage(SCI_STYLESETBACK, STYLE_DEFAULT, (LPARAM) GetSysColor(COLOR_WINDOW)); // default window color

  // set the forground color for some styles
  SendMessage(SCI_STYLESETFORE, 0, RGB(0,0,0));
  SendMessage(SCI_STYLESETFORE, 2, RGB(0,64,0));
  SendMessage(SCI_STYLESETFORE, 5, RGB(0,0,255));
  SendMessage(SCI_STYLESETFORE, 6, RGB(200,20,0));
  SendMessage(SCI_STYLESETFORE, 9, RGB(0,0,255));
  SendMessage(SCI_STYLESETFORE, 10, RGB(255,0,64));
  SendMessage(SCI_STYLESETFORE, 11, RGB(0,0,0));
  
  // set the backgroundcolor of brace highlights
  SendMessage(SCI_STYLESETBACK, STYLE_BRACELIGHT, RGB(0,255,0));
  
  // set end of line mode to CRLF
  // SendMessage(SCI_CONVERTEOLS, SC_EOL_LF, 0); //  
  SendMessage(SCI_SETEOLMODE, SC_EOL_LF, 0);
  //SendMessage(SCI_SETVIEWEOL, TRUE, 0);
  
  SendMessage(SCI_SETSELFORE,0,0);
  SendMessage(SCI_SETSELBACK,TRUE,RGB(0xC0,0xC0,0xC0)); // use a default value...
  
  SendMessage(SCI_SETCARETFORE,GetSysColor(COLOR_WINDOWTEXT),0); // default value
  SendMessage(SCI_SETCARETLINEVISIBLE,FALSE,0);
  
  // set markersymbol for marker type 0 - bookmark
  // define a set of markers to displa folding symbols
  SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPEN, SC_MARK_MINUS);
  SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDER,     SC_MARK_PLUS);
  SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERSUB,  SC_MARK_EMPTY);
  SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERTAIL, SC_MARK_EMPTY);
  SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEREND,  SC_MARK_EMPTY);
  SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPENMID, SC_MARK_EMPTY);
  SendMessage(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_EMPTY);
  
  SendMessage(SCI_SETEDGEMODE, EDGE_NONE, 0);
  SendMessage(SCI_SETINDENT,0,0);
  
  //SendMessage(SCI_SETCARETSTICKY,TRUE,0);
  SendMessage(SCI_SETCARETWIDTH,2,0);
  //SendMessage(SCI_WRAPCOUNT, 2);
  
  //SendMessage(SCI_MARKERDEFINE, 0, SC_MARK_CIRCLE);
  // display all margins
  //SetDisplayLinenumbers(TRUE);
  //SetDisplayFolding(TRUE);
  SetDisplaySelection(TRUE);
  
  // Dont use popup menu.
  SendMessage(SCI_USEPOPUP, 0);
  SendMessage(SCI_SETREADONLY, FALSE);

  SendMessage(SCI_SETVSCROLLBAR, TRUE);
  SendMessage(SCI_SETHSCROLLBAR, FALSE);
  
    // allow notifications for folding actions
  SendMessage(SCI_SETMODEVENTMASK, SC_MOD_INSERTTEXT|SC_MOD_DELETETEXT, 0);
  //   SendMessage(SCI_SETMODEVENTMASK, SC_MOD_CHANGEFOLD|SC_MOD_INSERTTEXT|SC_MOD_DELETETEXT, 0);
  // make the folding margin sensitive to folding events = if you click into the margin you get a notification event
  SendMessage(SCI_SETMARGINSENSITIVEN, 2, TRUE);
    
  SendMessage(SCI_SETMODEVENTMASK, /* SC_MODEVENTMASKALL */ SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT, 0);
  
  SendMessage(SCI_USEPOPUP, FALSE, 0);
  
  SendMessage(SCI_ASSIGNCMDKEY, (SCK_NEXT + (SCMOD_CTRL << 16)), SCI_PARADOWN);
  SendMessage(SCI_ASSIGNCMDKEY, (SCK_PRIOR + (SCMOD_CTRL << 16)), SCI_PARAUP);
  SendMessage(SCI_ASSIGNCMDKEY, (SCK_NEXT + ((SCMOD_CTRL | SCMOD_SHIFT) << 16)), SCI_PARADOWNEXTEND);
  SendMessage(SCI_ASSIGNCMDKEY, (SCK_PRIOR + ((SCMOD_CTRL | SCMOD_SHIFT) << 16)), SCI_PARAUPEXTEND);
  
  SendMessage(SCI_ASSIGNCMDKEY, (SCK_HOME + (0 << 16)), SCI_VCHOMEWRAP);
  SendMessage(SCI_ASSIGNCMDKEY, (SCK_END + (0 << 16)), SCI_LINEENDWRAP);
  SendMessage(SCI_ASSIGNCMDKEY, (SCK_HOME + (SCMOD_SHIFT << 16)), SCI_VCHOMEWRAPEXTEND);
  SendMessage(SCI_ASSIGNCMDKEY, (SCK_END + (SCMOD_SHIFT << 16)), SCI_LINEENDWRAPEXTEND);
  
  SendMessage(SCI_CMD_MODE,TRUE,0);
  SetCtrlCharMode(TRUE);
  
  AssignKey('C', SCMOD_CTRL, SCI_NULL);
  AssignKey('V', SCMOD_CTRL, SCI_NULL);
  
  // Nonprinting characters
  ShowPrintCharacter(FALSE, FALSE);
    
  SendMessage(SCI_SETUNDOCOLLECTION,0);
  SendMessage(SCI_EMPTYUNDOBUFFER);
  
  //SendMessage(SCI_ASSIGNCMDKEY, ('c' + (SCMOD_CTRL << 16)), SCI_COPY);
  //SendMessage(SCI_SETCODEPAGE,  SC_CP_UTF8, 0);
}

U32 CCmdCtl_i::SetCtrlCharMode(bool bCtrlMode)
{
  return SendMessage(SCI_SET_CTRLCHAR_MODE,bCtrlMode,0);
}

U32 CCmdCtl_i::InitHistoryCommand(CONST CHAR* pFileName)
{
    return SendMessage(SCI_CMD_HISTORY,1, (LPARAM)pFileName); // Load Command History 
}

U32 CCmdCtl_i::SaveHistoryCommand()
{
    return SendMessage(SCI_CMD_HISTORY,2,0); // Save Command History 
}

U32 CCmdCtl_i::SetCommandMode(bool bEnalbe)
{
  SendMessage(SCI_CMD_MODE,bEnalbe,0);
    
  return ERR_OK_I;
}

void CCmdCtl_i::ShowPrintCharacter(bool bViewWhiteSpace, bool bViewEOLs)
{
  SendMessage(SCI_SETVIEWWS,(bViewWhiteSpace)?SCWS_VISIBLEALWAYS:SCWS_INVISIBLE,0);
  SendMessage(SCI_SETVIEWEOL,bViewEOLs,0);
}

//  CodePageFromCharSet()
//
UINT CodePageFromCharSet(UINT uCharSet) // LOGFONT.lfCharSet; 
/*
ANSI_CHARSET
BALTIC_CHARSET
CHINESEBIG5_CHARSET
DEFAULT_CHARSET
EASTEUROPE_CHARSET
GB2312_CHARSET
*/
{
  CHARSETINFO ci;
  if (TranslateCharsetInfo((DWORD*)uCharSet,&ci,TCI_SRCCHARSET))
    return(ci.ciACP);
  else
    return(GetACP());
}

// New codepage detection functions from Textview 6.0


#define IDM_ENCODING_ANSI               40100
#define IDM_ENCODING_UNICODE            40101
#define IDM_ENCODING_UNICODEREV         40102
#define IDM_ENCODING_UTF8               40103
#define IDM_ENCODING_UTF8SIGN           40104
#define IDM_ENCODING_SETDEFAULT         40105

static int iDefaultCodePage = ANSI_CHARSET;

// int iEncodings[5] = { NCP_DEFAULT,
//                       NCP_UNICODE|NCP_UNICODE_BOM,
//                       NCP_UNICODE|NCP_UNICODE_REVERSE|NCP_UNICODE_BOM,
//                       NCP_UTF8,
//                       NCP_UTF8|NCP_UTF8_SIGN };

int CCmdCtl_i::GetEncoding()
{
  LRESULT r = SendMessage(SCI_GETCODEPAGE,0,0); 
  return r; 
}

void CCmdCtl_i::SetScroll(bool Vs, bool Hs) 
{
  SendMessage(SCI_SETVSCROLLBAR, Vs);
  SendMessage(SCI_SETHSCROLLBAR, Hs);
}


int CCmdCtl_i::SetCharSet(int iCharSet, int style)
{
  SendMessage(SCI_STYLESETCHARACTERSET, style, iCharSet);
  
  return 0;
}

/* SC_CP_UTF8: 
sets Scintilla into Unicode mode with the document treated as a sequence of characters expressed in UTF-8. 
The text is converted to the platform's normal Unicode encoding before being drawn by the OS and thus
 can display Hebrew, Arabic, Cyrillic, and Han characters. Languages which can use two characters stacked 
 vertically in one horizontal space, such as Thai, will mostly work but there are some issues where
 the characters are drawn separately leading to visual glitches. Bi-directional text is not supported. 
Code page can be set to 932 (Japanese Shift-JIS), 936 (Simplified Chinese GBK), 
949 (Korean Unified Hangul Code), 950 (Traditional Chinese Big5), or 
1361 (Korean Johab) although these may require installation of language specific support.
// Call with codePage set to zero to disable DBCS support 
*/
int CCmdCtl_i::Endcoding(int iCodePage)
{
  SendMessage(SCI_SETCODEPAGE,  iCodePage , 0);
  
  return 0;
}

U32 CCmdCtl_i::GetTotalLength(VOID)
{
  return (U32)SendMessage(SCI_GETLENGTH);
}

U32 CCmdCtl_i::GetTotalText(U32 ulBufferSize, PVOID pBuffer)
{
  return SendMessage(SCI_GETTEXT, ulBufferSize,(long)pBuffer);
}


U32 CCmdCtl_i::GetSelectionLength()
{
   U32 start = SendMessage(SCI_GETSELECTIONSTART, 0, 0);
   U32 end   = SendMessage(SCI_GETSELECTIONEND, 0, 0);

   return end - start;
}


U32 CCmdCtl_i::GetCurrentLineText(LPSTR pszLineBuf, U32 ulSize)
{
  return SendMessage(SCI_GETCURLINE, (WPARAM)ulSize, (LPARAM)pszLineBuf);
}

bool CCmdCtl_i::DeleteCurrentLineText()
{
  int iCurPos  = SendMessage(SCI_GETCURRENTPOS, 0, 0);
  int iCurLine = SendMessage(SCI_LINEFROMPOSITION, (WPARAM) iCurPos, 0);
  
  SendMessage(SCI_LINEDELETE, (WPARAM)0, (LPARAM)0);
  
  return TRUE;
}

void CCmdCtl_i::SetDisplaySelection(bool bFlag)
{
   if (bFlag)
	   SendMessage(SCI_SETMARGINWIDTHN, 1, 16);
   else
	   SendMessage(SCI_SETMARGINWIDTHN, 1, 0);
}

// Toggle the display of the folding margin
// void | not used
//
void CCmdCtl_i::SetDisplayFolding(bool bFlag)
{
   if (bFlag)
	   SendMessage(SCI_SETMARGINWIDTHN, 2, 16);
   else
	   SendMessage(SCI_SETMARGINWIDTHN, 2, 0);
}

// Set the display of line numbers on or off.
// Scintilla uses 3 "Margin" at the left of the edit window. The Margin (0)
// is used to display the linenumbers. If set to 0 no numbers are displayed. If
// set to a width > 0 - linenumbers are displayed. We use a helper function to 
// calculate the size of this margin.
// int | number of pixels for the margin width of margin (0) 
//
void CCmdCtl_i::SetDisplayLinenumbers(bool bFlag) // flag if we shuld display line numbers
{
  // if display is turned off we set margin 0 to 0
  if (!bFlag)
  {
    SendMessage(SCI_SETMARGINWIDTHN, 0, 0);
  }
  // if display is turned o we set margin 0 to the calculated width
  else
  {
    int nWidth = GetLinenumberWidth(); // + 4;
    SendMessage(SCI_SETMARGINWIDTHN, 0, nWidth);
  }
  m_bLinenumbers = bFlag;
}

// Try to calculate the number of characters needed for the display of the linenumbers.
// This function returns 3 for 10-99 lines in file, 4 for 100-999 lines and so on.
// int | number of characters needed to display linenumbers
//
int CCmdCtl_i::GetLinenumberChars ()
{
// try to get number of lines in control
   LRESULT lLines = SendMessage(SCI_GETLINECOUNT, 0, 0);

   int nChars = 1;
   while (lLines > 0)
   {
      lLines = lLines / 10;
      nChars++;
   }
   return nChars; // allow an extra char for safety
}

// Calculate a reasonable width for Linenumberdisplay
// number of pixels for the margin width of margin (0) 
int CCmdCtl_i::GetLinenumberWidth ()
{
  // get number of chars needed to display highest linenumber
  int nChars = GetLinenumberChars ()+1;
  // get width of character '9' in pixels
  LRESULT lWidth = SendMessage(SCI_TEXTWIDTH, STYLE_LINENUMBER, (long)_T("9"));
  return nChars * lWidth;
}

void CCmdCtl_i::UpdateMarginWidth(VOID)
{
  if (!m_bLinenumbers)
    return; 
  
  //int nWidth = GetLinenumberWidth() + 4;
  int nWidth = GetLinenumberWidth();
	SendMessage(SCI_SETMARGINWIDTHN, 0, nWidth);
}

// @mfunc Copy the selected text to the clipboard
// @rvalue void | not used
//
void CCmdCtl_i::Copy()
{
   SendMessage(SCI_COPY, 0, 0);
}

void CCmdCtl_i::Paste()
{
   SendMessage(SCI_PASTE, 0, 0);
}

void CCmdCtl_i::Clear()
{
   SendMessage(SCI_CLEAR, 0, 0);
}

void CCmdCtl_i::EnableUndo(bool bEnalbe)
{
  SendMessage(SCI_SETUNDOCOLLECTION, bEnalbe,0);
  SendMessage(SCI_EMPTYUNDOBUFFER, 0,0);
}

void CCmdCtl_i::ClearAll()
{
   SendMessage(SCI_CLEARALL, 0, 0);
}

void CCmdCtl_i::SelectAll()
{
   SendMessage(SCI_SELECTALL, 0, 0);
}

// Update UI and do brace matching
void CCmdCtl_i::UpdateUI()
{
  // do brace matching
  long lStart = SendMessage(SCI_GETCURRENTPOS, 0, 0);
  long lEnd = SendMessage(SCI_BRACEMATCH, lStart-1, 0);

  // if there is a matching brace highlight it
  if (lEnd >= 0)
    SendMessage(SCI_BRACEHIGHLIGHT, lStart-1, lEnd);
  // if there is NO matching brace erase old highlight
  else
    SendMessage(SCI_BRACEHIGHLIGHT, -1, -1);
}

CONST int iLineEndings[3] = { SC_EOL_CRLF,SC_EOL_LF, SC_EOL_CR };

void CCmdCtl_i::SetEOLMode(int mode) // 
{
  if(mode > 2)
  {
    return ;
  }

  SendMessage(SCI_SETEOLMODE, iLineEndings[mode], 0 );
}


void CCmdCtl_i::AutoIndent(CHAR ch)
{
  // Auto indent
  char* pLineBuf;
  char* pPos;
  
  // int iIndentLen;
  
  int iCurPos = SendMessage(SCI_GETCURRENTPOS, 0, 0);
  
  // intiAnchorPos = SendMessage(SCI_GETANCHOR,0,0);
  int iCurLine    = SendMessage(SCI_LINEFROMPOSITION, (WPARAM) iCurPos, 0);
  int iLineLength = SendMessage(SCI_LINELENGTH, iCurLine, 0);
  
  // intiIndentBefore = SendMessage(SCI_GETLINEINDENTATION,(WPARAM)iCurLine-1,0);
  
  if (iCurLine > 0 /* && iLineLength <= 2 */ )
  {
    int iPrevLineLength = SendMessage(SCI_LINELENGTH, iCurLine - 1, 0);
    pLineBuf = (char*)GlobalAlloc(GPTR, iPrevLineLength + 1);
    if (pLineBuf)
    {
      SendMessage(SCI_GETLINE, iCurLine - 1, (LPARAM) pLineBuf);
      *(pLineBuf + iPrevLineLength) = '\0';
      for (pPos = pLineBuf; *pPos; pPos++)
      {
        if (*pPos != ' ' && *pPos != '\t')
          *pPos = '\0';
      }
      if (*pLineBuf)
      {
        SendMessage(SCI_BEGINUNDOACTION, 0, 0);
        SendMessage(SCI_ADDTEXT, lstrlen(pLineBuf), (LPARAM) pLineBuf);
        SendMessage(SCI_ENDUNDOACTION, 0, 0);
      }
      GlobalFree(pLineBuf);
    }
  }
}


void CCmdCtl_i::BraceMatch(PCSTR pszMatchString)   // "()[]{}" 
{
  int iPos = SendMessage(SCI_GETCURRENTPOS, 0, 0);
  char c   = (char)SendMessage(SCI_GETCHARAT, iPos, 0);
  
  if (StrChr(pszMatchString, c))
  {
    int iBrace2 = SendMessage(SCI_BRACEMATCH, iPos, 0);
    
    if (iBrace2 != -1)
      SendMessage(SCI_BRACEHIGHLIGHT, iPos, iBrace2);
    else
      SendMessage(SCI_BRACEBADLIGHT, iPos, 0);
  }
  
  // Try one before
  else
  {
    iPos = SendMessage(SCI_POSITIONBEFORE, iPos, 0);
    c    = (char)SendMessage(SCI_GETCHARAT, iPos, 0);
    if (StrChr(pszMatchString, c))
    {
      int iBrace2 = SendMessage(SCI_BRACEMATCH, iPos, 0);
      
      if (iBrace2 != -1)
        SendMessage(SCI_BRACEHIGHLIGHT, iPos, iBrace2);
      else
        SendMessage(SCI_BRACEBADLIGHT, iPos, 0);
    }
    else
      SendMessage(SCI_BRACEHIGHLIGHT, (WPARAM) - 1, (LPARAM) - 1);
  }
}

// Do default folding for given margin and position - usually called from notification handler
void CCmdCtl_i::DoDefaultFolding(U32 nMargin,      // maring we want to handle - normally (2)
                              U32 lPosition)    // character position where user clicked margin
{
  // simply toggle fold
  if (nMargin == 2)
  {
    long lLine = SendMessage(SCI_LINEFROMPOSITION, lPosition, 0);
    SendMessage(SCI_TOGGLEFOLD, lLine, 0);
  }
}

void CCmdCtl_i::ClearAllBookmarks()
{
  SendMessage(SCI_MARKERDELETEALL,(WPARAM)-1,0);
}

void CCmdCtl_i::JumpToBookmark(bool bNext)
{
  int iCurrentPos = SendMessage(SCI_GETCURRENTPOS,0,0);
  int iCurrentLine = SendMessage(SCI_LINEFROMPOSITION,(WPARAM)iCurrentPos,0);
  int iMaxLine = SendMessage(SCI_GETLINECOUNT,0,0) - 1;
  
  if (bNext)
  {
    int iMarkerLine = -1;

    if (iCurrentLine < iMaxLine)
      iMarkerLine = SendMessage(SCI_MARKERNEXT,(WPARAM)iCurrentLine+1,1);

    if (iMarkerLine == -1)
      iMarkerLine = SendMessage(SCI_MARKERNEXT,(WPARAM)0,1);

    if (iMarkerLine != -1) {
      SendMessage(SCI_GOTOLINE,(WPARAM)iMarkerLine,0);
      SendMessage(SCI_CHOOSECARETX,0,0); }
  }

  else
  {
    int iMarkerLine = -1;

    if (iCurrentLine > 0)
      iMarkerLine = SendMessage(SCI_MARKERPREVIOUS,(WPARAM)iCurrentLine-1,1);

    if (iMarkerLine == -1)
      iMarkerLine = SendMessage(SCI_MARKERPREVIOUS,(WPARAM)iMaxLine,1);

    if (iMarkerLine != -1) {
      SendMessage(SCI_GOTOLINE,(WPARAM)iMarkerLine,0);
      SendMessage(SCI_CHOOSECARETX,0,0); }
  }
}

void CCmdCtl_i::ToggleBookmark()
{
  int iCurrentPos = SendMessage(SCI_GETCURRENTPOS,0,0);
  int iCurrentLine = SendMessage(SCI_LINEFROMPOSITION,(WPARAM)iCurrentPos,0);
  int iMarkerBits = SendMessage(SCI_MARKERGET,(WPARAM)iCurrentLine,0);

  if (iMarkerBits & 1) // Bit 0 represents the bookmark
    SendMessage(SCI_MARKERDELETE,(WPARAM)iCurrentLine,0);
  else
    SendMessage(SCI_MARKERADD,(WPARAM)iCurrentLine,0);
}

int CCmdCtl_i::GetCurrentStyle()
{
   long lPos = SendMessage(SCI_GETCURRENTPOS, 0, 0);
   return SendMessage(SCI_GETSTYLEAT, lPos, 0);
}

int CCmdCtl_i::GetCurrentLineNumber()
{
  int iCurrentPos = SendMessage(SCI_GETCURRENTPOS,0,0);
  int iCurrentLine = SendMessage(SCI_LINEFROMPOSITION,(WPARAM)iCurrentPos,0);
  return iCurrentLine;
}

// set the fontname e.g. "Arial"
void CCmdCtl_i::SetFontname(int nStyle, LPCSTR pszFontname)
{
  if(nStyle == -1)
    nStyle = STYLE_DEFAULT;
   SendMessage(SCI_STYLESETFONT, nStyle, (long)pszFontname);
}

// Set the font height in points
void CCmdCtl_i::SetFontheight(int nStyle, int nHeight)
{
  if(nStyle == -1)
    nStyle = STYLE_DEFAULT;
   SendMessage(SCI_STYLESETSIZE, nStyle, (long)nHeight);
}

// et the foreground color
void CCmdCtl_i::SetForeground(int nStyle, COLORREF crForeground)
{
  if(nStyle == -1)
    nStyle = STYLE_DEFAULT;
   SendMessage(SCI_STYLESETFORE, nStyle, (long)crForeground);
}

void CCmdCtl_i::SetCaretLineBackground(COLORREF crBackground)
{
  SendMessage(SCI_SETCARETLINEVISIBLE,TRUE,0);
  SendMessage(SCI_SETCARETLINEBACK,crBackground,0);
}

// set the backgroundcolor
void CCmdCtl_i::SetBackground(int nStyle, COLORREF crBackground)
{
  if(nStyle == -1)
    nStyle = STYLE_DEFAULT;
   SendMessage(SCI_STYLESETBACK, STYLE_DEFAULT, (LPARAM)crBackground); // GetSysColor(COLOR_WINDOW)); 
   SendMessage(SCI_STYLESETBACK, nStyle, (long)crBackground);
}

// set given style to bold
void CCmdCtl_i::SetBold(int nStyle, bool bBold)
{
  if(nStyle == -1)
    nStyle = STYLE_DEFAULT;
   SendMessage(SCI_STYLESETBOLD, nStyle, (long)bBold);
}

// set given style to bold
void CCmdCtl_i::SetItalic(int nStyle, bool bItalic)
{
  if(nStyle == -1)
    nStyle = STYLE_DEFAULT;
   SendMessage(SCI_STYLESETITALIC, nStyle, (long)bItalic);
}

// set given style to bold
void CCmdCtl_i::SetUnderline(int nStyle, bool bUnderline)
{
  if(nStyle == -1)
    nStyle = STYLE_DEFAULT;
   SendMessage(SCI_STYLESETUNDERLINE, nStyle, (long)bUnderline);
}

VOID CCmdCtl_i::NotifyTextChanged()
{
  UpdateMarginWidth();
}

VOID CCmdCtl_i::NotifyDoubleClick()
{

}

VOID CCmdCtl_i::NotifyNewLine(CHAR* pTxt)
{

}

VOID CCmdCtl_i::AddChar(CHAR ch)
{
  
}

bool CCmdCtl_i::UiNotify(WPARAM wParam, 
                      LPARAM lParam, // pointer to notification structure
                      LRESULT* pResult)   
{
  NMHDR *phDR;
  phDR = (NMHDR*)lParam;
    
  // does notification come from my scintilla window?
  if (phDR != NULL && phDR->hwndFrom == m_hWnd)
  {
    SCNotification *pMsg = (SCNotification*)lParam;
    switch (phDR->code)
    {
    case SCN_STYLENEEDED:
      break;

    case SCN_CHARADDED:
      AddChar(pMsg->ch);
      break;
      
    case SCN_SAVEPOINTREACHED:
      break;

    //   called when the document is changed - mark document modified
    case SCN_SAVEPOINTLEFT:
      {
//         CDocument *pDoc = GetDocument();
//         pDoc->SetModifiedFlag(TRUE);
      }
      break;
    
    case SCN_MODIFYATTEMPTRO:
      break;
    
    case SCN_KEY:
      {
        //GetCurrentLineText();
      }
      break;
    case SCN_NEWLINE:
      NotifyNewLine(pMsg->pNewLine);
      break;
    
    case SCN_TAB:
      break;

    case SCN_KEY_DOWN:
      {
        BOOL bKeyDown = (::GetKeyState(pMsg->ch) & 0x80000000);
        
        int key       = pMsg->ch;
        char c        = (char)key;
        int modifiers = pMsg->modifiers;

        if(m_bBookMark)
        {
          if (bKeyDown && (modifiers & SCMOD_CTRL) && (modifiers & SCMOD_SHIFT) && (key == VK_F2))
          {
            ClearAllBookmarks();
          }
          else if (bKeyDown && (modifiers & SCMOD_CTRL) && (key == VK_F2))
          {
            SetDisplaySelection(TRUE);
            ToggleBookmark();
          }
          else if (bKeyDown && (modifiers & SCMOD_SHIFT) && (key == VK_F2))
          {
            JumpToBookmark(FALSE);
          }
          else if (bKeyDown && key == VK_F2)
          {
            JumpToBookmark(TRUE);
          }
        }
      }
      break;
    
    case SCN_DOUBLECLICK:
      NotifyDoubleClick();
      break;
      
      //       called when something changes and we want to show new indicator state or brace matching
    case SCN_UPDATEUI:
      {
        UpdateUI();
      }
      break;
      
    case SCEN_CHANGE:
      break;
      
    case SCN_ZOOM:
    case SCN_MODIFIED:
      {
        NotifyTextChanged();
      }      
      break;
      
    case SCN_MACRORECORD:
      break;

      // user clicked margin - try folding action
    case SCN_MARGINCLICK:
      {
        DoDefaultFolding(pMsg->margin, pMsg->position);
      }
      break;
    
    case SCN_NEEDSHOWN:
      break;
    case SCN_PAINTED:
      break;
    case SCN_USERLISTSELECTION:
      break;
    case SCN_URIDROPPED:
      break;
    case SCN_DWELLSTART:
      break;
    case SCN_DWELLEND:
      break;
    }
    return TRUE; // we processed the message
  }

  return TRUE;
}

void CCmdCtl_i::EnableReadOnly(bool bReadOnly)
{
  SendMessage(SCI_SETREADONLY, bReadOnly, 0);
}

void CCmdCtl_i::EnableKeyinput(bool bEnable)
{
  SendMessage(SCI_CMD_HISTORY, 1, 0);
  SendMessage(SCI_CMD_KEY_INTPUT_MODE, bEnable, 0);
}


LRESULT CCmdCtl_i::SetLimitSize(DWORD dwLimitSize)
{
  return SendMessage(SCI_CMD_LIMIT_TEXT_SIZE, dwLimitSize, 0); 
}


void CCmdCtl_i::EnableBookMark(bool bMark)
{
   m_bBookMark    = bMark;
}

void CCmdCtl_i::EnableCtrlcvxzy(bool bCtrlcvxzy)
{
   m_bCtrlcvxzy    = bCtrlcvxzy;
}


LRESULT CCmdCtl_i::AddStyledTextEx(CONST CHAR* szText, CHAR style, bool bAppend)
{
  CHAR buf[2048+2];
  size_t len = strlen(szText);
  
  if(len >= 1024)
  {
    return ERR_INVALID_PARAM_I;
  }
  
  if (bAppend)
  {
    SendMessage(SCI_DOCUMENTEND, 0, 0); 
  }
  
  if(style == -1)
  {
    style = 32;
  }
  
  for (size_t i = 0; i < len; i++)
  {
    buf[i* 2] = szText[i];
    buf[i * 2 + 1] = static_cast < char > (style);
  }
  
  if (bAppend)
  {
    SendMessage(SCI_DOCUMENTEND, 0, 0); 
  }
  
  return SendMessage(SCI_ADDSTYLEDTEXT, (WPARAM)(len* 2), (LPARAM)(buf));
}


LRESULT CCmdCtl_i::AddStyledText(CONST CHAR* pText, U32 ulTextSize, CHAR cStyle, bool bAppend)
{
  CHAR cStyleBuf[1024+1] = {0, };
  
  if(ulTextSize == 0)
  {
    ulTextSize = strlen(pText);
  }
  
  if(ulTextSize >  1024)
  {
    ulTextSize = 1024;
  }
  
  if((cStyle == -1) || (cStyle > 32))
  {
    cStyle = 32;
  }
  
  for (size_t i = 0; i < ulTextSize; i++)
  {
    cStyleBuf[i] = static_cast <char> (cStyle);
  }
  
  if(bAppend)
  {
    SendMessage(SCI_DOCUMENTEND, 0, 0);
  }
  
  SendMessage(SCI_ADDTEXT,      (WPARAM)(ulTextSize), (LPARAM)(pText));
  SendMessage(SCI_SETSTYLINGEX, (WPARAM)(ulTextSize), (LPARAM)(cStyleBuf));
  
  if (bAppend)
  {
    SendMessage(SCI_DOCUMENTEND, 0, 0); 
  }
  
  return 0;
}

LRESULT CCmdCtl_i::AddText(LPCSTR pszText, U32 szTextLen, bool bScroll)
{
  LRESULT ret = 0;

  if(bScroll)
  {
    SendMessage(SCI_DOCUMENTEND, 0, 0);
  }
  
  if(pszText && szTextLen)
  {
    // The current selection is not changed and the new text is not scrolled into view.
    //SendMessage(SCI_SETREADONLY, FALSE, 0);
    ret = SendMessage(SCI_APPENDTEXT, (WPARAM)szTextLen, (LPARAM)pszText);
    if(bScroll)
    {
      SendMessage(SCI_DOCUMENTEND, 0, 0);
      //LRESULT lLines = SendMessage(SCI_GETLINECOUNT, 0, 0);
      //SendMessage(SCI_GOTOLINE, lLines-1, 0);
    }
  }

  //SendMessage(SCI_SETREADONLY, TRUE, 0);
  
  return ret;
}


LRESULT CCmdCtl_i::AddLastLineStartText(LPCSTR pszText, U32 szTextLen, bool bScroll)
{
  LRESULT ret = 0;

  if(bScroll)
  {
    SendMessage(SCI_DOCUMENTEND, 0, 0);
  }
  
  // The current selection is not changed and the new text is not scrolled into view.
  //SendMessage(SCI_SETREADONLY, FALSE, 0);
  ret = SendMessage(SCI_LINESTART_TEXT, (WPARAM)szTextLen, (LPARAM)pszText);
  if(bScroll)
  {
    SendMessage(SCI_DOCUMENTEND, 0, 0);
    //LRESULT lLines = SendMessage(SCI_GETLINECOUNT, 0, 0);
    //SendMessage(SCI_GOTOLINE, lLines-1, 0);
  }
  //SendMessage(SCI_SETREADONLY, TRUE, 0);
  
  return ret;
}


LRESULT CCmdCtl_i::AddLastLineStartStyledText(CONST CHAR* pText, U32 ulTextSize, CHAR cStyle, bool bAppend)
{
  CHAR cStyleBuf[1024] = {0, };
  
  if(ulTextSize >= 1024)
    return ERR_INVALID_PARAM_I;
  
  if(bAppend)
  {
    SendMessage(SCI_DOCUMENTEND, 0, 0);
  }

  if(ulTextSize == 0)
    ulTextSize = strlen(pText);
  
  if(cStyle == -1 || cStyle > 32)
    cStyle = 32;
  
  for (size_t i = 0; i < ulTextSize; i++)
  {
    cStyleBuf[i] = static_cast <char> (cStyle);
  }
    
  SendMessage(SCI_LINESTART_TEXT,   (WPARAM)(ulTextSize), (LPARAM)(pText));
  SendMessage(SCI_SETSTYLINGEX, (WPARAM)(ulTextSize), (LPARAM)(cStyleBuf));

  if (bAppend)
  {
    SendMessage(SCI_DOCUMENTEND, 0, 0);
    //LRESULT lLines = SendMessage(SCI_GETLINECOUNT, 0, 0);
    //SendMessage(SCI_GOTOLINE, lLines-1, 0); 
  }

  return 0;
}

LRESULT CCmdCtl_i::InsertSpecialChar(CHAR ch, bool bCurrentPos)
{
  LRESULT ret = 0;
  ret = SendMessage(SCI_INSERT_CTRLCHAR, ch, bCurrentPos?-1:0); // -1: current pos; 0: doc end position.
  
  return ret;
}

#if 0 
LRESULT CCmdCtl_i::LineSplit()
{
  LRESULT ret = 0;
  int iCurPos = SendMessage(SCI_GETCURRENTPOS, 0, 0);
  
  ret = SendMessage(SCI_LINESSPLIT, iCurPos); 
  
  return ret;
}
#endif 
