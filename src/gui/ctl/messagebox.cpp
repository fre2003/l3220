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

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include "StdAfx.h"
#include <types_i.h>
#include <windows.h>
#include "tchar.h"
#include "Shlwapi.h"
#include <gui_i.h>
 
#define COUNTOF(array) (sizeof(array)/sizeof(array[0]))

#define MESSAGE_BOX_VALUE_INVALID 0xFFFFFFFF

#if 0 
main()
{
  UINT32 r =  MessageBox_i(NULL,  
    _T("Message\r\nDisplay it"), 
    _T("Captain"),
    MB_TOPMOST|MB_ICONSTOP, // Style
    TRUE,  // check box 
    5,     // time out
    MB_DEFBUTTON2, // Specify the default button to display time out info.
    NULL,  // customized Icon 
    _T("OK\nCancel\nDon't Display it"), // Button Name
    _T("Identifier100"), // Identifier for CheckBox
    RGB(255,0,0), RGB(255,255,255), NULL, 10 
    );
  
  if(MSGBOX_BUTTON_ID(r) == MSGBOX_BUTTON_ID1) //  button id
  {
    printf("MSG_BUTTON_ID1  ");
    if(IS_MSGBOX_CHECKBOX(r))
    {
      printf("Check Box \n");
    } 
    else 
    {
      printf("\n");
    }
  }
  if(MSGBOX_BUTTON_ID(r) == MSGBOX_BUTTON_ID2) //  button id
  {
    printf("MSG_BUTTON_ID2  ");
    if(IS_MSGBOX_CHECKBOX(r))
    {
      printf("Check Box \n");
    } 
    else 
    {
      printf("\n");
    }
  }
  if(MSGBOX_BUTTON_ID(r) == MSGBOX_BUTTON_ID3) //  button id
  {
    printf("MSG_BUTTON_ID3  ");
    if(IS_MSGBOX_CHECKBOX(r))
    {
      printf("Check Box \n");
    } 
    else 
    {
      printf("\n");
    }
  }
}
#endif
     
static DWORD ReadRegistry(LPCTSTR lpszKey)
{
  if (!lpszKey || lpszKey[0] == _T('\0'))
    return 0;
  
  TCHAR * szRegPath = _T("Software\\MessageBox_i\\");
 
  // open the registry event source key
  DWORD dwData = 0;
  HKEY hKey = NULL;
  LONG lRet = ::RegOpenKeyEx(HKEY_CURRENT_USER, szRegPath, 0, KEY_READ, &hKey);
  
  if (lRet == ERROR_SUCCESS)
  {
    // registry key was opened or created -
    // === write EventMessageFile key ===
    DWORD dwType = 0;
    DWORD dwSize = sizeof(DWORD);
    lRet = ::RegQueryValueEx(hKey, lpszKey, 0, &dwType, (LPBYTE) &dwData, &dwSize);
    
    ::RegCloseKey(hKey);
    
    if (lRet != ERROR_SUCCESS)
      dwData = 0;
  }
  else
  {
    dwData = 0;
  }
  
  return dwData;
}

static void WriteRegistry(LPCTSTR lpszKey, DWORD dwData)
{
  if (!lpszKey || lpszKey[0] == _T('\0'))
    return;
  
  TCHAR * szRegPath = _T("Software\\MessageBox_i\\");
  
  // open the registry key
  DWORD dwResult = 0;
  HKEY hKey = NULL;
  LONG lRet = ::RegCreateKeyEx(HKEY_CURRENT_USER, szRegPath, 0, NULL,REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwResult);
  
  if (lRet == ERROR_SUCCESS)
  {
    // registry key was opened or created -
    
    ::RegSetValueEx(hKey, lpszKey, 0, REG_DWORD, (const BYTE *) &dwData, sizeof(DWORD));
    
    ::RegCloseKey(hKey);
  }
  
  return;
}


static TCHAR* g_pszOfficeFontName		  = _T("Tahoma");
static TCHAR* g_pszOffice2007FontName = _T("Segoe UI");

static int CALLBACK FontFamalyProcFonts (const LOGFONT FAR* lplf,
									const TEXTMETRIC FAR* /*lptm*/,
									ULONG /*ulFontType*/,
									LPARAM lParam)
{
  if(lstrcmpi(lplf->lfFaceName, (LPCTSTR) lParam) == 0)
    return 0;
  else 
    return 1;
  
  // a nonzero value to continue enumeration; to stop enumeration, it must return zero.
}

static void GetDefaultFont(HDC hDc, LOGFONT &lf )
{
  // Check for font existance:
  if (::EnumFontFamilies (hDc, NULL, FontFamalyProcFonts, 
    (LPARAM)(LPCTSTR) g_pszOffice2007FontName) == 0)
  {
    // Found.
    StrCpyN(lf.lfFaceName, g_pszOffice2007FontName, LF_FACESIZE);
    lf.lfQuality = 5; // CLEARTYPE_QUALITY 
  }
  else if (::EnumFontFamilies (hDc, NULL, FontFamalyProcFonts, 
    (LPARAM)(LPCTSTR) g_pszOfficeFontName) == 0)
  {
    // Found.
    StrCpyN(lf.lfFaceName, g_pszOfficeFontName, LF_FACESIZE);
    lf.lfQuality = 5; // CLEARTYPE_QUALITY 
  } 
}

// If pszFaceName is an empty string, GDI uses the first font that matches the other specified attributes. 
static HFONT CreatFont_i(LPCTSTR pszFaceName, INT32 nFontSize, BOOL bBold, BOOL bItalic)
{
  HFONT hFont = NULL;
  LOGFONT lf;
  HDC hDc = ::GetDC(NULL);
  
  memset (&lf, 0, sizeof (LOGFONT));
  lf.lfHeight  = -MulDiv(nFontSize, GetDeviceCaps(hDc, LOGPIXELSY), 72);
  lf.lfCharSet = DEFAULT_CHARSET;
  lf.lfQuality = 5; // CLEARTYPE_QUALITY;
  lf.lfWeight  = bBold?FW_BOLD:FW_REGULAR;
  lf.lfItalic  = bItalic?TRUE:FALSE;
  
  if (pszFaceName == NULL)
  {
		  GetDefaultFont(hDc, lf);
  }
  else 
  {
		  if (::EnumFontFamilies (hDc, NULL, FontFamalyProcFonts, 
        (LPARAM)(LPCTSTR) pszFaceName) == 0)
      {
        // Found.
        StrCpyN(lf.lfFaceName, pszFaceName, LF_FACESIZE);
        lf.lfQuality = 5; // CLEARTYPE_QUALITY 
      }
      else 
      {
        GetDefaultFont(hDc, lf);
      }
  }
  
  hFont = CreateFontIndirect(&lf); 
  
  return hFont;
}


class CRectX : public tagRECT
{
public:
	CRectX() {}

	// retrieves the width
	int Width() const	{ return right - left; }

	// returns the height
	int Height() const	{ return bottom - top; }

  // Operations

	// set rectangle from left, top, right, and bottom
	void SetRect(int x1, int y1, int x2, int y2)
	{
		::SetRect(this, x1, y1, x2, y2);
	}
};


#define DEFAULT_BUTTON_TXT_LEN 32 
#define CHECKBOXHEIGHT         10		// dialog units

enum EN_CONTROL_TYPE { ICON = 0x7F, BUTTON, EDITCONTROL, STATICTEXT, CHECKBOX };

BOOL m_bRightJustifyButtons = FALSE;

enum { IconControlId    = 1000, MessageControlId    = 1001};
enum
{
	ButtonWidth             = 38,		// dialog units 
	ButtonTimeoutExtraWidth = 13,		// dialog units 
	ButtonHeight            = 12,		// dialog units 
	ButtonSpacing           = 6,
	BottomMargin            = 12,
	SpacingBetweenMessageAndButtons = 5,

	// if you change the value for MaxItems, make sure that the code 
	MaxItems                = 8,	// max no. of items in the dialog
	MinimalHeight           = 70,
	SpacingSize             = 8,
};

// following 4 ids MUST be sequential
#define IDCUSTOM1			23
#define IDCUSTOM2			24
#define IDCUSTOM3			25
#define IDCUSTOM4			26 // use as chekbox button

#define IDCHECKBOX    IDCUSTOM4 

enum EN_IDCUST_IDX
{
  IDCUSTOM1_ID = 0,
  IDCUSTOM2_ID,
  IDCUSTOM3_ID,
  IDCHECKBOX_ID,
  IDCUST_NUM
};

#define MB_TIMEOUT			        0x80000000L	// returned if timeout expired


class CDlgBoxItem
{
public:
	DLGITEMTEMPLATE m_dlgItemTemplate;
  int m_nButton;
  EN_CONTROL_TYPE m_controltype;
  TCHAR* m_pszCaption;
  UINT m_nDefaultButtonId;

  void AddItem( EN_CONTROL_TYPE ctrltype,
    UINT nID,
    CRectX* prect,
    LPCTSTR lpszCaption)
  {
    short hidbu = HIWORD(GetDialogBaseUnits());
    short lodbu = LOWORD(GetDialogBaseUnits());

    m_controltype = ctrltype;
    if (m_controltype == CHECKBOX)
    	m_controltype = BUTTON;
    
    if (prect != NULL)
    {
      m_dlgItemTemplate.x = (short)((prect->left * 4) / lodbu);
      m_dlgItemTemplate.y = (short)((prect->top * 8) / hidbu);
      m_dlgItemTemplate.cx = (short)((prect->Width() * 4) / lodbu);
      m_dlgItemTemplate.cy = (short)((prect->Height() * 8) / hidbu);
    }
    else
    {
      m_dlgItemTemplate.x = 0;
      m_dlgItemTemplate.y = 0;
      m_dlgItemTemplate.cx = 10; // some useless default
      m_dlgItemTemplate.cy = 10;
    }
    
    m_dlgItemTemplate.dwExtendedStyle = 0;
    m_dlgItemTemplate.id = (WORD)nID;
    
    switch (ctrltype)
    {
    case ICON:
      m_dlgItemTemplate.style = WS_CHILD | SS_ICON | WS_VISIBLE;
      break;
    
    case BUTTON:
      m_nButton ++;
      m_dlgItemTemplate.style = WS_VISIBLE | WS_CHILD | WS_TABSTOP;
      if (nID == m_nDefaultButtonId)
      {
        m_dlgItemTemplate.style |= BS_DEFPUSHBUTTON;
      }
      else
      {
        m_dlgItemTemplate.style |= BS_PUSHBUTTON;
      }
      break;
      
    case CHECKBOX:
      m_dlgItemTemplate.style = WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_AUTOCHECKBOX;
      break;
      
    case EDITCONTROL:
      m_dlgItemTemplate.style = WS_CHILD | WS_VISIBLE | ES_READONLY | WS_TABSTOP | ES_MULTILINE | ES_LEFT;
      break;
      
    case STATICTEXT:
      m_dlgItemTemplate.style = WS_CHILD | WS_VISIBLE | SS_LEFT;
      break;
    
    default:
      break;
    }
    
    int stringLength = (int)(lpszCaption ? lstrlen(lpszCaption) : 0);
    m_pszCaption = new TCHAR [stringLength + 1];
    lstrcpy(m_pszCaption, lpszCaption ? lpszCaption : _T(""));    
  }
  
  CDlgBoxItem() 
  {
    m_nDefaultButtonId = 0;
    m_pszCaption = NULL;
  };
  
  ~CDlgBoxItem()
  {
    if (m_pszCaption)
		delete [] m_pszCaption;
  };
};

// Add CheckBox
void AddCheckBox(int& x, 
                 int& y, 
                 CRectX& rect, 
                 CRectX& mbrect, 
                 CRectX& buttonrow, 
                 CRectX& checkboxrect,
                 LPCTSTR lpszButtonCaption,
                 int m_nButtonHeight,
                 int m_nDoNotAskAgainHeight,
                 CDlgBoxItem m_pDlgItemArray[MaxItems], 
                 DLGTEMPLATE& m_dlgTempl)
{
  x = 2 * ButtonSpacing + 5;
  y += m_nButtonHeight + (2 * ButtonSpacing);
  
  int w = checkboxrect.Width();
  
  SetRect(&rect, x, y, x + w, y + m_nDoNotAskAgainHeight);
  m_pDlgItemArray[m_dlgTempl.cdit].AddItem(CHECKBOX, IDCHECKBOX, &rect, lpszButtonCaption);          
  m_dlgTempl.cdit++;
  
  buttonrow.bottom = y + m_nDoNotAskAgainHeight;
  mbrect.bottom = buttonrow.bottom + SpacingSize;
  if (mbrect.Width() < (x + w))
    mbrect.right = mbrect.left + x + w;
}

static INT_PTR CALLBACK MsgBoxDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM /*lParam*/);

class CDlgBoxEntry
{
public:
  HBRUSH m_hBackgroundBrush;
  UINT		m_nMessageId;		// message static control id
  UINT    m_nStyle;
  COLORREF m_crText; 	   // message text color 
  COLORREF m_crBackground;  // message background
  CRectX m_msgrect;			// message rect for painting
  HFONT m_hFont;
  UINT		m_nMaxID;			// max control id (one more)
  int m_Options;
  int m_X;
  int m_Y;
  UINT m_nDefaultButtonId;
  BOOL m_bCheckBox;
  BOOL m_bNeedCheck;
  HICON m_hIcon;
  int m_nTimeoutSeconds;
  BOOL m_bShutdown;
  int m_nReturnValue;
  int m_nReturnValueCheckBox;
  TCHAR m_szDefaultButton [DEFAULT_BUTTON_TXT_LEN];
  TCHAR m_szIdentifier [80];
  
  CDlgBoxEntry()
  {
    m_nStyle = 0;
    m_bCheckBox = FALSE;
    m_nReturnValueCheckBox = FALSE;
    m_bShutdown = FALSE;
    m_nTimeoutSeconds = 0;
    m_hIcon = NULL;
    m_X = 0;
    m_Y = 0;
    m_crText = MESSAGE_BOX_VALUE_INVALID;
    m_crBackground = MESSAGE_BOX_VALUE_INVALID;
    m_hBackgroundBrush = NULL;
    m_nMessageId       = 0;
    m_hFont            = NULL;
    m_Options = 0;
    m_nReturnValue = -1;
    m_szDefaultButton[0] = _T('\0');
  };
};

#define   BUTTONS_TEXT_MAX_LEN  4*32 
#define   CAPTION_TEXT_MAX_LEN  128 

// 
// EXAMPLES: 
// m_hWnd,  _T("Message \r\nDisplay it"), _T("Captain"),MB_DEFBUTTON2, FALSE, TRUE, 
// 5, NULL, _T("OK\nCancel"), 
// RGB(255,0,0), RGB(255,255,255), NULL, 10 
// 
// Return Value;
// IDOK, IDCANCEL, IDCUSTOM1, IDCUSTOM2, IDCUSTOM3
u32  MessageBox_i(HWND hWnd, // if NULL, use the active window
                  LPCTSTR pszText,    // Pointer to a null-terminated string that contains the message to display. 
                  LPCTSTR pszCaption, // Pointer to a null-terminated string that is used in the title bar of the message box. 
                  UINT nStyle,  // MB_TOPMOST, MB_SETFOREGROUND, MB_SYSTEMMODAL,MB_ICONSTOP,MB_ICONINFOMATION, MB_ICONINFORMATION,MB_ICONWARNING,MB_ICONERROR
                  bool bCheckBox, // if need check box, return IDCHECKBOX 
                  u32 ulDisplayTimeOut, // 
                  UINT uDefaultButton, // causes specific info(like timeout) to display on, from MB_DEFBUTTON1 to MB_DEFBUTTON3 
                  HICON  hIcon, // if NULL use system default icon.
                  TCHAR szButtons[BUTTONS_TEXT_MAX_LEN], // button string, seprate by '\n' like _T("OK\nCancel"), the last button is checkbox button if bCheckBox=TRUE.
                  LPCTSTR pszIdentifier, // = NULL, used for CheckBox if need and save to registry and check it next time.
                  COLORREF crText, // MESSAGE_BOX_VALUE_INVALID		   // message text color 
                  COLORREF crBackground, // =MESSAGE_BOX_VALUE_INVALID  // message background
                  LPCTSTR pszFaceName, // NULL 
                  INT32 nFontSize // =10
                  )
{
  UINT32 ret = 0;
  CDlgBoxItem m_pDlgItemArray[MaxItems];
  HWND m_hWndOwner = hWnd;		// handle of owner window 
  TCHAR szButtonTxtSeq[IDCUST_NUM][DEFAULT_BUTTON_TXT_LEN] = {{_T("")}, };
  TCHAR szCheckButtonTxt[DEFAULT_BUTTON_TXT_LEN];
  
  szCheckButtonTxt[0] = _T('\0');

  // check input parameters.
  if (szButtons[0] == _T('\0'))
  {
    return -1;
  }
  
  CDlgBoxEntry* pDlgBox = new CDlgBoxEntry;
  if(pDlgBox == NULL)
  {
    return -2;
  }
  
  pDlgBox->m_bNeedCheck = FALSE;

  if(pszIdentifier)
  {
    DWORD dwData = ReadRegistry(pszIdentifier);
    
    // Note:  dwData will be 0 if either ReadRegistry or 
    // GetPrivateProfileString fail to find key
    
    if (LOWORD(dwData) == IDCHECKBOX)
    {
      pDlgBox->m_bNeedCheck = HIWORD(dwData);
    }
  }

  // Save to process in MessageBoxProc
  pDlgBox->m_crText       =  crText;
  pDlgBox->m_crBackground = crBackground;
  pDlgBox->m_nStyle       = nStyle;
  
  if(pszIdentifier)
  {
    lstrcpyn(pDlgBox->m_szIdentifier, pszIdentifier, COUNTOF(pDlgBox->m_szIdentifier));
  }
  
  if (crBackground == MESSAGE_BOX_VALUE_INVALID)
 			crBackground = ::GetSysColor(COLOR_WINDOW);
  if (crBackground != MESSAGE_BOX_VALUE_INVALID)
    pDlgBox->m_hBackgroundBrush = ::CreateSolidBrush(crBackground);
  
 	// translate dialog units to pixels 
  int nBaseunitX = LOWORD(GetDialogBaseUnits());
  int nBaseunitY = HIWORD(GetDialogBaseUnits());
  
  int m_nButtonWidth  = MulDiv(ButtonWidth, nBaseunitX, 4); // button width in pixels
  int m_nButtonHeight = MulDiv(ButtonHeight, nBaseunitY, 8);
  int m_nButtonTimeoutExtraWidth = MulDiv(ButtonTimeoutExtraWidth, nBaseunitX, 4); // timeout button extra width in pixels
  int m_nDoNotAskAgainHeight     = MulDiv(CHECKBOXHEIGHT, nBaseunitY, 8); // checkbox height in pixels
    
  // Specify the default button to change dispaly text for timeout
  switch (uDefaultButton)
  {
		case MB_DEFBUTTON1 : pDlgBox->m_nDefaultButtonId = IDCUSTOM1; break;
    case MB_DEFBUTTON2 : pDlgBox->m_nDefaultButtonId = IDCUSTOM2; break;
    case MB_DEFBUTTON3 : pDlgBox->m_nDefaultButtonId = IDCUSTOM3; break;
    case MB_DEFBUTTON4 : pDlgBox->m_nDefaultButtonId = IDCHECKBOX; break;
    default:             pDlgBox->m_nDefaultButtonId = 0; break;
  }
  
  // m_szDefaultButton is used to save text for timeout option
  pDlgBox->m_szDefaultButton[0] = _T('\0');

  // get dc for drawing
  HDC hdc = ::CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
  
  pDlgBox->m_hFont = CreatFont_i(pszFaceName, nFontSize, FALSE, FALSE);
  
  HFONT hOldFont = (HFONT)::SelectObject(hdc, pDlgBox->m_hFont);
  
  int button_width = m_nButtonWidth;
  
  pDlgBox->m_nTimeoutSeconds = ulDisplayTimeOut;
    
  int nMaxWidth = (::GetSystemMetrics(SM_CXSCREEN) / 2) + 80; 
  if (nStyle & MB_ICONMASK)
    nMaxWidth -= GetSystemMetrics(SM_CXICON) + 2*SpacingSize;
  SetRect(&pDlgBox->m_msgrect, 0, 0, nMaxWidth, nMaxWidth);
  
  // get output size of message text
  ::DrawText(hdc, pszText, -1, &pDlgBox->m_msgrect, DT_LEFT | DT_NOPREFIX |
    DT_WORDBREAK | DT_CALCRECT | DT_EXPANDTABS); 
  
  int nMessageHeight = pDlgBox->m_msgrect.Height();
  
  // get height of a single line
  SIZE nLineSize;
  ::GetTextExtentPoint32(hdc, _T("My"), 2, &nLineSize);	
  
  pDlgBox->m_msgrect.right  += 12;
  pDlgBox->m_msgrect.bottom += 5;
  
  pDlgBox->m_msgrect.left   = 2 * SpacingSize;
  pDlgBox->m_msgrect.top     = 2 * SpacingSize;
  pDlgBox->m_msgrect.right  += 2 * SpacingSize;
  pDlgBox->m_msgrect.bottom += 3 * SpacingSize;
  
  // client rect
  CRectX mbrect;
  SetRect(&mbrect, 0, 0,pDlgBox->m_msgrect.Width() + (2 * SpacingSize), pDlgBox->m_msgrect.Height() + (2 * SpacingSize));
  if (mbrect.Height() < MinimalHeight)
    mbrect.bottom = MinimalHeight;
  
  ///////////////////////////////////////////////////////////////////////////
  DLGTEMPLATE m_dlgTempl = {0, };			// message box dialog template
  
  // initialize the DLGTEMPLATE structure
  m_dlgTempl.x = 0;
  m_dlgTempl.y = 0;
  
  m_dlgTempl.cdit = 0;
  
  m_dlgTempl.style = WS_CAPTION | WS_VISIBLE | WS_SYSMENU | WS_POPUP | DS_MODALFRAME | DS_CENTER;
  m_dlgTempl.dwExtendedStyle = 0;
  
  if (nStyle & MB_SYSTEMMODAL)
    m_dlgTempl.style |= DS_SYSMODAL;
  
  if(hIcon == NULL)
  {
    if (nStyle & MB_ICONMASK)
    {
      LPTSTR lpIcon = (LPTSTR)IDI_EXCLAMATION;
      
      switch (nStyle & MB_ICONMASK)
      {
      case MB_ICONEXCLAMATION: lpIcon = (LPTSTR)IDI_EXCLAMATION; break;
      case MB_ICONHAND:        lpIcon = (LPTSTR)IDI_HAND;        break;
      case MB_ICONQUESTION:    lpIcon = (LPTSTR)IDI_QUESTION;    break;
      case MB_ICONASTERISK:    lpIcon = (LPTSTR)IDI_ASTERISK;    break;
      }
      
      if (lpIcon)
        pDlgBox->m_hIcon = ::LoadIcon(NULL, lpIcon);
    }
  }
  else 
  {
    pDlgBox->m_hIcon = hIcon;
  }
  
  // Set Icon 
  if (pDlgBox->m_hIcon)
  {
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    
    int icon_x = SpacingSize; 
    int icon_y = SpacingSize; 
    
    pDlgBox->m_msgrect.left  += cxIcon + icon_x;
    pDlgBox->m_msgrect.right += cxIcon + icon_x;
    
    mbrect.right = pDlgBox->m_msgrect.right + SpacingSize; 
    CRectX iconrect;
    
    SetRect(&iconrect, icon_x, icon_y, icon_x + cxIcon + 2, icon_y + cyIcon + 2);
    
    m_pDlgItemArray[m_dlgTempl.cdit++].AddItem(STATICTEXT, IconControlId, &iconrect, _T("")); // Set ICON control.
  }
  
  // Mesage control 
  m_pDlgItemArray[m_dlgTempl.cdit++].AddItem(STATICTEXT /*EDITCONTROL*/, MessageControlId, &pDlgBox->m_msgrect, pszText); 
  pDlgBox->m_nMessageId = MessageControlId;
    
  // process buttons
  int cItems = 0;
  int nWidthCustomButtons = 0; 
  TCHAR* pszLastBtn = NULL;

  // process custom buttons
  
  TCHAR szCustomButtons[BUTTONS_TEXT_MAX_LEN];
  lstrcpyn(szCustomButtons, szButtons, BUTTONS_TEXT_MAX_LEN);
  
  int i = 0;
  TCHAR * cp = _tcstok(szCustomButtons, _T("\n"));
  while (cp != NULL)
  {
    // Find the next token in a string.    
    if(i < IDCUST_NUM)
    {
      szButtonTxtSeq[i][0] = _T('\0');
      lstrcpyn(szButtonTxtSeq[i], cp, DEFAULT_BUTTON_TXT_LEN);
    }
    
    cp = _tcstok(NULL, _T("\n"));

    i++;
  }
  
  cItems = i;
  cItems = min(cItems, IDCUST_NUM);
  
  pDlgBox->m_bCheckBox = bCheckBox;
  pDlgBox->m_nReturnValueCheckBox = FALSE;

  if(bCheckBox)
  {
    szCheckButtonTxt[0] = _T('\0');

    int k = cItems-1;
    if(k>=0 && k<IDCUST_NUM)
    {
      lstrcpyn(szCheckButtonTxt, szButtonTxtSeq[k], sizeof(szCheckButtonTxt));
      szButtonTxtSeq[k][0] = _T('\0');
    }
  }

  // Calculate Button width.
  i = 0; 
  while ( (i<IDCUST_NUM) && (szButtonTxtSeq[i][0] != _T('\0')) )
  {
    // Find the next token in a string.
    
    SIZE size;
    ::GetTextExtentPoint32(hdc, szButtonTxtSeq[i], lstrlen(szButtonTxtSeq[i]), &size); 
    
    int w = size.cx + 20;
    w = (w > button_width) ? w : button_width;
    
    // allow for wider buttons if timeout specified
    if (ulDisplayTimeOut)
      w += m_nButtonTimeoutExtraWidth;
    
    nWidthCustomButtons += w;
    
    i++;
  }
  i = 0; 

  CRectX iconrect;
  SetRect(&iconrect, 0, 0, 0, 0);
  
  int y = 0;
  int x = 0;
  CRectX buttonrow;
  
  y = (pDlgBox->m_msgrect.bottom > iconrect.bottom) ? pDlgBox->m_msgrect.bottom : iconrect.bottom;
  y += SpacingBetweenMessageAndButtons;
  
  if (pDlgBox->m_hIcon || pDlgBox->m_hBackgroundBrush)
    y += 9;
  
  int nTotalButtonWidth = nWidthCustomButtons + (ButtonSpacing * (cItems-1));
  SetRect(&buttonrow, 0, y, nTotalButtonWidth, y + m_nButtonHeight);
  
  int nCheckBoxWidth = 0;

  // get checkbox width
  if (szCheckButtonTxt[0] != _T('\0'))
  {
    nCheckBoxWidth = m_nDoNotAskAgainHeight;	// use height as width of box
    
    SIZE size = {0, };
		::GetTextExtentPoint32(hdc, szCheckButtonTxt,  lstrlen(szCheckButtonTxt), &size); 
    
		int w = size.cx + 20;
		w = (w > button_width) ? w : button_width; 
    
    nCheckBoxWidth += w;
  }
  
  mbrect.bottom = buttonrow.bottom + BottomMargin;
  
  int bw     = buttonrow.Width();
  int bleft  = 2 * SpacingSize;
  int bright = bleft + bw;
  
  if (mbrect.right <= (bright + (2 * SpacingSize)))
    mbrect.right = bright + (2 * SpacingSize);
  
  if (mbrect.Width() < nCheckBoxWidth)
    mbrect.right = bleft + nCheckBoxWidth;
  
  x = (mbrect.Width() - bw) / 2;
  y = buttonrow.top;
  
  if (m_bRightJustifyButtons)
  {
    x = mbrect.right - nTotalButtonWidth - 2*SpacingSize; 
  }
  
  CRectX rect;
  
  //////////////////////////////////////////////////////////////////////////
  // Add buttons  
  i = 0;  
  while ( (i<IDCUST_NUM) && (szButtonTxtSeq[i][0] != _T('\0')) )
  {
    SIZE size;
    ::GetTextExtentPoint32(hdc, szButtonTxtSeq[i], lstrlen(szButtonTxtSeq[i]), &size); 
    
    int w = size.cx + 20;
    
    w = (w > button_width) ? w : button_width;
    
    // allow for wider buttons if timeout specified
    if (ulDisplayTimeOut)
      w += m_nButtonTimeoutExtraWidth;
    
    rect.SetRect(x, y, x + w, y + m_nButtonHeight);
    m_pDlgItemArray[m_dlgTempl.cdit].AddItem(BUTTON, IDCUSTOM1 + i, &rect, szButtonTxtSeq[i]);          
    m_dlgTempl.cdit++;
    
    x += w + ButtonSpacing;
    i++;
  }
  
  ///////////////////////////////////////////////////////////////////////////
  // add checkbox
  if (szCheckButtonTxt[0] != _T('\0')) 
  {
    CRectX checkboxrect;
    SetRect(&checkboxrect, 0, 0, nCheckBoxWidth, m_nDoNotAskAgainHeight);
    AddCheckBox(x, y, rect, mbrect, buttonrow, checkboxrect, szCheckButtonTxt,
      m_nButtonHeight, m_nDoNotAskAgainHeight,m_pDlgItemArray,m_dlgTempl);
  }
  
  if (buttonrow.bottom >= mbrect.bottom)
    mbrect.bottom = buttonrow.bottom + (2 * SpacingSize);
  
  if (mbrect.right < (buttonrow.right + (2 * SpacingSize)))
    mbrect.right = buttonrow.right + (2 * SpacingSize);
  
  m_dlgTempl.x = 0;
  m_dlgTempl.y = 0;
  m_dlgTempl.cx = (short)((mbrect.Width() * 4) / nBaseunitX);
  m_dlgTempl.cy = (short)((mbrect.Height() * 8) / nBaseunitY);
  
  ::SelectObject(hdc, hOldFont);
  ::DeleteDC(hdc);
  
  /////////////////////////////////////////////////////////////////////////
  // The first step is to allocate memory to define the dialog. 
  // The information to be stored in the allocated buffer is the following:
  //
  // 1. DLGTEMPLATE structure
  //    typedef struct
  //    {
  //       DWORD style;
  //       DWORD dwExtendedStyle;
  //       WORD cdit;
  //       short x;
  //       short y;
  //       short cx;
  //       short cy;
  //    } DLGTEMPLATE;
  // 2. 0x0000 (Word) indicating the dialog has no menu
  // 3. 0x0000 (Word) Let windows assign default class to the dialog
  // 4. (Caption) Null terminated unicode string
  // 5. 0x000B (size of the font to be used)
  // 6. "MS Sans Serif" (name of the typeface to be used)
  // 7. DLGITEMTEMPLATE structure for the button (HAS TO BE DWORD ALIGNED)
  //    typedef struct
  //    {
  //       DWORD style;
  //       DWORD dwExtendedStyle;
  //       short x;
  //       short y;
  //       short cx;
  //       short cy;
  //       WORD id;
  //    } DLGITEMTEMPLATE;
  // 8. 0x0080 to indicate the control is a button
  // 9. (Title). Unicode null terminated string with the caption
  // 10. 0x0000 0 extra bytes of data for this control
  // 11. DLGITEMTEMPLATE structure for the Static Text (HAS TO BE DWORD ALIGNED)
  // 12. 0x0081 to indicate the control is static text
  // 13. (Title). Unicode null terminated string with the text
  // 14. 0x0000. 0 extra bytes of data for this control
  //////////////////////////////////////////////////////////////////////////

  int nTitleLen = (int)lstrlen(pszCaption);
  int nBufferSize = sizeof(DLGTEMPLATE) +
    (2 * sizeof(WORD)) + // menu and class
    ((nTitleLen + 1) * sizeof(WCHAR));
  
  // NOTE - font is set in MsgBoxDlgProc
  
  nBufferSize = (nBufferSize + 3) & ~3; // adjust size to make first control DWORD aligned
  
  // loop to calculate size of buffer we need add size of each control: 
  // sizeof(DLGITEMTEMPLATE) +
  // sizeof(WORD) +        // atom value flag 0xFFFF
  // sizeof(WORD) +        // ordinal value of control's class
  // sizeof(WORD) +        // no. of bytes in creation data array 
  // sizeof title in WCHARs
  
  for (i = 0; i < m_dlgTempl.cdit; i++)
  {
    int nItemLength = sizeof(DLGITEMTEMPLATE) + 3 * sizeof(WORD);
    
#ifdef _UNICODE
    int nActualChars = lstrlen(m_pDlgItemArray[i]->m_pszCaption) + 1;
#else
    int nActualChars = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)m_pDlgItemArray[i].m_pszCaption, -1, NULL, 0); 
#endif
    if(nActualChars <= 0)
      break;
    
    nItemLength += nActualChars * sizeof(WCHAR);
    
    if (i != m_dlgTempl.cdit - 1) // the last control does not need extra bytes
    {
      nItemLength = (nItemLength + 3) & ~3; // take into account gap so next control is DWORD aligned
    }
    
    nBufferSize += nItemLength;
  }
  
  HLOCAL hLocal = LocalAlloc(LHND, nBufferSize);
  if (hLocal == NULL)
  {
    return IDCANCEL;
  }
  
  BYTE* pBuffer = (BYTE*)LocalLock(hLocal);
  if (pBuffer == NULL)
  {
    LocalFree(hLocal);
    return IDCANCEL;
  }
  
  BYTE* pdest = pBuffer;
  
  // transfer DLGTEMPLATE structure to the buffer
  memcpy(pdest, &m_dlgTempl, sizeof(DLGTEMPLATE));
  pdest += sizeof(DLGTEMPLATE);
  
  *(WORD*)pdest = 0;		// no menu
  pdest += sizeof(WORD); 
  *(WORD*)pdest = 0;		// use default window class 
  pdest += sizeof(WORD); 
  
  // transfer title
  WCHAR pchCaption[CAPTION_TEXT_MAX_LEN+80] = {0, };
  nTitleLen = min(nTitleLen,CAPTION_TEXT_MAX_LEN);
  memset(pchCaption, 0, nTitleLen*sizeof(WCHAR));
  
#ifdef _UNICODE
  memcpy(pchCaption, pszCaption, nTitleLen*sizeof(WCHAR));
  int nActualChars = nTitleLen + 1;
#else
  int nActualChars = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszCaption, -1, pchCaption, nTitleLen + 1);
#endif
  
  if(nActualChars <= 0)
    return IDCANCEL;
  
  memcpy(pdest, pchCaption, nActualChars * sizeof(WCHAR));
  pdest += nActualChars * sizeof(WCHAR); 
  
  // will now transfer the information for each one of the item templates
  for (i = 0; i < m_dlgTempl.cdit; i++)
  {
    pdest = (BYTE*)(((DWORD)pdest + 3) & ~3);	// make the pointer DWORD aligned
    memcpy(pdest, (void *)&m_pDlgItemArray[i].m_dlgItemTemplate, sizeof(DLGITEMTEMPLATE));
    pdest += sizeof(DLGITEMTEMPLATE);
    *(WORD*)pdest = 0xFFFF;						// indicating atom value
    pdest += sizeof(WORD);
    *(WORD*)pdest = (WORD)m_pDlgItemArray[i].m_controltype;	// atom value for the control
    pdest += sizeof(WORD);
    
    // transfer the caption even when it is an empty string    
    int nChars = (int)lstrlen(m_pDlgItemArray[i].m_pszCaption) + 1; 
     
    WCHAR* pszchText = NULL;
    int nActualChars = 0;

#ifdef _UNICODE
    pszchText = new TCHAR[nChars*sizeof(TCHAR)+4];
    if(pszchText)
    {
      memset(pszchText, 0, nChars*sizeof(TCHAR) + 4);
      memcpy(pszchText, m_pDlgItemArray[i]->m_pszCaption, nChars * sizeof(TCHAR)); 
    }
    nActualChars = nChars;
#else
    pszchText = new WCHAR[nChars*sizeof(WCHAR)+4];
    if(pszchText)
    {
      nActualChars = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)m_pDlgItemArray[i].m_pszCaption, -1, pszchText, nChars);
    }
#endif
    
    if(nActualChars <= 0)
      break;

    if(pszchText)
    {
      memcpy(pdest, pszchText, nActualChars * sizeof(WCHAR));
    }
    pdest += nActualChars * sizeof(WCHAR);

    if(pszchText)
    {
      delete [] pszchText ;
      pszchText = NULL;
    }
    
    *(WORD*)pdest = 0; // How many bytes in data for control
    pdest += sizeof(WORD);
  } // for 
  
  if(pdest - pBuffer > nBufferSize)
  {
    return 0;
  }
  
  HINSTANCE hInstance = ::GetModuleHandle(NULL);
  
  HWND hDlg = ::CreateDialogIndirectParam(hInstance, (LPCDLGTEMPLATE) pBuffer, m_hWndOwner, (DLGPROC)MsgBoxDlgProc, (LPARAM) pDlgBox);
  DWORD dwError = ::GetLastError();
    
  if (hDlg)
  {
    // disable owner - this is a modal dialog
    ::EnableWindow(m_hWndOwner, FALSE);
    
    MSG msg;
    memset(&msg, 0, sizeof(msg));
    
    // message loop for dialog
    
    while (!pDlgBox->m_bShutdown)
    {
      msg.message = 0;

      if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
      {
        if (msg.message == WM_QUIT) 
        {
          break;
        } 

        if (msg.message == WM_KEYDOWN)
        {
          // returns TRUE if Ctrl-C processed
          if (OnKeyDown(hDlg, msg.wParam, msg.lParam))
            continue;
        }

        if (!::IsDialogMessage(hDlg, &msg)) 
        {
          ::TranslateMessage(&msg);
          ::DispatchMessage(&msg);
        }
      }
      else if (!pDlgBox->m_bShutdown) 
      {
        ::WaitMessage();	// suspend thread until new message arrives
      }
    }
    
    if (msg.message == WM_QUIT) 
    {
      PostQuitMessage((int)msg.wParam);
    }
    
    // re-enable owner
    if (::IsWindow(m_hWndOwner))
    {
      ::EnableWindow(m_hWndOwner, TRUE);
    }
    
    ::DestroyWindow(hDlg);
    hDlg = NULL;
  } // if (hDlg)   
  
  LocalUnlock(hLocal);
  LocalFree(hLocal);
  
  if (pDlgBox->m_hIcon)
    DestroyIcon(pDlgBox->m_hIcon);
  pDlgBox->m_hIcon = 0;
  
  if (pDlgBox->m_hFont)
    ::DeleteObject(pDlgBox->m_hFont);
  pDlgBox->m_hFont = NULL;
  
  if (pDlgBox->m_hBackgroundBrush)
    ::DeleteObject(pDlgBox->m_hBackgroundBrush);
  pDlgBox->m_hBackgroundBrush = 0;
    
  ret = MAKELONG(pDlgBox->m_nReturnValue, pDlgBox->m_nReturnValueCheckBox);
  
  delete pDlgBox;
  pDlgBox = NULL;
  
  return ret;
} 


///////////////////////////////////////////////////////////////////////////////
// SetClipboardText - Places text on the clipboard
//
// Parameters:
//	lpszBuffer - pointer to a string to put on the clipboard
// 
// Return Values:
//	TRUE       - Text was successfully copied onto clipboard
//	FALSE      - Text not copied
//
BOOL SetClipboardText(LPCTSTR lpszBuffer)
{
	BOOL bSuccess = FALSE;

	// First, open the clipboard. OpenClipboard() takes one
	// parameter, the handle of the window that will temporarily
	// be it's owner. If NULL is passed, the current process
	// is assumed. After opening, empty the clipboard so we
	// can put our text on it.
	if (::OpenClipboard(NULL))
	{
		::EmptyClipboard();

		// Get the size of the string in the buffer that was
		// passed into the function, so we know how much global
		// memory to allocate for the string.
		size_t nSize = lstrlen(lpszBuffer);

		// Allocate the memory for the string.
		HGLOBAL hGlobal = ::GlobalAlloc(GMEM_ZEROINIT, (nSize+1)*sizeof(TCHAR));

		// If we got any error during the memory allocation,
		// we have been returned a NULL handle.
		if (hGlobal)
		{
			// Now we have a global memory handle to the text
			// stored on the clipboard. We have to lock this global
			// handle so that we have access to it.
			LPTSTR lpszData = (LPTSTR) ::GlobalLock(hGlobal);

			if (lpszData)
			{
				// Now, copy the text from the buffer into the allocated
				// global memory pointer
				lstrcpy(lpszData, lpszBuffer);

				// Now, simply unlock the global memory pointer,
				// set the clipboard data type and pointer,
				// and close the clipboard.
				::GlobalUnlock(hGlobal);
#ifdef _UNICODE
				::SetClipboardData(CF_UNICODETEXT, hGlobal);
#else
				::SetClipboardData(CF_TEXT, hGlobal);
#endif
				bSuccess = TRUE;
			}
		}
		::CloseClipboard();
	}

	return bSuccess;
}


BOOL OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM /*lParam*/)
{
	BOOL rc = FALSE;

	// convert virtual key code
	TCHAR ch = (TCHAR) wParam;
  
	// if the most significant bit is set, the key is down
	BOOL bCtrlIsDown = 	GetAsyncKeyState(VK_CONTROL) < 0;
  
	if (bCtrlIsDown && (ch == _T('C')))
	{
		rc = TRUE;

		// user hit Ctrl-C -- copy contents to clipboard
		if (::IsWindow(hWnd))
		{
			TCHAR *pszDivider = _T("---------------------------\r\n");
			TCHAR *pszText = new TCHAR [8192];
			lstrcpy(pszText, pszDivider);
			size_t n = lstrlen(pszText);
			::GetWindowText(hWnd, &pszText[n], 1000);		// caption
			_tcscat(pszText, _T("\r\n"));
			_tcscat(pszText, pszDivider);
			HWND hMsgHwnd = ::GetDlgItem(hWnd, MessageControlId);
			if (::IsWindow(hMsgHwnd))
			{
				n = lstrlen(pszText);
				::GetWindowText(hMsgHwnd, &pszText[n], 4000);	// message
				_tcscat(pszText, _T("\r\n"));
				_tcscat(pszText, pszDivider);
			}

			TCHAR szClassName[MAX_PATH];
			TCHAR szButton[200];
			HWND hwndChild = ::GetWindow(hWnd, GW_CHILD);
			while (hwndChild)
			{
				if (::IsWindow(hwndChild))
				{
					::GetClassName(hwndChild, szClassName, COUNTOF(szClassName)-2);
					if (_tcsicmp(szClassName, _T("Button")) == 0)
					{
						::GetWindowText(hwndChild, szButton, COUNTOF(szButton)-1);	// button text
						size_t i = lstrlen(pszText);
						TCHAR *cp = szButton;
						while (*cp)
						{
							if (*cp != _T('&'))
								pszText[i++] = *cp;
							cp++;
						}
						pszText[i] = 0;
						_tcscat(pszText, _T("   "));
					}
				}
				hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
			}

			_tcscat(pszText, _T("\r\n"));
			_tcscat(pszText, pszDivider);
			SetClipboardText(pszText);
			delete [] pszText;
		}
	}

	return rc;
}


// IconProc
LONG CALLBACK IconProc(HWND hWnd, UINT message, WPARAM, LPARAM)
{
	if (message == WM_PAINT)
	{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hWnd, &ps);
		::DrawIcon(hdc, 0, 0, (HICON)(::GetWindowLong(hWnd, GWL_USERDATA)));
		EndPaint(hWnd, &ps);
	}

	return FALSE;
}

INT_PTR CALLBACK MsgBoxDlgProc(HWND hWnd,
                               UINT message,
                               WPARAM wParam,
                               LPARAM lParam)
{
	CDlgBoxEntry* pDlgBox = (CDlgBoxEntry*) ::GetWindowLong(hWnd, GWL_USERDATA);
	HWND hwndChild = 0;
  
	switch (message)
	{
    //case WM_THEMECHANGED:
    case WM_STYLECHANGED:
    case WM_STYLECHANGING:
    case WM_SYSCOLORCHANGE:
    case WM_SETTINGCHANGE:
      return FALSE; // SKIP it.

    case WM_CTLCOLORSTATIC:	 
		{
			if (pDlgBox && pDlgBox->m_hBackgroundBrush)
			{
				HWND hStatic = ::GetDlgItem(hWnd, pDlgBox->m_nMessageId);
				if (pDlgBox->m_crBackground == MESSAGE_BOX_VALUE_INVALID)
					::SetBkColor((HDC)wParam, ::GetSysColor(COLOR_WINDOW));
				else
					::SetBkColor((HDC)wParam, pDlgBox->m_crBackground);
				if (pDlgBox->m_crText == MESSAGE_BOX_VALUE_INVALID)
					::SetTextColor((HDC)wParam, ::GetSysColor(COLOR_WINDOWTEXT));
				else
					::SetTextColor((HDC)wParam, pDlgBox->m_crText);
				if (hStatic && (hStatic == (HWND)lParam))
					return (LRESULT)pDlgBox->m_hBackgroundBrush;
			}
			return FALSE;
		}

		case WM_ERASEBKGND:
		{
			if (pDlgBox && pDlgBox->m_hBackgroundBrush)
			{
				HDC hdc = (HDC) wParam;

				CRectX rect;
				::GetClientRect(hWnd, &rect);

				// first fill entire client area 
				HBRUSH hBrush = ::CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));
				::FillRect(hdc, &rect, hBrush);
				::DeleteObject(hBrush);
        
				// next fill message background 
				rect.bottom = pDlgBox->m_msgrect.bottom;
				if (pDlgBox->m_hBackgroundBrush)
					::FillRect(hdc, &rect, pDlgBox->m_hBackgroundBrush);
        
				return TRUE;
			}
			return FALSE;
		}

		case WM_INITDIALOG:
		{
			::SetWindowLong(hWnd, GWL_USERDATA, lParam);	// save it for the others

			pDlgBox = (CDlgBoxEntry*) lParam; 
      if(pDlgBox == NULL)
        return FALSE;
      
			HDC hdc = ::CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
      if(hdc == NULL)
        return FALSE;

			::SelectObject(hdc, pDlgBox->m_hFont);
			::DeleteDC(hdc);

			UINT nID;
 
      hwndChild = ::GetDlgItem(hWnd, pDlgBox->m_nMessageId);
      if (::IsWindow(hwndChild))
        ::SendMessage(hwndChild, WM_SETFONT, (WPARAM)pDlgBox->m_hFont, 0);
	    
			for (nID = IDCUSTOM1; nID <  IDCUSTOM4; nID++)
			{
				hwndChild = ::GetDlgItem(hWnd, nID);
				if (hwndChild && ::IsWindow(hwndChild))
				{
					::SendMessage(hwndChild, WM_SETFONT, (WPARAM)pDlgBox->m_hFont, 0);
				}
			}
            
			hwndChild = ::GetDlgItem(hWnd, IDCHECKBOX);
			if (hwndChild && ::IsWindow(hwndChild))
			{
				::SendMessage(hwndChild, WM_SETFONT, (WPARAM)pDlgBox->m_hFont, 0);
				CheckDlgButton(hWnd, IDCHECKBOX, pDlgBox->m_bNeedCheck);
			}
      
			hwndChild = ::GetDlgItem(hWnd, pDlgBox->m_nDefaultButtonId);
			if (hwndChild && ::IsWindow(hwndChild))
				::SetFocus(hwndChild);
      
			if (pDlgBox->m_hIcon)
			{
				HWND hwndIcon;
        
				hwndIcon = ::GetDlgItem(hWnd, 1000);
				::SetWindowLong(hwndIcon, GWL_WNDPROC, (LONG) IconProc);
				::SetWindowLong(hwndIcon, GWL_USERDATA, (LONG) pDlgBox->m_hIcon);
			}

			if (pDlgBox->m_nStyle & MB_TOPMOST)
				::SetWindowPos(hWnd, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);

			if (pDlgBox->m_nStyle & MB_SETFOREGROUND)
				::SetForegroundWindow(hWnd);

      HWND hParent = ::GetParent(hWnd);

			if (pDlgBox->m_X || pDlgBox->m_Y)
			{
				// caller has specified screen coordinates
				CRectX dlgRect;
				::GetWindowRect(hWnd, &dlgRect);
				::MoveWindow(hWnd, pDlgBox->m_X, pDlgBox->m_Y, dlgRect.Width(), dlgRect.Height(), TRUE);
			}
			else if (hParent != NULL)
			{
				// code to center dialog
        CRectX mainRect, dlgRect, desktopRect;  
        ::GetWindowRect(hWnd, &dlgRect);
        if (IsIconic(hParent)) 
        { 
          HWND hDesktop = ::GetDesktopWindow(); 
          ::GetWindowRect(hDesktop, &mainRect); 
        }
        else 
        {
          ::GetWindowRect(hParent, &mainRect);
        }
        
        int x = (mainRect.right  + mainRect.left)/2 - dlgRect.Width()/2;
        int y =	(mainRect.bottom + mainRect.top) /2 - dlgRect.Height()/2;
        if(x < 0 || y < 0)
        {
          ::MoveWindow(hWnd, pDlgBox->m_X, pDlgBox->m_Y, dlgRect.Width(), dlgRect.Height(), TRUE);
        }
        else 
        {
          ::MoveWindow(hWnd, x, y, dlgRect.Width(), dlgRect.Height(), TRUE); 
        }
			}

			// display seconds countdown if timeout specified
			if (pDlgBox->m_nTimeoutSeconds > 0)
				::SetTimer(hWnd, 1, 1000, NULL);
			::SetForegroundWindow(hWnd);

			return FALSE;

		} // case WM_INITDIALOG

		case WM_COMMAND:
		{
			// user clicked on something - stop the timer
			::KillTimer(hWnd, 1);
      
      if(pDlgBox == NULL)
        return FALSE;
      
			if (pDlgBox->m_nDefaultButtonId)
			{
				HWND hwndDefButton = ::GetDlgItem(hWnd, pDlgBox->m_nDefaultButtonId);
				if (hwndDefButton && ::IsWindow(hwndDefButton))
				{
					if (pDlgBox->m_szDefaultButton[0] != _T('\0'))
					{
						::SetWindowText(hwndDefButton, pDlgBox->m_szDefaultButton);
					}
				}
			}
      
      if( wParam == (pDlgBox->m_nDefaultButtonId | MB_TIMEOUT))
      {
					pDlgBox->m_bShutdown = TRUE;
          pDlgBox->m_nReturnValue = wParam;
          hwndChild = ::GetDlgItem(hWnd, IDCHECKBOX);
          if (hwndChild && ::IsWindow(hwndChild))
          {
            pDlgBox->m_nReturnValueCheckBox = IsDlgButtonChecked(hWnd, IDCHECKBOX);
          }
          return TRUE;
      }
      
			switch (wParam)
			{
				case IDCLOSE:
          {
            hwndChild = ::GetDlgItem(hWnd, IDCHECKBOX);
            if (hwndChild && ::IsWindow(hwndChild))
            {
              pDlgBox->m_nReturnValueCheckBox = IsDlgButtonChecked(hWnd, IDCHECKBOX);
            }
            return TRUE;
          }
        
				case IDOK:
          {
            hwndChild = ::GetDlgItem(hWnd, IDCHECKBOX);
            if (hwndChild && ::IsWindow(hwndChild))
            {
              pDlgBox->m_nReturnValueCheckBox = IsDlgButtonChecked(hWnd, IDCHECKBOX);
            }
            pDlgBox->m_bShutdown    = TRUE;
            pDlgBox->m_nReturnValue = IDOK;
          }
					return FALSE;
        
        case IDCANCEL:
          {
            hwndChild = ::GetDlgItem(hWnd, IDCHECKBOX);
            if (hwndChild && ::IsWindow(hwndChild))
            {
              pDlgBox->m_nReturnValueCheckBox = IsDlgButtonChecked(hWnd, IDCHECKBOX);
            }
            pDlgBox->m_bShutdown    = TRUE;
            pDlgBox->m_nReturnValue = IDCANCEL;
          }
          return FALSE;

        case IDCUSTOM1:
          {
            hwndChild = ::GetDlgItem(hWnd, IDCHECKBOX);
            if (hwndChild && ::IsWindow(hwndChild))
            {
              pDlgBox->m_nReturnValueCheckBox = ::IsDlgButtonChecked(hWnd, IDCHECKBOX);
            }
            pDlgBox->m_bShutdown    = TRUE;
						pDlgBox->m_nReturnValue = IDCUSTOM1;
          }
          break;

        case IDCUSTOM2:
          {
            hwndChild = ::GetDlgItem(hWnd, IDCHECKBOX);
            if (hwndChild && ::IsWindow(hwndChild))
            {
              pDlgBox->m_nReturnValueCheckBox = IsDlgButtonChecked(hWnd, IDCHECKBOX);
            }
            pDlgBox->m_bShutdown    = TRUE;
						pDlgBox->m_nReturnValue = IDCUSTOM2;
          }
          break;

        case IDCUSTOM3:
          {
            hwndChild = ::GetDlgItem(hWnd, IDCHECKBOX);
            if (hwndChild && ::IsWindow(hwndChild))
            {
              pDlgBox->m_nReturnValueCheckBox = IsDlgButtonChecked(hWnd, IDCHECKBOX);
            }
            pDlgBox->m_bShutdown    = TRUE;
            pDlgBox->m_nReturnValue = IDCUSTOM3;
          }
          break;

        case IDCHECKBOX:
          if(pDlgBox->m_bCheckBox)
          {
            hwndChild = ::GetDlgItem(hWnd, IDCHECKBOX);
            BOOL bCheckBox = FALSE;
            
            if (hwndChild && ::IsWindow(hwndChild))
            {
              bCheckBox = (BOOL)::SendMessage(hwndChild, BM_GETCHECK, 0, 0);
            }
            
            WriteRegistry(pDlgBox->m_szIdentifier,MAKELONG(IDCHECKBOX, bCheckBox));
            pDlgBox->m_nReturnValueCheckBox = bCheckBox;
          }
          else 
          {
            pDlgBox->m_nReturnValueCheckBox = FALSE;
          	pDlgBox->m_bShutdown            = TRUE;
						pDlgBox->m_nReturnValue         = IDCUSTOM4;
          }
          break;
        
        default:
          break;
			} // switch (wParam)

      return FALSE;
		} // case WM_COMMAND

		case WM_LBUTTONDOWN:
		case WM_NCLBUTTONDOWN:
		{
			// user clicked on dialog or titlebar - stop the timer
			::KillTimer(hWnd, 1);

			if (pDlgBox->m_nDefaultButtonId)
			{
				HWND hwndDefButton = ::GetDlgItem(hWnd, pDlgBox->m_nDefaultButtonId);
				if (hwndDefButton && ::IsWindow(hwndDefButton))
				{
					if (pDlgBox->m_szDefaultButton[0] != _T('\0'))
					{
						::SetWindowText(hwndDefButton, pDlgBox->m_szDefaultButton);
					}
				}
			}

			return FALSE;
		}

		case WM_TIMER: // used for timeout
		{
			if (wParam == 1)
			{
        // Timeout occur 
				if (pDlgBox->m_nTimeoutSeconds <= 0)
				{
					::KillTimer(hWnd, wParam);

					// time is up, select default button 
					::SendMessage(hWnd, WM_COMMAND, pDlgBox->m_nDefaultButtonId | MB_TIMEOUT, 0);
          
					return FALSE;
				}

				if (pDlgBox->m_nDefaultButtonId == 0)
					return FALSE;

				HWND hwndDefButton = ::GetDlgItem(hWnd,  pDlgBox->m_nDefaultButtonId);
				if (hwndDefButton == NULL || !::IsWindow(hwndDefButton))
					return FALSE;

				if (pDlgBox->m_szDefaultButton[0] == _T('\0'))
				{
					// first time - get text of default button
					::GetWindowText(hwndDefButton, pDlgBox->m_szDefaultButton, DEFAULT_BUTTON_TXT_LEN);
				}
        
				TCHAR szButtonText[DEFAULT_BUTTON_TXT_LEN*2];
				wsprintf(szButtonText, _T("%s = %d"), pDlgBox->m_szDefaultButton, pDlgBox->m_nTimeoutSeconds);

				::SetWindowText(hwndDefButton, szButtonText);
        
				pDlgBox->m_nTimeoutSeconds--;
			}
			return FALSE;
		}

	} // switch (message)

	return FALSE;
}


