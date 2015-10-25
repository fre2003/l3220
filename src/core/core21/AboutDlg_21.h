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


#if !defined(AFX_ITSABOUTDLG_H__E737B8B5_FFAE_4588_AA34_4691A0B27D3F__INCLUDED_)
#define AFX_ITSABOUTDLG_H__E737B8B5_FFAE_4588_AA34_4691A0B27D3F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ItsAboutDlg.h : header file
//

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
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ITSABOUTDLG_H__E737B8B5_FFAE_4588_AA34_4691A0B27D3F__INCLUDED_)
