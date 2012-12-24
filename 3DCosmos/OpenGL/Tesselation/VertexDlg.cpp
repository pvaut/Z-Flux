// VertexDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFCTess.h"
#include "VertexDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVertexDlg dialog


CVertexDlg::CVertexDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVertexDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVertexDlg)
	m_Blue = 0;
	m_Green = 0;
	m_Red = 0;
	m_X = 0.0f;
	m_Y = 0.0f;
	m_Z = 0.0f;
	//}}AFX_DATA_INIT
}


void CVertexDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVertexDlg)
	DDX_Control(pDX, IDC_EDIT_RED, m_Edit_Red);
	DDX_Control(pDX, IDC_EDIT_GREEN, m_Edit_Green);
	DDX_Control(pDX, IDC_EDIT_BLUE, m_Edit_Blue);
	DDX_Control(pDX, IDC_BTN_COLOR, m_Btn_Color);
	DDX_Text(pDX, IDC_EDIT_BLUE, m_Blue);
	DDX_Text(pDX, IDC_EDIT_GREEN, m_Green);
	DDX_Text(pDX, IDC_EDIT_RED, m_Red);
	DDX_Text(pDX, IDC_EDIT_X, m_X);
	DDX_Text(pDX, IDC_EDIT_Y, m_Y);
	DDX_Text(pDX, IDC_EDIT_Z, m_Z);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVertexDlg, CDialog)
	//{{AFX_MSG_MAP(CVertexDlg)
	ON_BN_CLICKED(IDC_BTN_COLOR, OnBtnColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVertexDlg message handlers

void CVertexDlg::OnBtnColor() 
{
	UpdateData() ;
	CColorDialog dlg( RGB( m_Red, m_Green, m_Blue ), CC_FULLOPEN ) ;
 
	if ( dlg.DoModal() == IDOK ) 
	{
		COLORREF col = dlg.GetColor() ;
		m_Red = GetRValue( col ) ;
		m_Green = GetGValue( col ) ;
		m_Blue = GetBValue( col ) ;
		UpdateData( FALSE ) ;
	}
}
