#if !defined(AFX_POLYDLG_H__616FB862_C429_454A_B0D4_70055308A1B5__INCLUDED_)
#define AFX_POLYDLG_H__616FB862_C429_454A_B0D4_70055308A1B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PolyDlg.h : header file
//
#include "Vector3d.h"
#include "espdlgedit.h"
#include "ColorComboBox.h"
#include "MFCTess.h"
/////////////////////////////////////////////////////////////////////////////
// CPolyDlg dialog

class CPolyDlg : public CDialog
{
// Construction
public:
	CPolyDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPolyDlg)
	enum { IDD = IDD_POLYGON };
	CButton	m_Btn_OK;
//	CComboBox	m_Combo_Corner;
	CColorComboBox	m_Combo_Corner;
	CButton	m_Btn_DelContour;
	CButton	m_Btn_NewContour;
	CButton	m_Btn_ModCorner;
	CButton	m_Btn_InsCorner;
	CButton	m_Btn_DelCorner;
	CButton	m_Btn_AddCorner;
	CListBox	m_ContourList;
	//}}AFX_DATA
	
	CObList m_ContourPtrList ;
	
protected:
	CString m_csCoordFmtStr ; //added for coord list esthetics - mvd here 09/20/03
	ESPDlgEdit m_subedit; //subclass object
	CWnd* m_pEditWnd;	//ptr to  editbox
	int m_LastContourSelIdx ;

	//corner list enable/disable colors
	CBrush m_OnBrush, m_OffBrush ;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPolyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPolyDlg)
	afx_msg void OnBtnNewContour();
	afx_msg void OnBtnDeleteCorner();
	afx_msg void OnBtnInsertCorner();
	afx_msg void OnBtnModifyCorner();
	afx_msg void OnBtnRemContour();
	afx_msg void OnSelchangeContour();
	afx_msg void OnSelchangeComboWallpts();
	afx_msg void OnBtnAddCorner();
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void UpdateControls() ;
	void FormatNumStr( CString& numstr, int num ) ;
	void SetItemNum( CString& str, const int num ) ;
	int FindDupCrnr( Vector3D crnr, CComboBox& combo ) ;
	int GetNumTokens( CString& instr ) ;
	int GetItemNum( CString itemstr ) ;
	int RenumberList( CComboBox* pCombo ) ;
	Vector3D GetCnrVector( CString cnrstr ) ;
	void EnableCornerList( BOOL bEnable ) ;
	COLORREF GetCnrColor( CString& itemstr ) ;

public:
	void OnEnterKey( UINT ctlID ) ; //manage combo editbox processing
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POLYDLG_H__616FB862_C429_454A_B0D4_70055308A1B5__INCLUDED_)
