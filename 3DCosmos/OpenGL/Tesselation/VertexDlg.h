#if !defined(AFX_VERTEXDLG_H__9D2DF209_4E28_489E_B7D7_503B5514A7D0__INCLUDED_)
#define AFX_VERTEXDLG_H__9D2DF209_4E28_489E_B7D7_503B5514A7D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VertexDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVertexDlg dialog

class CVertexDlg : public CDialog
{
// Construction
public:
	CVertexDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVertexDlg)
	enum { IDD = IDD_OPENGLVERTEX };
	CEdit	m_Edit_Red;
	CEdit	m_Edit_Green;
	CEdit	m_Edit_Blue;
	CButton	m_Btn_Color;
	int		m_Blue;
	int		m_Green;
	int		m_Red;
	float	m_X;
	float	m_Y;
	float	m_Z;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVertexDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVertexDlg)
	afx_msg void OnBtnColor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VERTEXDLG_H__9D2DF209_4E28_489E_B7D7_503B5514A7D0__INCLUDED_)
