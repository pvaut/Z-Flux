 #if !defined(AFX_ESPDLGEDIT_H__E6749407_2741_4F2B_B208_C4F7B1A35BD9__INCLUDED_)
#define AFX_ESPDLGEDIT_H__E6749407_2741_4F2B_B208_C4F7B1A35BD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ESPDlgEdit.h : header file
//
//#ifdef ESPBENCH

/////////////////////////////////////////////////////////////////////////////
// ESPDlgEdit window
//class ESPDlgPLC ;
//class ESPDlgVLC ;
//class ESPDlgPLX ; //added 09/05/03
class CPolyDlg ; //01/11/05 have to use fwd decl to avoid #include loop
class ESPDlgEdit : public CEdit
{
// Construction
public:
	ESPDlgEdit();

// Attributes
public:
	UINT m_CtlID ;//added 5/6/00: ID of the subclassed control
	CPolyDlg* m_pDlg ; 

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ESPDlgEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~ESPDlgEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(ESPDlgEdit)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
//#endif // ESPBENCH 04/27/04

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ESPDLGEDIT_H__E6749407_2741_4F2B_B208_C4F7B1A35BD9__INCLUDED_)
