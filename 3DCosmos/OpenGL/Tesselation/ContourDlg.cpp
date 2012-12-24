// ContourDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFCTess.h"
#include "ContourDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CContourDlg dialog


CContourDlg::CContourDlg(CWnd* pParent /*=NULL*/)
:CDialog(CContourDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CContourDlg)
	m_Name = _T("");
	//}}AFX_DATA_INIT
}


void CContourDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CContourDlg)
	DDX_Text(pDX, IDC_EDIT_NAME, m_Name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CContourDlg, CDialog)
	//{{AFX_MSG_MAP(CContourDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CContourDlg message handlers
