#if !defined(AFX_CONTOURDLG_H__A999E9C5_C17F_4DBA_B8F1_80982985958A__INCLUDED_)
#define AFX_CONTOURDLG_H__A999E9C5_C17F_4DBA_B8F1_80982985958A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ContourDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CContourDlg dialog

class CContourDlg : public CDialog
{
// Construction
public:
	CContourDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CContourDlg)
	enum { IDD = IDD_CONTOUR };
	CString	m_Name;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CContourDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CContourDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTOURDLG_H__A999E9C5_C17F_4DBA_B8F1_80982985958A__INCLUDED_)
