// ESPDlgEdit.cpp : implementation file
//
//#ifdef ESPBENCH //04/27/04

#include "stdafx.h"
#include "ESPDlgEdit.h"
#include "PolyDlg.h"

BEGIN_MESSAGE_MAP(ESPDlgEdit, CEdit)
	//{{AFX_MSG_MAP(ESPDlgEdit)
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ESPDlgEdit
//Purpose: Allows user to press ENTER in a drop-down list's editbox

ESPDlgEdit::ESPDlgEdit()
:m_CtlID( NULL ), //added 5/6/00 for ctl ID of subclassed combobox
 m_pDlg( NULL )
{
}

ESPDlgEdit::~ESPDlgEdit()
{
}


/////////////////////////////////////////////////////////////////////////////
// ESPDlgEdit message handlers

UINT ESPDlgEdit::OnGetDlgCode()
{
      return DLGC_WANTALLKEYS;
}

void ESPDlgEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{

	if (nChar == VK_RETURN)
	{
		if( m_pDlg)
		{
			m_pDlg->OnEnterKey( m_CtlID ) ;
		}

		return ;
	}

	//added 01/11/05 to jump to next window in tab order
	if (nChar == VK_TAB)
	{
		CWnd* pParentWnd = GetParent() ; //edit box is child of combo box
		CWnd* pWnd = pParentWnd->GetNextWindow() ; //get nxt wnd in tab order

		if( pWnd )
		{
			pWnd->SetFocus() ;
		}

		return ;
	}
	
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}
