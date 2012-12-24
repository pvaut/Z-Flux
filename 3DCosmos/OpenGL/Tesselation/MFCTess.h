// MFCTess.h : main header file for the MFCTESS application
//

#if !defined(AFX_MFCTESS_H__F40CD08C_5C17_4DE3_A389_7DEF96063A50__INCLUDED_)
#define AFX_MFCTESS_H__F40CD08C_5C17_4DE3_A389_7DEF96063A50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMFCTessApp:
// See MFCTess.cpp for the implementation of this class
//

#define PI 3.141592653589793  //hopefully this is at global scope

BOOL GlobalSignOrDecimalPt( CString dlgtext ) ;

class CMFCTessApp : public CWinApp
{
public:
	CMFCTessApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMFCTessApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CMFCTessApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCTESS_H__F40CD08C_5C17_4DE3_A389_7DEF96063A50__INCLUDED_)
