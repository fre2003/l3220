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
// ItsAboutDlg.cpp : implementation file
//

#include "stdafx.h"

#define _BCGCBPRO_VERSION_		0x100000
#define _BCGCBPRO_VERSION_MAJOR	10
#define _BCGCBPRO_VERSION_MINOR	00

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDD_ITS_ABOUT_DLG               17072

/////////////////////////////////////////////////////////////////////////////
// CItsAboutDlg dialog

class CItsAboutDlg : public CDialog
{
// Construction
public:
	CItsAboutDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CItsAboutDlg)
	enum { IDD = IDD_ITS_ABOUT_DLG };
	//CBCGPURLLinkButton	m_wndURL;
	CButton	m_wndPurchaseBtn;
	CStatic	m_wndAppName;
	//CBCGPURLLinkButton	m_wndEMail;
	CString	m_strAppName;
	CString	m_strVersion;
	CString	m_strYear;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CItsAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CItsAboutDlg)
	afx_msg void OnBcgbarresPurchase();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CItsAboutDlg::CItsAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CItsAboutDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBCGPAboutDlg)
	m_strAppName = _T("");
	m_strVersion = _T("");
	m_strYear = _T("");
	//}}AFX_DATA_INIT

// 	m_strVersion.Format (_T("%d.%d"), _BCGCBPRO_VERSION_MAJOR, _BCGCBPRO_VERSION_MINOR);
// 
// 	CString strCurrDate = __DATE__;
// 	m_strYear.Format (_T("1998-%s"), strCurrDate.Right (4));
// 
// 	m_strAppName = lpszAppName;
}


void CItsAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CItsAboutDlg)
	//DDX_Control(pDX, IDC_BCGBARRES_URL, m_wndURL);
	DDX_Control(pDX, IDC_BCGBARRES_PURCHASE, m_wndPurchaseBtn);
	DDX_Control(pDX, IDC_BCGBARRES_NAME, m_wndAppName);
	DDX_Control(pDX, IDC_BCGBARRES_MAIL, m_wndEMail);
	DDX_Text(pDX, IDC_BCGBARRES_NAME, m_strAppName);
	DDX_Text(pDX, IDC_BCGBARRES_VERSION, m_strVersion);
	DDX_Text(pDX, IDC_BCGBARRES_YEAR, m_strYear);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CItsAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CItsAboutDlg)
	ON_BN_CLICKED(IDC_BCGBARRES_PURCHASE, OnBcgbarresPurchase)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItsAboutDlg message handlers

BOOL CItsAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//------------------
	// Create bold font:
	//------------------
	CFont* pFont = m_wndAppName.GetFont ();
	ASSERT_VALID (pFont);

	LOGFONT lf;
	memset (&lf, 0, sizeof (LOGFONT));

	pFont->GetLogFont (&lf);

	lf.lfWeight = FW_BOLD;
	m_fontBold.CreateFontIndirect (&lf);

	m_wndAppName.SetFont (&m_fontBold);

	//-----------------------------
	// Setup URL and e-mail fields:
	//-----------------------------
/*
		m_wndEMail.SetURLPrefix (_T("mailto:"));
		m_wndEMail.SetURL (_T("info@bcgsoft.com"));
		m_wndEMail.SizeToContent ();
		m_wndEMail.SetTooltip (_T("Send mail to us"));
		m_wndEMail.m_bDrawFocus = FALSE;
	
		m_wndURL.m_bDrawFocus = FALSE;
		m_wndURL.SizeToContent ();
*/
	
  
	//--------------------
	// Set dialog caption:
	//--------------------
	CString strCaption;
	strCaption.Format (_T("About %s"), m_strAppName);

	SetWindowText (strCaption);

	//------------------------------------------
	// Hide "Purchase" button in retail version:
	//------------------------------------------
#ifndef _BCGCBPRO_EVAL_
	m_wndPurchaseBtn.EnableWindow (FALSE);
	m_wndPurchaseBtn.ShowWindow (SW_HIDE);
#endif
  
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CItsAboutDlg::OnBcgbarresPurchase() 
{
	CString strURL = _T("http://www.bcgsoft.com/register-bcgcbpe.htm");
	::ShellExecute (NULL, _T("open"), strURL, NULL, NULL, SW_SHOWNORMAL);

	EndDialog (IDOK);
}

void ITS_ShowAboutDlg (LPCTSTR lpszAppName)
{
	//CBCGPLocalResource locaRes;

	CItsAboutDlg dlg (lpszAppName);
	dlg.DoModal ();
}

void ITS_ShowAboutDlg (UINT uiAppNameResID)
{
	CString strAppName;
	strAppName.LoadString (uiAppNameResID);

	ITS_ShowAboutDlg (strAppName);
}

