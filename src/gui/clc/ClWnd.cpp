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
  
#pragma comment(exestr, "GUI3220 lib")

#ifdef _DEBUG
#pragma data_seg(".DbgVer")
int g_DbgVershared = 0;
#pragma data_seg()
#pragma comment(linker, "/SECTION:.DbgVer,RWS")
#endif 

#ifndef HAVE_SUPPORT_UNICODE 
#undef StrChr
#undef lstrlen
#define StrChr  StrChrA 
#define lstrlen lstrlenA 
#endif


bool CCWndMain(HINSTANCE hInstance, DWORD dwReason) 
{
	if (dwReason == DLL_PROCESS_ATTACH) 
  {
		if (!Scintilla_RegisterClasses(hInstance))
			return FALSE;
	}
  else if (dwReason == DLL_PROCESS_DETACH) 
  {
		Scintilla_ReleaseResources();
	}
	return TRUE;
}

CEditCtl_i::CEditCtl_i()
{
   m_bLinenumbers = FALSE;
   m_bSelection = TRUE;
   m_bFolding = FALSE;
   m_nSearchflags = 0;
   m_bBookMark    = TRUE;
   m_bFind        = TRUE;
   m_bCtrlc       = TRUE;
   m_CtrlId       = 0;
   m_bOutpuMode   = FALSE;
}

CEditCtl_i::~CEditCtl_i()
{

}


#define STR_SCINTILLAWND _T("Scintilla")

//  Create the window
bool CEditCtl_i::Create (LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
  m_CtrlId = nID;
  if (!CWnd::CreateEx(WS_EX_CLIENTEDGE, STR_SCINTILLAWND, lpszWindowName, dwStyle, rect, pParentWnd,(UINT)nID))
  {
    return FALSE;
  }
  

#if 0
  	// CPP lexer
	//SendMessage( SCI_SETLEXER, SCLEX_CPP );

	// Set number of style bits to use
	SendMessage( SCI_SETSTYLEBITS, 5 );
	
	// Set tab width
	SendMessage( SCI_SETTABWIDTH, 4 );
  
	// Use CPP keywords
	//SendEditor( SCI_SETKEYWORDS, 0, (LPARAM)g_cppKeyWords );

	// Set up the global default style. These attributes are used wherever no explicit choices are made.
	SetStyle( STYLE_DEFAULT, WHITE, BLACK, 10, "Courier New" );
  
	// Set caret foreground color
	SendMessage( SCI_SETCARETFORE, RGB( 255, 255, 255 ) );
  
	// Set all styles
	SendMessage( SCI_STYLECLEARALL );
  
	// Set selection color
	SendMessage( SCI_SETSELBACK, TRUE, RGB( 0, 0, 255 ) );
	
	// Set syntax colors
//	for ( long i = 0; g_rgbSyntaxCpp[ i ].iItem != -1; i++ ) 
		//SetAStyle( g_rgbSyntaxCpp[ i ].iItem, g_rgbSyntaxCpp[ i ].rgb );
#else
    Init();
  	
//   SendMessage(SCI_SETSTYLEBITS, 7, 0);
//   SendMessage(SCI_STYLERESETDEFAULT, 0, 0);
//   SendMessage(SCI_SETCODEPAGE, SC_CP_UTF8, 0);
//   
//   SendMessage(SCI_STYLESETFONT, STYLE_DEFAULT,reinterpret_cast<uptr_t>("Arial"));
//   SendMessage( SCI_STYLESETSIZE, STYLE_DEFAULT, 12);
//   SendMessage( SCI_STYLESETBACK, STYLE_DEFAULT, RGB(0xff, 0xff, 0xff));
//   SendMessage( SCI_STYLECLEARALL, 0, 0);
//   
//   SendMessage(SCI_STYLESETFORE, 0, RGB(0xff, 0, 0));
//   SendMessage( SCI_STYLESETFORE, 1, RGB(0xff, 0x00, 0xff) );
//   SendMessage( SCI_STYLESETFORE, 2, RGB(0, 0, 0) );
//   SendMessage(SCI_STYLESETFORE, 4, RGB(0, 0x7f, 0x7f) );
//   SendMessage(SCI_STYLESETITALIC, 2, 1);
#endif 

  
  return TRUE;
}

// BOOL CEditCtl_i::OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
// { 
//   LPNMHDR pnmh = (LPNMHDR)lParam;
//   
//   if(pnmh->idFrom == m_CtrlId && !m_bOutpuMode)
//   {
//     UiNotify(wParam, lParam, pResult);
//   }
//    
//   return CWnd::OnNotify(wParam, lParam, pResult);
// }

U32 CEditCtl_i::SetOutputMode(bool bOutputMode)
{
  m_bOutpuMode = bOutputMode ;
  return SendMessage(SCI_OUTPUT_MODE, bOutputMode, 0);
}

// Reset the Scintiall control and add new Text
ULONG CEditCtl_i::SetText (LPCSTR szText)
{
  LRESULT lResult = 0;
  if (szText != NULL)
  {
    lResult = SendMessage(SCI_SETTEXT,0,(LPARAM)szText);
  }

  GotoPosition(0);

  SetFocus();

  return 0;
}
 
// Goto givven character position
ULONG CEditCtl_i::GotoPosition(U32 lPos) // character position
{
	SendMessage(SCI_GOTOPOS, lPos, 0);

  return 0;
}

ULONG CEditCtl_i::GotoLine(U32 lLine) // lines start at 0
{
	return SendMessage(SCI_GOTOLINE, lLine, 0);
}

DWORD CEditCtl_i::GetLines(VOID)
{
 return SendMessage(SCI_GETLINECOUNT, 0, 0);
}

#if 0 
DWORD CEditCtl_i::GetLine2(int line, BYTE* pszLineBuf, U32& ulSize)
{
  int len;
  
  // Get needed buffer size
  if (line < 0)
  {
    len = SendMessage(SCI_GETCURLINE, 0, 0);
  }
  else
  {
    len = SendMessage(SCI_GETLINE, line, 0);
  }
  
  if(len > ulSize)
  {
    return ERR_ITS_INVALID_PARAM;
  }
  
  ulSize = len;
  
  // And get the line
  if (line < 0)
  {
    SendMessage(SCI_GETCURLINE, len, pszLineBuf);
  }
  else
  {
    SendMessage(SCI_GETLINE, line, pszLineBuf);
  }
  
  return ERR_OK_I;
}
#endif  


DWORD CEditCtl_i::GetPosition(int line, int& lineStart, int& lineEnd)
{
  lineStart = SendMessage(SCI_POSITIONFROMLINE, line, 0);
  lineEnd   = SendMessage(SCI_GETLINEENDPOSITION, line, 0);

  return 0;
}

int CEditCtl_i::GetLineLength(int line)
{
  int iLineLength = SendMessage(SCI_LINELENGTH, line, 0);
  return iLineLength;
}

DWORD CEditCtl_i::GetLine(int line, BYTE* pLineBuf, U32& ulSize)
{
  int lineStart = SendMessage(SCI_POSITIONFROMLINE, line, 0);
  int lineEnd   = SendMessage(SCI_GETLINEENDPOSITION, line, 0);
  
  if(lineEnd < lineStart)
  {
    ulSize  = 0 ;
    return -1;
  }
  else 
  {
    ulSize = lineEnd - lineStart;
  }
  
  return GetRange(lineStart,lineEnd,pLineBuf);
}

DWORD CEditCtl_i::GetRange(int selStart, int selEnd, BYTE* pOutText)
{
  TextRange tr;
  
  tr.chrg.cpMin = selStart;
  tr.chrg.cpMax = selEnd;
  tr.lpstrText  = (char*)pOutText;
  SendMessage(SCI_GETTEXTRANGE, 0, (LPARAM)&tr);

  return 0;
}

DWORD CEditCtl_i::GotoLastLine()
{
  //int lineCurrent = SendMessage(SCI_LINEFROMPOSITION, SendMessage(SCI_GETCURRENTPOS, 0, 0), 0) - 1;
  int lineCnt   = SendMessage(SCI_GETLINECOUNT, 0, 0);
//   int lineStart = SendMessage(SCI_POSITIONFROMLINE, lineCnt-1, 0);
//   int lineEnd   = SendMessage(SCI_GETLINEENDPOSITION, lineCnt-1, 0);
  
  SendMessage(SCI_GOTOLINE, lineCnt-1, 0);

  int lineEnd   = SendMessage(SCI_GETLINEENDPOSITION, lineCnt-1, 0);
  GotoPosition(lineEnd);
  
  return 0;
}

DWORD CEditCtl_i::GotoCurrentLineEnd()
{
  int lineCurrent = SendMessage(SCI_LINEFROMPOSITION, SendMessage(SCI_GETCURRENTPOS, 0, 0), 0) - 1;
  
  // intlineStart = SendMessage(SCI_POSITIONFROMLINE, lineCurrent, 0);
  int lineEnd = SendMessage(SCI_GETLINEENDPOSITION, lineCurrent, 0);
  
  GotoPosition(lineEnd);
  
  return 0;
}

// DWORD CEditCtl_i::GetCurrentLineText(BYTE* pLineBuf, U32& ulSize)
// {
//   int line = SendMessage(SCI_LINEFROMPOSITION, SendMessage(SCI_GETCURRENTPOS, 0, 0), 0) - 1;
//   
//   return GetLine(line, pLineBuf, ulSize);
// }

#if 0 
bool CEditCtl_i::GetCurrentLineText(LPSTR pszLineBuf, U32 ulSize)
{
  SendMessage(SCI_GETCURLINE, (WPARAM)ulSize, (LPARAM)pszLineBuf);
  
  return TRUE;
}
#endif 


bool CEditCtl_i::DeleteCurrentLineText()
{
  int iCurPos  = SendMessage(SCI_GETCURRENTPOS, 0, 0);
  int iCurLine = SendMessage(SCI_LINEFROMPOSITION, (WPARAM) iCurPos, 0);
  
  SendMessage(SCI_LINEDELETE, (WPARAM)0, (LPARAM)0);
  
  return TRUE;
}

ULONG CEditCtl_i::ReplaceCurrentLineText(LPCSTR pszText, U32 szTextLen)
{
  int iCurPos  = SendMessage(SCI_GETCURRENTPOS, 0, 0);
  int iCurLine = SendMessage(SCI_LINEFROMPOSITION, (WPARAM) iCurPos, 0);
  //int iLinePos = iCurPos - SendMessage(SCI_POSITIONFROMLINE, (WPARAM) iCurLine, 0);

  int iStartPos  = SendMessage(SCI_GETLINEENDPOSITION, iCurLine, 0);
  int iEndPos    = SendMessage(SCI_GETLINESELSTARTPOSITION, iCurLine, 0);
  
  if ((int)szTextLen >= iEndPos - iStartPos + 1)
  {
    SendMessage(SCI_SETSEL, iStartPos, iEndPos);
    SendMessage(SCI_REPLACESEL, (WPARAM)0, (LPARAM)pszText);
  }

  return 0;
}

U32 CEditCtl_i::AddText(LPCSTR pszText, U32 szTextLen, bool bScroll)
{
  U32 ret = ERR_OK_I;

  // The current selection is not changed and the new text is not scrolled into view.
  //SendMessage(SCI_SETREADONLY, FALSE, 0);
  
  if(szTextLen && szTextLen)
  {
    ret = SendMessage(SCI_APPENDTEXT, (WPARAM)szTextLen, (LPARAM)pszText);
  }
  
  if(bScroll)
  {
    LRESULT lLines = SendMessage(SCI_GETLINECOUNT, 0, 0);
    GotoLine(lLines);
  }
  //SendMessage(SCI_SETREADONLY, TRUE, 0);

  return ret;
}

ULONG CEditCtl_i::SetEdge(int foreRgb, int backRgb)
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

  return 0;
}


// Max. Len = 1024 bytes.
U32 CEditCtl_i::AddStyledText(LPCSTR pszText, ULONG szTextLen, int style, bool bAppend)
{
  U32 ret = ERR_OK_I;
  char buf[2048+4]; 
  
  if(szTextLen > 1024)
  {
    szTextLen = 1024;
  }
  
  if(style == -1)
  {
    style = 32;
  }

  buf[0] = 0;
  buf[0] = 1;
  
  for (size_t i = 0; i < szTextLen; i++)
  {
    buf[i* 2] = pszText[i];
    buf[i * 2 + 1] = static_cast < char > (style);
  }
  buf[2048+2] = 0;
  buf[2048+3] = 0;
  
  if (bAppend)
  {
    LRESULT lLines = SendMessage(SCI_GETLINECOUNT, 0, 0);
    
    GotoLine(lLines);
  }
  
  ret = SendMessage(SCI_ADDSTYLEDTEXT, (WPARAM)(szTextLen* 2), (LPARAM)(buf));
    
  if(bAppend)
  {
    SendMessage(SCI_DOCUMENTEND, 0, 0);
  }
  
  return ret;
}

ULONG CEditCtl_i::SetWordWrapMode(bool bWrap)
{
  return SendMessage(SCI_SETWRAPMODE,(bWrap)?SC_WRAP_WORD:SC_WRAP_NONE,0);
}

ULONG CEditCtl_i::SelectionStyle(int foreRgb, int backRgb)
{
    SendMessage(SCI_SETSELFORE,TRUE,foreRgb);// selection fore
    SendMessage(SCI_SETSELBACK,TRUE,backRgb); // selection back
   
    //SendMessage(SCI_SETSELFORE,0,0);
    //SendMessage(SCI_SETSELBACK,TRUE,RGB(0xC0,0xC0,0xC0)); // use a default value...

    return 0;
}

// GetSysColor(CaretLineStyle)
ULONG CEditCtl_i::CaretLineStyle(int foreRgb, int backRgb) // RGB((iValue&0xFF0000) >> 16,(iValue&0xFF00) >> 8,iValue&0xFF);
{
    SendMessage(SCI_SETCARETLINEVISIBLE,TRUE,0);// caret line back
    SendMessage(SCI_SETCARETLINEBACK,backRgb,0);
    //SendMessage(SCI_SETCARETLINEVISIBLE,FALSE,0);
    
    SendMessage(SCI_SETCARETFORE,foreRgb,0);
    // SendMessage(SCI_SETCARETFORE,GetSysColor(COLOR_WINDOWTEXT),0); // default value

    return 0;
}

ULONG CEditCtl_i::CaretforeColor(int foreRgb)
{
  return SendMessage(SCI_SETCARETFORE,foreRgb,0);
}

ULONG CEditCtl_i::BookMarkerStyle(UINT iValue)
{
  LPSTR pszXPM;
  if(iValue > 9)
  {
    return ERR_INVALID_PARAM_I;
  }

  iValue = max(min(iValue + 99, IDX_BM8), IDX_BM0);
  pszXPM = LoadXPM(iValue);
  if(pszXPM)
  {
    SendMessage(SCI_MARKERDEFINE, 0, SC_MARK_PIXMAP);
    SendMessage(SCI_MARKERDEFINEPIXMAP, 0, (LPARAM) pszXPM);
    SendMessage(SCI_COLOURISE,0,(LPARAM)-1);
    LocalFree(pszXPM);
  }

  return 0;
}

ULONG  CEditCtl_i::CaretBlinkingTime(DWORD dwMilliseconds) // caret blinking time 
{
  return SendMessage(SCI_SETCARETPERIOD,dwMilliseconds,0);
}

static long LongFromTwoShorts(short a, short b)
{
  return (a) | ((b) << 16);
}

ULONG CEditCtl_i::AssignKey(int key, int mods, int cmd)
{
  return SendMessage(SCI_ASSIGNCMDKEY, LongFromTwoShorts(static_cast <short> (key), static_cast <short> (mods)), cmd);
}

ULONG  CEditCtl_i::Init()
{
  // clear all text styles
  SendMessage(SCI_CLEARDOCUMENTSTYLE, 0, 0);
  
  // set the number of styling bits to 7 - the asp/html views need a lot of styling - default is 5
  // If you leave the default you will see twiggle lines instead of ASP code
  SendMessage(SCI_SETSTYLEBITS, 7, 0);
  
  // set the display for indetation guides to on - this displays virtical dotted lines from the beginning of 
  // a code block to the end of the block
  // SendMessage(SCI_SETINDENTATIONGUIDES, TRUE, 0);
  
  // set tabwidth to 4
  SendMessage(SCI_SETTABWIDTH,4,0);

  // set the caret blinking time to 500 milliseconds
  SendMessage(SCI_SETCARETPERIOD,500,0);

  //SendMessage(SCI_STYLECLEARALL);
 
  // source folding section  
  SendMessage(SCI_STYLESETFORE, STYLE_DEFAULT, (LPARAM) GetSysColor(COLOR_WINDOWTEXT)); // default text color
  SendMessage(SCI_STYLESETBACK, STYLE_DEFAULT, (LPARAM) GetSysColor(COLOR_WINDOW)); // default window color
 
   // set the forground color for some styles
//   SendMessage(SCI_STYLESETFORE, 0, RGB(0,0,0));
//   SendMessage(SCI_STYLESETFORE, 2, RGB(0,64,0));
//   SendMessage(SCI_STYLESETFORE, 5, RGB(0,0,255));
//   SendMessage(SCI_STYLESETFORE, 6, RGB(200,20,0));
//   SendMessage(SCI_STYLESETFORE, 9, RGB(0,0,255));
//   SendMessage(SCI_STYLESETFORE, 10, RGB(255,0,64));
//   SendMessage(SCI_STYLESETFORE, 11, RGB(0,0,0));

  // set the backgroundcolor of brace highlights
  SendMessage(SCI_STYLESETBACK, STYLE_BRACELIGHT, RGB(0,255,0));

  // set end of line mode to CRLF
  SendMessage(SCI_CONVERTEOLS, 2, 0);
  SendMessage(SCI_SETEOLMODE, 2, 0);
  // SendMessage(SCI_SETVIEWEOL, TRUE, 0);
  
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

  //SendMessage(SCI_MARKERDEFINE, 0, SC_MARK_CIRCLE);
  // display all margins
  SetDisplayLinenumbers(TRUE);
  
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
    
  SendMessage(SCI_SETEOLMODE, SC_EOL_CRLF, 0);  
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
  
  //SendMessage(SCI_CMD_MODE,0,0);
  
  // Nonprinting characters
  ShowPrintCharacter(FALSE, FALSE);
  
  AssignKey('C', SCMOD_CTRL, SCI_COPY);
  AssignKey('c', SCMOD_CTRL, SCI_COPY);
  AssignKey('v', SCMOD_CTRL, SCI_PASTE);
  AssignKey('V', SCMOD_CTRL, SCI_PASTE);
  
  //SendMessage(SCI_ASSIGNCMDKEY, ('c' + (SCMOD_CTRL << 16)), SCI_LINECOPY);
  
  //SendMessage(SCI_SETCODEPAGE,  SC_CP_UTF8, 0);
  
  SetStyle( STYLE_DEFAULT, WHITE, BLACK, 10, "Courier New" );
  
  // Set all styles
  SendMessage( SCI_STYLERESETDEFAULT );
  SendMessage( SCI_STYLECLEARALL );

  return 0;
}

ULONG  CEditCtl_i::ShowPrintCharacter(bool bViewWhiteSpace, bool bViewEOLs)
{
  SendMessage(SCI_SETVIEWWS,(bViewWhiteSpace)?SCWS_VISIBLEALWAYS:SCWS_INVISIBLE,0);
  return SendMessage(SCI_SETVIEWEOL,bViewEOLs,0);
}

int CEditCtl_i::GetEncoding()
{
  LRESULT r = SendMessage(SCI_GETCODEPAGE,0,0); 
  return r; 
}


// By default, the vertical scroll bar is always displayed when required. You can choose to hide or show it 
ULONG CEditCtl_i::SetScroll(bool Vs, bool Hs) 
{
  LRESULT ret = SendMessage(SCI_SETVSCROLLBAR, Vs);
  ret = SendMessage(SCI_SETHSCROLLBAR, Hs);
  
  return ret;
}

#include "SciLexer.h"

int CEditCtl_i::SetLexer(int lexer)
{
  if(lexer == CPP_LEXER)
    SendMessage( SCI_SETLEXER, SCLEX_CPP );
  else if(lexer == HTML_LEXER)
    SendMessage( SCI_SETLEXER, SCLEX_HTML );
  else if(lexer == XML_LEXER)
    SendMessage( SCI_SETLEXER, SCLEX_XML );
  else if(lexer == PERL_LEXER)
    SendMessage( SCI_SETLEXER, SCLEX_PERL );
  else if(lexer == LUA_LEXER)
    SendMessage( SCI_SETLEXER, SCLEX_LUA );
  else if(lexer == TCL_LEXER)
    SendMessage( SCI_SETLEXER, SCLEX_TCL );
  else if(lexer == VHDL_LEXER)
    SendMessage( SCI_SETLEXER, SCLEX_VHDL );  
  else if(lexer == PYTHON_LEXER)
    SendMessage( SCI_SETLEXER, SCLEX_PYTHON );  
  else 
    SendMessage( SCI_SETLEXER, lexer );
  
  return 0;
}

int CEditCtl_i::SetKeywords(LPARAM lp)
{
  return SendMessage(SCI_SETKEYWORDS, 0, (LPARAM)lp);
}


int CEditCtl_i::SetCharSet(int iCharSet, int style)
{
  SendMessage(SCI_STYLESETCHARACTERSET, style, iCharSet);
  
  return 0;
}

int CEditCtl_i::Endcoding(int iCodePage)
{
  SendMessage(SCI_SETCODEPAGE, iCodePage, 0);
  
  return 0;
}

bool CEditCtl_i::SetLeftBorder(DWORD dwFolder, DWORD dwMargin)
{
  // Tell scintilla to draw folding lines UNDER the folded line
  SendMessage(SCI_SETFOLDFLAGS, dwFolder,0);
  
  // Set margin 2 = folding margin to display folding symbols
  SendMessage(SCI_SETMARGINMASKN,dwMargin, SC_MASK_FOLDERS);

  return TRUE;
}

U32 CEditCtl_i::GetSelectionStart()
{
   return SendMessage(SCI_GETSELECTIONSTART, 0, 0);
}

U32 CEditCtl_i::GetSelectionEnd()
{
   return SendMessage(SCI_GETSELECTIONEND, 0, 0);
}

U32 CEditCtl_i::GetSelectionText(PVOID pBuffer)
{
  return SendMessage(SCI_GETSELTEXT, 0, (long)pBuffer);
}

U32 CEditCtl_i::GetLength(VOID)
{
  return (U32)SendMessage(SCI_GETLENGTH);
}

U32 CEditCtl_i::GetText(U32 ulBufferSize, BYTE* pBuffer)
{
  return SendMessage(SCI_GETTEXT, ulBufferSize,(long)pBuffer);
}

U32 CEditCtl_i::FindNext(CHAR szFindText[256], U32 ulFlag)
{
  U32 ret        = ERR_OK_I;
  U32 ulFindFlag = 0;
  struct TextToFind ttf = {0, };
  
  if (ulFlag & FINDCASE)
    ulFindFlag |= SCFIND_MATCHCASE;
  if (ulFlag & MATCH_WHOLE_WORD)
    ulFindFlag |= SCFIND_WHOLEWORD;
  if (ulFlag & MATCH_BEGIN_WORD)
    ulFindFlag |= SCFIND_WORDSTART;
  if (ulFlag & REGULAR_EXP)
    ulFindFlag |= SCFIND_REGEXP;
  
  int iPos;
  
  ttf.chrg.cpMin = SendMessage(SCI_GETSELECTIONEND, 0, 0);
  ttf.chrg.cpMax = SendMessage(SCI_GETLENGTH, 0, 0);
  ttf.lpstrText  = szFindText;
  
  iPos = SendMessage(SCI_FINDTEXT, ulFindFlag, (LPARAM) & ttf);
  
  if (iPos == -1 && ttf.chrg.cpMin > 0)
  {
    ttf.chrg.cpMin = 0;
    iPos  = SendMessage(SCI_FINDTEXT, ulFindFlag, (LPARAM) & ttf);
  }
  if (iPos == -1)
  {
    ret = ERR_NOT_FOUND_I;
    return ret;
  }
  
  SendMessage(SCI_SETSEL, ttf.chrgText.cpMin, ttf.chrgText.cpMax);
  
  return ret;
}


U32 CEditCtl_i::FindPrev(CHAR szFindText[256], U32 ulFlag)
{
  U32 ret        = ERR_OK_I;
  U32 ulFindFlag = 0;
  struct TextToFind ttf = {0, };
  
  if (ulFlag & FINDCASE)
    ulFindFlag |= SCFIND_MATCHCASE;
  if (ulFlag & MATCH_WHOLE_WORD)
    ulFindFlag |= SCFIND_WHOLEWORD;
  if (ulFlag & MATCH_BEGIN_WORD)
    ulFindFlag |= SCFIND_WORDSTART;
  if (ulFlag & REGULAR_EXP)
    ulFindFlag |= SCFIND_REGEXP;
  
  int iPos;
  int iLength;
  
  ZeroMemory(&ttf, sizeof(ttf));
  
  ttf.chrg.cpMin = max(0, SendMessage(SCI_GETSELECTIONSTART, 0, 0) - 1);
  ttf.chrg.cpMax = 0;
  ttf.lpstrText  = szFindText;
  
  iPos = SendMessage(SCI_FINDTEXT, ulFindFlag, (LPARAM) & ttf);
  
  iLength = SendMessage(SCI_GETLENGTH, 0, 0);
  if (iPos == -1 && ttf.chrg.cpMin < iLength)
  {
    ttf.chrg.cpMin = iLength;
    iPos           = SendMessage(SCI_FINDTEXT, ulFindFlag, (LPARAM) & ttf);
  }
  
  if (iPos == -1)
  {
    ret = ERR_NOT_FOUND_I;
    return ret;
  }
  
  SendMessage(SCI_SETSEL, ttf.chrgText.cpMin, ttf.chrgText.cpMax);
  
  return ret;
}


bool CEditCtl_i::SetReadOnly(bool bReadOnly)
{
  SendMessage(SCI_SETREADONLY, bReadOnly);

  return TRUE;
}

bool CEditCtl_i::GetReadOnly()
{
  return SendMessage(SCI_GETREADONLY,0,0)?TRUE:FALSE;
}

ULONG CEditCtl_i::SetDisplaySelection(bool bFlag)
{
   m_bSelection = bFlag;
   if (bFlag)
	   SendMessage(SCI_SETMARGINWIDTHN, 1, 16);
   else
	   SendMessage(SCI_SETMARGINWIDTHN, 1, 0);

   return 0;
}

// Toggle the display of the folding margin
//
ULONG CEditCtl_i::SetDisplayFolding(bool bFlag)
{
   m_bFolding = bFlag;
   if (bFlag)
	   SendMessage(SCI_SETMARGINWIDTHN, 2, 16);
   else
	   SendMessage(SCI_SETMARGINWIDTHN, 2, 0);

   return 0;
}

// Set the display of line numbers on or off.
// Scintilla uses 3 "Margin" at the left of the edit window. The Margin (0)
// is used to display the linenumbers. If set to 0 no numbers are displayed. If
// set to a width > 0 - linenumbers are displayed. We use a helper function to 
// calculate the size of this margin.
// int | number of pixels for the margin width of margin (0) 
//
ULONG CEditCtl_i::SetDisplayLinenumbers(bool bFlag) //@parm flag if we shuld display line numbers
{
  // if nothing changes just return
  if (GetDisplayLinenumbers() == bFlag)
    return 0;

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

  return 0;
}

// Try to calculate the number of characters needed for the display of the linenumbers.
// This function returns 3 for 10-99 lines in file, 4 for 100-999 lines and so on.
//
int CEditCtl_i::GetLinenumberChars ()
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
int CEditCtl_i::GetLinenumberWidth ()
{
  // get number of chars needed to display highest linenumber
  int nChars = GetLinenumberChars ()+1;
  // get width of character '9' in pixels
  LRESULT lWidth = SendMessage(SCI_TEXTWIDTH, STYLE_LINENUMBER, (long)_T("9"));
  return nChars * lWidth;
}


ULONG CEditCtl_i::UpdateMarginWidth(VOID)
{
  if (!GetDisplayLinenumbers())
    return 0; 
  
  //int nWidth = GetLinenumberWidth() + 4;
  int nWidth = GetLinenumberWidth();
	SendMessage(SCI_SETMARGINWIDTHN, 0, nWidth);
  
  return 0;
}

// Copy the selected text to the clipboard
ULONG CEditCtl_i::Copy()
{
   return SendMessage(SCI_COPY, 0, 0);
}

ULONG CEditCtl_i::Paste()
{
   return SendMessage(SCI_PASTE, 0, 0);
}

ULONG CEditCtl_i::Clear()
{
   return SendMessage(SCI_CLEAR, 0, 0);
}


ULONG CEditCtl_i::ClearAll()
{
   return SendMessage(SCI_CLEARALL, 0, 0);
}

// Select the complete text
ULONG CEditCtl_i::SelectAll()
{
   return SendMessage(SCI_SELECTALL, 0, 0);
}


// Get the current line number - this the with the caret in it
// @rvalue long | line number with the caret in it - starts with 0
//
long CEditCtl_i::GetCurrentLine()
{
   long lPos = SendMessage(SCI_GETCURRENTPOS, 0, 0);
   return SendMessage(SCI_LINEFROMPOSITION, lPos, 0);
}


// Update UI and do brace matching
ULONG CEditCtl_i::UpdateUI()
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

  return 0;
}

CONST int iLineEndings[3] = { SC_EOL_CRLF,SC_EOL_LF, SC_EOL_CR };

ULONG CEditCtl_i::SetEOLMode(int mode) // 
{
  if(mode > 2)
  {
    return ERR_INVALID_PARAM_I;
  }

  return SendMessage(SCI_SETEOLMODE, iLineEndings[mode], 0 );
}


ULONG CEditCtl_i::AutoIndent(CHAR ch)
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
      
      // intiIndent = SendMessage(SCI_GETLINEINDENTATION,(WPARAM)iCurLine,0);
      // SendMessage(SCI_SETLINEINDENTATION,(WPARAM)iCurLine,(LPARAM)iIndentBefore);
      // iIndentLen= /*- iIndent +*/ SendMessage(SCI_GETLINEINDENTATION,(WPARAM)iCurLine,0);
      // if(iIndentLen > 0)
      //  SendMessage(SCI_SETSEL,(WPARAM)iAnchorPos+iIndentLen,(LPARAM)iCurPos+iIndentLen);
    }
  }

  return 0;
}


ULONG CEditCtl_i::BraceMatch(PCSTR pszMatchString)   // "()[]{}" 
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

  return 0;
}

// Do default folding for given margin and position - usually called from notification handler
ULONG CEditCtl_i::DoDefaultFolding(U32 nMargin,      // maring we want to handle - normally (2)
                                  U32 lPosition)   // character position where user clicked margin
{
  // simply toggle fold
  if (nMargin == 2)
  {
    long lLine = SendMessage(SCI_LINEFROMPOSITION, lPosition, 0);
    SendMessage(SCI_TOGGLEFOLD, lLine, 0);
  }

  return 0;
}

U32 CEditCtl_i::GetNotifyCodeInfo(LPARAM lParam, int& ch, int& margin, int& position)
{
  SCNotification *pMsg = (SCNotification*)lParam;

  ch = pMsg->ch;
  margin = pMsg->margin;
  position=pMsg->position;

  return 0;
}

// Do default folding for given margin and position - usually called from notification handler
ULONG CEditCtl_i::Refresh()
{
   return SendMessage(SCI_COLOURISE, 0, -1);
}

ULONG CEditCtl_i::ClearAllBookmarks()
{
  return SendMessage(SCI_MARKERDELETEALL,(WPARAM)-1,0);
}

ULONG CEditCtl_i::JumpToBookmark(bool bNext)
{
  int iCurrentPos  = SendMessage(SCI_GETCURRENTPOS,0,0);
  int iCurrentLine = SendMessage(SCI_LINEFROMPOSITION,(WPARAM)iCurrentPos,0);
  int iMaxLine     = SendMessage(SCI_GETLINECOUNT,0,0) - 1;
  
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

  return 0;
}

ULONG CEditCtl_i::ToggleBookmark()
{
  int iCurrentPos = SendMessage(SCI_GETCURRENTPOS,0,0);
  int iCurrentLine = SendMessage(SCI_LINEFROMPOSITION,(WPARAM)iCurrentPos,0);
  int iMarkerBits = SendMessage(SCI_MARKERGET,(WPARAM)iCurrentLine,0);

  if (iMarkerBits & 1) // Bit 0 represents the bookmark
    SendMessage(SCI_MARKERDELETE,(WPARAM)iCurrentLine,0);
  else
    SendMessage(SCI_MARKERADD,(WPARAM)iCurrentLine,0);

  return 0;
}


ULONG CEditCtl_i::SetLineMark(int nLine, bool bAdd, u8 nMarkStyle)
{
		SendMessage( SCI_MARKERDEFINE, 0, nMarkStyle ); // SC_MARK_SHORTARROW
    SendMessage( SCI_MARKERSETFORE, 0, RGB( 80, 0, 0 ) );
    SendMessage( SCI_MARKERSETBACK, 0, RGB( 255, 0, 0 ) );
    
    if(bAdd)
    {
      SendMessage(SCI_MARKERADD,(WPARAM)nLine,0); 
    }
    else 
    {
      SendMessage(SCI_MARKERDELETE,(WPARAM)nLine,0);      
    }
    
    return 0;
}

int CEditCtl_i::GetCurrentStyle()
{
   long lPos = SendMessage(SCI_GETCURRENTPOS, 0, 0);
   return SendMessage(SCI_GETSTYLEAT, lPos, 0);
}

/// Sets a Scintilla style
ULONG CEditCtl_i::SetStyle(u8 style, 
                           COLORREF fore, 
                           COLORREF back , 
                           int nFontsizeInPoints , 
                           const char *face,
                           bool bDefaultAttributes)
{	
  ULONG ret = ERR_OK_I;
  
  if(style > 32)
    style = STYLE_DEFAULT;
  
  ret = SendMessage(SCI_STYLESETFORE, style, fore);
  ret = SendMessage(SCI_STYLESETBACK, style, back);
  
  if (nFontsizeInPoints >= 1)
    SendMessage(SCI_STYLESETSIZE, style, nFontsizeInPoints);
  
  if (face) 
    SendMessage(SCI_STYLESETFONT, style, reinterpret_cast<LPARAM>(face));
  
  if(bDefaultAttributes)
  {
    // Set all styles
    SendMessage( SCI_STYLECLEARALL );
  }

  return ret;
}


// set the fontname e.g. "Arial"
ULONG CEditCtl_i::SetFontname(int nStyle, LPCSTR pszFontname)
{
  if(nStyle == -1)
    nStyle = STYLE_DEFAULT;
   SendMessage(SCI_STYLESETFONT, nStyle, (long)pszFontname);

   return 0;
}

// Set the font height in points
ULONG CEditCtl_i::SetFontheight(int nStyle, int nHeight)
{
  if(nStyle == -1)
    nStyle = STYLE_DEFAULT;
   SendMessage(SCI_STYLESETSIZE, nStyle, (long)nHeight);

   return 0;
}

// Set the foreground color
ULONG CEditCtl_i::SetForeground(int nStyle, COLORREF crForeground)
{
  if(nStyle == -1)
    nStyle = STYLE_DEFAULT;
   SendMessage(SCI_STYLESETFORE, nStyle, (long)crForeground);
 
   return 0;
}

// set the backgroundcolor
ULONG CEditCtl_i::SetBackground(int nStyle, COLORREF crBackground)
{
   SendMessage(SCI_STYLESETBACK, STYLE_DEFAULT, (LPARAM)crBackground);// GetSysColor(COLOR_WINDOW)); 
   SendMessage(SCI_STYLESETBACK, nStyle, (long)crBackground);

   return 0;
}

 
// set given style to bold 
ULONG CEditCtl_i::SetBold(int nStyle, bool bBold)
{
  if(nStyle == -1)
    nStyle = STYLE_DEFAULT;
   SendMessage(SCI_STYLESETBOLD, nStyle, (long)bBold);
 
   return 0;
}

// set given style to bold
ULONG CEditCtl_i::SetItalic(int nStyle, bool bItalic)
{
  if(nStyle == -1)
    nStyle = STYLE_DEFAULT;
   SendMessage(SCI_STYLESETITALIC, nStyle, (long)bItalic);
 
   return 0;
}

// set given style to bold
ULONG CEditCtl_i::SetUnderline(int nStyle, bool bUnderline)
{
  if(nStyle == -1)
    nStyle = STYLE_DEFAULT;
   SendMessage(SCI_STYLESETUNDERLINE, nStyle, (long)bUnderline);
 
   return 0;
}

// Return flag if we are in overstrike mode
bool CEditCtl_i::GetOverstrike()
{
  return SendMessage(SCI_GETOVERTYPE , 0, 0)?TRUE:FALSE;
}

// set set overstrike mode (TRUE) or insert mode (FALSE)
ULONG CEditCtl_i::SetOverstrike(bool bOverstrike)
{
   return SendMessage(SCI_SETOVERTYPE, bOverstrike, 0);
}


#define StrEnd(pStart) (pStart + lstrlenA(pStart))
#define COUNTOF(ar) (sizeof(ar)/sizeof(ar[0]))

int CEditCtl_i::FormatNumberStr(LPSTR lpNumberStr)
{
  CHAR *c;
  CHAR szSep[8];
  int  i = 0;

  if (!lstrlenA(lpNumberStr))
    return(0);

  if (!GetLocaleInfoA(LOCALE_USER_DEFAULT,
                     LOCALE_STHOUSAND,
                     szSep,
                     COUNTOF(szSep)))
    szSep[0] = '\'';

  c = StrEnd(lpNumberStr);

  while ((c = CharPrevA(lpNumberStr,c)) != lpNumberStr)
  {
    if (++i == 3)
    {
      i = 0;
      MoveMemory(c+1,c,lstrlenA(c)+1);
      *c = szSep[0];
    }
  }

  return(lstrlenA(lpNumberStr));
}

//  FormatBytes()
//
int CEditCtl_i::FormatBytes(LPSTR lpOutput,int nOutput,DWORD dwBytes)
{
  CHAR tch[256];
  int i;
  double dBytes = dwBytes;
  static const CHAR *pBytes[] = {  ("Bytes"),  ("KB"),  ("MB"),  ("GB") };

  if (dwBytes > 1023)
  {
    for (i = 0; i < 4; i++)
    {

      if (dBytes >= 1024.0)
        dBytes /= 1024.0;

      else
        break;
    }
    wsprintfA(tch, ("%.2f"),dBytes);
    GetNumberFormatA(LOCALE_USER_DEFAULT,0,tch,NULL,lpOutput,nOutput);
    lstrcatA(lpOutput, (" "));
    lstrcatA(lpOutput,pBytes[i]);
  }

  else
  {
    wsprintfA(lpOutput,  ("%i"),dwBytes);
    FormatNumberStr(lpOutput);
    lstrcatA(lpOutput,  (" "));
    lstrcatA(lpOutput,pBytes[0]);
  }

  return 0;
}


ULONG CEditCtl_i::NotifyTextChanged()
{
  return 0;
  
}

ULONG CEditCtl_i::NotifyDoubleClick()
{
  return 0;
}

ULONG CEditCtl_i::NotifyNewLine(CHAR* pTxt)
{
  return 0;
}

ULONG CEditCtl_i::AddChar(CHAR ch)
{
  return 0;
}

bool CEditCtl_i::UiNotify(WPARAM wParam,       //  not used
                      LPARAM lParam,       // pointer to notification structure
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

      //       user clicked margin - try folding action
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
    case SCN_KEY_DOWN:
      {
        BOOL bKeyDown = (::GetKeyState(pMsg->ch) & 0x80000000);
        
        int key       = pMsg->ch;
        char c        = (char)key;
        int modifiers = pMsg->modifiers;
        
        if(m_bBookMark)
        {
          //ITS_TRACEA("%d: key = %x modifier:%d \n", GetTickCount(), key, modifiers);
          if ((modifiers & SCMOD_CTRL) && (modifiers & SCMOD_SHIFT) && (key == VK_F2))
          {
            ClearAllBookmarks();
          }
          else if (bKeyDown && (modifiers & SCMOD_CTRL) && (key == VK_F2))
          {
            if(!GetDisplaySelection())
            {
              SetDisplaySelection(TRUE);
            }
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
       
        if(m_bFind)
        {
          if (bKeyDown && (modifiers & SCMOD_CTRL) && (key == VK_F3))
          {
            //ITS_TRACEA("CTRL+F3\n");
          }
          else if (bKeyDown && key == VK_F3)
          {
            //ITS_TRACEA("F3 \n");
          }
        }
        
#if 0 
        if (m_bCtrlc)
        {
          if ((modifiers & SCMOD_CTRL) && (key == 'C' || key == 'c'))
          {
            //ITS_TRACEA("CTRL+C %d\n", bKeyDown);
            Copy();
          }
        }
#endif 
      }
      
      break;
    }
    return TRUE; // we processed the message
  }

  return TRUE;
}


ULONG CEditCtl_i::EnableFind(bool bFind)
{
   m_bFind        = bFind;
   return 0;
}

ULONG CEditCtl_i::EnableBookMark(bool bMark)
{
   m_bBookMark    = bMark;
   return 0;
}

ULONG CEditCtl_i::EnableCtrlc(bool bCtrlc)
{
   m_bCtrlc    = bCtrlc;
   return 0;
}
