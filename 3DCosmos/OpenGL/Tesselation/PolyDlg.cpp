// PolyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFCTess.h"
#include "PolyDlg.h"
#include "ContourDlg.h"
#include "strstrea.h"  
#include "VertexDlg.h"
#include <float.h>
#include "Contour.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPolyDlg dialog


CPolyDlg::CPolyDlg(CWnd* pParent /*=NULL*/)
:CDialog(CPolyDlg::IDD, pParent),
 m_csCoordFmtStr( "     %3.6f     %3.6f    %3.6f   %u   %u   %u" ), //for vertex editbox string
// m_csCoordFmtStr( "     %3.6f     %3.6f    %3.6f   %d   %d   %d" ), //for vertex editbox string
 m_pEditWnd( NULL ),
 m_LastContourSelIdx( -1 )
{
	//{{AFX_DATA_INIT(CPolyDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_OnBrush.CreateSolidBrush( GetSysColor( COLOR_WINDOW ) )  ;
	m_OffBrush.CreateSolidBrush( GetSysColor( COLOR_3DFACE ) )  ;
}


void CPolyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPolyDlg)
	DDX_Control(pDX, IDOK, m_Btn_OK);
	DDX_Control(pDX, IDC_COMBO_WALLPTS, m_Combo_Corner);
	DDX_Control(pDX, IDC_BTN_REMPROF, m_Btn_DelContour);
	DDX_Control(pDX, IDC_BTN_NEW, m_Btn_NewContour);
	DDX_Control(pDX, IDC_BTN_MODIFY, m_Btn_ModCorner);
	DDX_Control(pDX, IDC_BTN_INSERT, m_Btn_InsCorner);
	DDX_Control(pDX, IDC_BTN_DELETE, m_Btn_DelCorner);
	DDX_Control(pDX, IDC_BTN_ADD, m_Btn_AddCorner);
	DDX_Control(pDX, IDC_LIST_FILE, m_ContourList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPolyDlg, CDialog)
	//{{AFX_MSG_MAP(CPolyDlg)
	ON_BN_CLICKED(IDC_BTN_NEW, OnBtnNewContour)
	ON_BN_CLICKED(IDC_BTN_DELETE, OnBtnDeleteCorner)
	ON_BN_CLICKED(IDC_BTN_INSERT, OnBtnInsertCorner)
	ON_BN_CLICKED(IDC_BTN_MODIFY, OnBtnModifyCorner)
	ON_BN_CLICKED(IDC_BTN_REMPROF, OnBtnRemContour)
	ON_LBN_SELCHANGE(IDC_LIST_FILE, OnSelchangeContour)
	ON_CBN_SELCHANGE(IDC_COMBO_WALLPTS, OnSelchangeComboWallpts)
	ON_BN_CLICKED(IDC_BTN_ADD, OnBtnAddCorner)
	ON_WM_DRAWITEM()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPolyDlg message handlers

BOOL CPolyDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//Subclass combobox edit ctrl
	m_subedit.m_pDlg = this;	// so ESPDlgEdit can call OnEnterKey()
	CWnd *pWnd = (m_Combo_Corner.GetWindow(GW_CHILD));
	m_pEditWnd = (pWnd->GetNextWindow()); //pointer to editbox window
	m_subedit.SubclassWindow(m_pEditWnd->GetSafeHwnd()); //subclass to CPMDlgEdit class

	//load contour list
	POSITION pos = m_ContourPtrList.GetHeadPosition() ;
	while ( pos ) 
	{
		CString ctrstr ;
		CContour* pCtr = (CContour*)m_ContourPtrList.GetNext( pos ) ;
		ctrstr = pCtr->m_csContourName ;
		m_ContourList.AddString( ctrstr ) ;
	}

	//load corner list from 1st contour
	m_LastContourSelIdx = -1 ;
	if( m_ContourList.GetCount() > 0 )
	{
		m_ContourList.SetCurSel( 0 ) ;
		OnSelchangeContour() ;
	}
	
	UpdateControls() ;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPolyDlg::OnBtnNewContour() 
{
	CContourDlg dlg ;
	if( dlg.DoModal() == IDOK )
	{
		CString namestr = dlg.m_Name ;
		
		//chk for dupes
		BOOL bIsdupe = FALSE ;
		if( !namestr.IsEmpty() )
		{
			int numcontours = m_ContourList.GetCount() ;
			for( int conidx = 0; conidx < numcontours; conidx++ )
			{
				CString constr ; 
				m_ContourList.GetText( conidx, constr ) ;
				if( constr.Find( namestr ) >= 0 )
				{
					//oops - name collision
					AfxMessageBox( "Contour Name Collision - Try Again!", MB_OK|MB_ICONEXCLAMATION ) ;
					bIsdupe = TRUE ;
					break ;
				}
			}
		}
		else
		{
			//construct a contour name
			int numctr = m_ContourList.GetCount() ;
			namestr.Format( "Contour%d", numctr + 1 ) ;
		}

		if( !bIsdupe )
		{
			m_ContourList.AddString( namestr ) ;

			CContour* pCtr = new CContour ;
			pCtr->m_csContourName = namestr ;

			m_ContourPtrList.AddTail( pCtr ) ;
			m_ContourList.SetCurSel( m_ContourList.GetCount() - 1 ) ; //select the new items
			OnSelchangeContour() ;
		}
	}

	UpdateControls() ;
}

void CPolyDlg::OnBtnRemContour() 
{
	int selidx = m_ContourList.GetCurSel() ;
	int numctrs = m_ContourList.GetCount() ;
	ASSERT( selidx >= 0 && selidx < numctrs ) ;
	
	POSITION pos = m_ContourPtrList.FindIndex( selidx ) ;

	if( pos )
	{
		CContour* pCtr = (CContour*)m_ContourPtrList.GetAt( pos ) ;
		m_ContourPtrList.RemoveAt( pos ) ;
		delete pCtr ;

		m_ContourList.DeleteString( selidx ) ;
		m_Combo_Corner.ResetContent() ;

		if( selidx > 0 && selidx < numctrs ) selidx-- ;
		m_ContourList.SetCurSel( selidx ) ;

		//force the corner list to be refreshed
		if( selidx == m_LastContourSelIdx ) m_LastContourSelIdx-- ;
		OnSelchangeContour() ;

		UpdateControls() ;
	}
}

void CPolyDlg::OnSelchangeContour() 
{
	POSITION pos ;
	int numcnrs, cnridx ;
	CContour* pCtr ;
	
	int selidx = m_ContourList.GetCurSel() ;
	if( m_LastContourSelIdx != selidx )
	{
		//save current corner points back into parent contour object
		if( m_LastContourSelIdx >= 0 && m_LastContourSelIdx < m_ContourList.GetCount() )
		{
			pos = m_ContourPtrList.FindIndex( m_LastContourSelIdx ) ;
			numcnrs = m_Combo_Corner.GetCount() ;

			if( pos && numcnrs > 0 )
			{
				pCtr = (CContour*)m_ContourPtrList.GetAt( pos ) ;

				//don't delete pCtr, but do delete vertix arrays and m_ppVertexArray
				int numcnrs = pCtr->m_NumVertex ;
				for( cnridx = 0; cnridx < numcnrs; cnridx++ )
				{
					GLdouble* pcnr = pCtr->m_ppVertexArray[ cnridx ] ; //pcnr pts to a GLdouble[6] array
					delete[] pcnr ;
					pcnr = NULL ;
				}
				delete[] pCtr->m_ppVertexArray ;
				pCtr->m_ppVertexArray = NULL ;

				//now create new arrays & load them from combobox
				pCtr->m_ppVertexArray = new GLdouble*[ numcnrs ] ;
				pCtr->m_NumVertex = numcnrs ;

				for( cnridx = 0; cnridx < numcnrs; cnridx++ )
				{
					GLdouble* pcnr = new GLdouble[6] ; //pcnr pts to a GLdouble[6] array
					CString cnrstr ;
					
					m_Combo_Corner.GetLBText( cnridx, cnrstr ) ;
					Vector3D vcnr = GetCnrVector( cnrstr ) ;
					COLORREF col = GetCnrColor( cnrstr ) ;
					int R = GetRValue( col ) ;
					int G = GetGValue( col ) ;
					int B = GetBValue( col ) ;
					
					pcnr[0] = (GLdouble)vcnr.x ;
					pcnr[1] = (GLdouble)vcnr.y ;
					pcnr[2] = (GLdouble)vcnr.z ;

					//need to convert integer color value to normalized GLdouble
					pcnr[3] = (GLdouble)R/255. ;
					pcnr[4] = (GLdouble)G/255. ;
					pcnr[5] = (GLdouble)B/255. ;

					pCtr->m_ppVertexArray[ cnridx ] = pcnr ;
				}

				m_Combo_Corner.ResetContent() ; //dont do this if last selidx was invalid
			}
		}

		//load selected countour's corner pts into the combo list
		pos = m_ContourPtrList.FindIndex( selidx ) ;
		if( pos )
		{
			CContour* pCtr = (CContour*)m_ContourPtrList.GetAt( pos ) ;
			int numcnrs = pCtr->m_NumVertex ;

			for( int cnridx = 0; cnridx < numcnrs; cnridx++ )
			{
				GLdouble* pcnr = pCtr->m_ppVertexArray[ cnridx ] ; //pcnr pts to a GLdouble[6] array
				CString numstr, cnrstr ;
				FormatNumStr( numstr, cnridx + 1 ) ;
				cnrstr.Format( m_csCoordFmtStr, pcnr[0], pcnr[1], pcnr[2], int(pcnr[3]*255), int(pcnr[4]*255), int(pcnr[5]*255) ) ;
				m_Combo_Corner.AddString( numstr + cnrstr ) ;
			}
		}

		//now its safe to overwrite last idx & combo list contents
		m_LastContourSelIdx = selidx ;
	}

	UpdateControls() ;
}

void CPolyDlg::OnBtnAddCorner() 
{
	UpdateData() ;
	CVertexDlg dlg ;
	
	if( dlg.DoModal() == IDOK )
	{
		CString numstr, cnrstr ;
		Vector3D vcnr = Vector3D( dlg.m_X, dlg.m_Y, dlg.m_Z ) ;
		FormatNumStr( numstr, m_Combo_Corner.GetCount() + 1 ) ;
		cnrstr.Format( m_csCoordFmtStr, dlg.m_X, dlg.m_Y, dlg.m_Z, dlg.m_Red, dlg.m_Green, dlg.m_Blue ) ;
		
		if( FindDupCrnr( vcnr, m_Combo_Corner ) < 0 )
		{
			m_Combo_Corner.AddString( numstr + cnrstr ) ;
		}
		else
		{
			AfxMessageBox( "Duplicate Corner - skipping!", MB_OK | MB_ICONEXCLAMATION ) ;
		}
	}

	UpdateControls() ;
}

void CPolyDlg::OnBtnDeleteCorner() 
{
	int selidx = m_Combo_Corner.GetCurSel() ;
	m_Combo_Corner.DeleteString( selidx ) ;
	if( m_Combo_Corner.GetCount() > 0 ) //12/02/01 bugfix
	{
		RenumberList( &m_Combo_Corner ) ;

		//set selection to item just before deleted one
		selidx-- ;
		m_Combo_Corner.SetCurSel( selidx ) ;
	}
	else //added 12/4/01 to clear the editbox for empty list
	{
		m_Combo_Corner.ResetContent() ;
	}

	UpdateControls() ;
}

void CPolyDlg::OnBtnInsertCorner() 
{
	CVertexDlg dlg ;
	Vector3D vcnr ;
	COLORREF color ;
	
	//get values from selected item on list
	int selidx = m_Combo_Corner.GetCurSel() ;
	if( selidx >= 0 )
	{
		CString itemstr ;
		int numsegs = m_Combo_Corner.GetCount() ;
		m_Combo_Corner.GetLBText( selidx, itemstr ) ;
		vcnr = GetCnrVector( itemstr ) ;
		color = GetCnrColor( itemstr ) ;
	}

	dlg.m_X = vcnr.x ;
	dlg.m_Y = vcnr.y ;
	dlg.m_Z = vcnr.z ;

	dlg.m_Red = GetRValue( color ) ;
	dlg.m_Green = GetGValue( color ) ;
	dlg.m_Blue = GetBValue( color ) ;

	//launch the dialog
	if( dlg.DoModal() == IDOK )
	{
		int numsegs = m_Combo_Corner.GetCount() ;

		//06/20/03 don't allow duplicate points
		Vector3D vNewCrnr = Vector3D( dlg.m_X, dlg.m_Y, dlg.m_Z ) ;
		int matchnum = FindDupCrnr( vNewCrnr, m_Combo_Corner ) ;  //rev 09/11/03
		if( matchnum > 0 )
		{	
			//If match found, warn user and exit w/o adding to list
			CString csMsgstr;
			csMsgstr.Format("Duplicate point found at corner #%d !" , matchnum );
			AfxMessageBox( csMsgstr,  MB_OK | MB_ICONEXCLAMATION );
			return;
		}

		//insert the new point into the corner list
		CString numstr, coordstr ;
		FormatNumStr( numstr, numsegs + 1 ) ;
		coordstr.Format( m_csCoordFmtStr, dlg.m_X, dlg.m_Y, dlg.m_Red, dlg.m_Green, dlg.m_Blue ) ;
		m_Combo_Corner.InsertString( selidx, numstr + coordstr ) ;
		RenumberList( &m_Combo_Corner ) ;
		m_Combo_Corner.SetCurSel( selidx ) ;

		UpdateControls() ;
	}
}

void CPolyDlg::OnBtnModifyCorner() 
{
	CComboBox& combo = m_Combo_Corner ; //convenience
	int cnridx = m_Combo_Corner.GetCurSel() ;

	ASSERT( combo.GetCount() > 0 && cnridx >= 0 ) ;

	//get values from list
	CString itemstr ;
	combo.GetLBText( cnridx, itemstr ) ;
	Vector3D vcnr = GetCnrVector( itemstr ) ;
	COLORREF color = GetCnrColor( itemstr ) ;
	int itemnum = GetItemNum( itemstr ) ;

	//11/29/01 launch 2D coordinate entry dlg
	CVertexDlg dlg ;
	CString xstr, ystr ;

	dlg.m_X = vcnr.x ;
	dlg.m_Y = vcnr.y ;
	dlg.m_Z = vcnr.z ;

	dlg.m_Red = GetRValue( color ) ;
	dlg.m_Green = GetGValue( color ) ;
	dlg.m_Blue = GetBValue( color ) ;

	if( dlg.DoModal() == IDOK )
	{
		int numsegs = m_Combo_Corner.GetCount() ;

		//06/20/03 don't allow duplicate points
		Vector3D vNewCrnr = Vector3D( dlg.m_X, dlg.m_Y, dlg.m_Z ) ;
		int matchnum = FindDupCrnr( vNewCrnr, combo ) ; //rev 09/11/03
		if( matchnum > 0 && matchnum != cnridx+1 )
		{	//If match found, warn user and exit w/o adding to list
			CString csMsgstr;
			csMsgstr.Format("Duplicate point found at corner #%d !" , matchnum );
			AfxMessageBox( csMsgstr,  MB_OK | MB_ICONEXCLAMATION );
			return;
		}

		//replace the original point in the corner list
		CString numstr, coordstr ;
		FormatNumStr( numstr, itemnum ) ;
		coordstr.Format( m_csCoordFmtStr, dlg.m_X, dlg.m_Y, dlg.m_Z, dlg.m_Red, dlg.m_Green, dlg.m_Blue ) ;
		combo.DeleteString( cnridx ) ;
		combo.InsertString( cnridx, numstr + coordstr ) ;
		combo.SetCurSel( cnridx ) ;

		UpdateControls() ;
	}
}

void CPolyDlg::OnSelchangeComboWallpts() 
{
	UpdateControls() ;
}

void CPolyDlg::UpdateControls() 
{
	int numcontours =  m_ContourList.GetCount() ;
	int ctrselidx = m_ContourList.GetCurSel() ;
	int numcnrs = m_Combo_Corner.GetCount() ;
	
	BOOL bContourAvail = ( numcontours > 0 && ctrselidx >= 0 && ctrselidx < numcontours ) ;
	EnableCornerList( bContourAvail ) ;
	m_ContourList.EnableWindow( bContourAvail ) ;
	m_Btn_DelContour.EnableWindow( bContourAvail ) ;

	if( bContourAvail)
	{
		int selidx = m_Combo_Corner.GetCurSel() ;
		m_Btn_DelCorner.EnableWindow( selidx >= 0 ) ;
		m_Btn_InsCorner.EnableWindow( selidx >= 0 ) ;
		m_Btn_ModCorner.EnableWindow( selidx >= 0 ) ;
	}
	
	m_Btn_OK.EnableWindow( bContourAvail && numcnrs > 2 ) ; //must have at least a triangle

	Invalidate() ;
}

void CPolyDlg::EnableCornerList( BOOL bEnable )
{
	m_Btn_DelCorner.EnableWindow( bEnable ) ;
	m_Btn_InsCorner.EnableWindow( bEnable ) ;
	m_Btn_ModCorner.EnableWindow( bEnable ) ;
	m_Btn_AddCorner.EnableWindow( bEnable ) ;
	m_Combo_Corner.EnableWindow( bEnable ) ;
	m_Combo_Corner.m_bEnable =  bEnable  ; //added to change bkgrnd color
}


void CPolyDlg::OnEnterKey( UINT ctlID )
{
	CString csMsg, csEditstr, csItemNum, newitmstr ;
	int itemnum ;
	CComboBox& combo = m_Combo_Corner ; //convenience

	//Get editbox string
	CWnd* pWnd = (combo.GetWindow(GW_CHILD)) ;
	CWnd* pEditWnd = (pWnd->GetNextWindow()); //pointer to editbox window
	pEditWnd->GetWindowText( csEditstr );
	csEditstr.TrimLeft() ; csEditstr.TrimRight() ;

	int numtokens = GetNumTokens( csEditstr ) ;
	int numitems = combo.GetCount() ;

	//check for too few or too many tokens in the entry string
	if( numtokens < 2 )
	{
		csMsg = "Each entry must have at least X & Y positions " ;
		AfxMessageBox( csMsg + "\n" + csEditstr ) ;
		return ;
	}

	else if( numtokens > 7 )
	{
		csMsg = "Each line can have no more than seven items" ;
		AfxMessageBox( csMsg + "\n" + csEditstr ) ;
		return ;
	}

	//OK, 2 or 3 tokens: get valid X/Y pos
	Vector3D vcrnr = GetCnrVector( csEditstr ) ;

	//3 tokens are considered to be [X Position] [Y Position] [Z Position]
	if( numtokens == 3 )
	{
		//prepend item num & add to end
		itemnum = numitems + 1 ;
		FormatNumStr( csItemNum, itemnum ) ;
		newitmstr.Format( m_csCoordFmtStr, vcrnr.x, vcrnr.y, vcrnr.z, 0, 0, 0 ) ;
		csEditstr = csItemNum + newitmstr ;
		combo.AddString( csEditstr ) ;
		itemnum = GetItemNum( csEditstr ) ;
	}

	// 6 tokens are assumed to be [X Position] [Y Position] [Z Position] [R] [G] [B]
	else if( numtokens == 6 )
	{
		COLORREF col = GetCnrColor( csEditstr ) ;
		int R = GetRValue( col ) ;
		int G = GetGValue( col ) ;
		int B = GetBValue( col ) ;

		//prepend item num & add to end
		itemnum = numitems + 1 ;
		FormatNumStr( csItemNum, itemnum ) ;
		newitmstr.Format( m_csCoordFmtStr, vcrnr.x, vcrnr.y, vcrnr.z, R, G, B ) ;
		csEditstr = csItemNum + newitmstr ;
		combo.AddString( csEditstr ) ;
		itemnum = GetItemNum( csEditstr ) ;
	}

	//otherwise we assume  [Item #] [X Position] [Y Position] [Z Position] [R] [G] [B]
	else
	{
		itemnum = GetItemNum( csEditstr ) ;
		COLORREF col = GetCnrColor( csEditstr ) ;
		int R = GetRValue( col ) ;
		int G = GetGValue( col ) ;
		int B = GetBValue( col ) ;

		//insert at item number or add to end
		if( itemnum > numitems )
		{
			//prepend correct item num & add to end
			itemnum = numitems + 1 ;
			FormatNumStr( csItemNum, itemnum ) ;
			newitmstr.Format( m_csCoordFmtStr, vcrnr.x, vcrnr.y, vcrnr.z, R, G, B ) ;
			csEditstr = csItemNum + newitmstr ;
			combo.AddString( csEditstr ) ;

		}
		else if( itemnum < numitems )
		{
			combo.DeleteString( itemnum - 1 ) ;
			FormatNumStr( csItemNum, itemnum ) ;
			newitmstr.Format( m_csCoordFmtStr, vcrnr.x, vcrnr.y, vcrnr.z, R, G, B ) ;
			csEditstr = csItemNum + newitmstr ;
			combo.InsertString( itemnum - 1, csEditstr ) ;
		}
		else //itemnum points to last string
		{
			combo.DeleteString( itemnum - 1 ) ;
			FormatNumStr( csItemNum, itemnum ) ;
			newitmstr.Format( m_csCoordFmtStr, vcrnr.x, vcrnr.y, vcrnr.z, R, G, B ) ;
			csEditstr = csItemNum + newitmstr ;
			combo.AddString( csEditstr ) ;
		}
	}

	//leave this item selected
	combo.SetCurSel( itemnum - 1 ) ;

	UpdateControls() ; //added 07/10/03
}


void CPolyDlg::FormatNumStr( CString& numstr, int num )
{
	//Purpose: fmt integer for item number in combo list item string
	//Notes:
	//	02/13/02: suddenly, 1000 doesn't seem like such a large number!
	//	Revised to increase capacity to 10,000 points

	CString csSpacer ;

	//02/13/02 added one space to each of 1st 3 lines, & added 4th line
	if( num < 10 ) csSpacer = "     " ;
	else if( num < 100 ) csSpacer = "   " ;
	else if( num < 1000 ) csSpacer = "  " ;
	else if( num < 10000 ) csSpacer = " " ; //added 02/13/02
	numstr.Format( "%d", num ) ;
	numstr = numstr + csSpacer ;
}

int CPolyDlg::RenumberList( CComboBox* pCombo )
{
	//Purpose: renumber points after combobox insert/delete operations
	//Inputs: pointer to combobox to renumber
	//Outputs:
	//	1) renumbered point list
	//	2) returns number of points in list
	//	3) item highlighting removed

	CString itmstr, itmnumstr, coordstr ;
	int itemnum ;

	int numitems = pCombo->GetCount() ;
	for( int itmidx = 0; itmidx < numitems; itmidx++ )
	{
		pCombo->GetLBText(itmidx, itmstr ) ;
		if( ( itemnum = GetItemNum( itmstr ) ) <= 0 )
		{
			AfxMessageBox( itmstr + " is an invalid item!" ) ;
		}
		else
		{
			//09/20/03 rev to extract item# & cnr vals & then
			//reconstruct string using m_csCoordFmtStr
			FormatNumStr( itmnumstr, itmidx+1 ) ;
			Vector3D v = GetCnrVector( itmstr ) ;
			coordstr.Format( m_csCoordFmtStr, v.x, v.y, v.z ) ;
			pCombo->DeleteString( itmidx ) ;
			pCombo->InsertString( itmidx, itmnumstr+coordstr ) ;
		}
	}

	pCombo->SetCurSel( -1 ) ;

	return numitems ; //changed 6/12/00
}

int CPolyDlg::GetItemNum( CString itemstr )
{
	itemstr.TrimLeft() ; itemstr.TrimRight() ;
	int spcidx = itemstr.Find( ' ' ) ;
	CString numstr = itemstr.Left( spcidx ) ;
	int itemnum = atoi( numstr ) ;
	return itemnum ;
}

int CPolyDlg::GetNumTokens( CString& instr )
{
	//typical string has "[item num] [ZPosition] [Radius]"
	//tokens must be separated by spaces
	CString str, tempstr ;
	str = instr ;
	int numtokens = 0 ;

	str.TrimRight() ; str.TrimLeft() ;

	while( str.GetLength() > 0 )
	{
		tempstr = "" ; //reset for next iteration
		int spaceidx = str.Find(' ') ;
		if( spaceidx > 0 )
		{
			tempstr = str.Left( spaceidx ) ; //extract substring
			if( tempstr.GetLength() > 0 ) numtokens++ ;
			str = str.Right( str.GetLength() - spaceidx - 1 ) ;
			str.TrimRight() ; str.TrimLeft() ;
		}
		else //no delimeters found, but could still be one token left
		{
			str.TrimRight() ; str.TrimLeft() ;
			if( str.GetLength() > 0 ) numtokens++ ;
			str = "" ; //zero it out
		}
	}

	return numtokens ;
}

void CPolyDlg::SetItemNum( CString& str, const int num )
{
	//Inputs:
	//	str = contains depth & possibly groove number substrings
	//Outputs:
	//	str = contains depth & groove number substrings
	//Notes:
	//	02/13/02: revised to fix a bug excited with profiles of more than 99 points

	CString numstr, itemstr ;
	str.TrimLeft() ; str.TrimRight() ;
	int spcidx = str.Find( ' ' ) ; //02/13/02 bugfix
	FormatNumStr( numstr, num ) ;

	if( spcidx >= 0 )
	{
		itemstr = str.Right( str.GetLength() - spcidx - 1 ) ;
	}
	else
	{
		itemstr = str ;
	}

	itemstr.TrimLeft() ; itemstr.TrimRight() ;
	str = numstr + itemstr ;
}

int CPolyDlg::FindDupCrnr( Vector3D crnr, CComboBox& combo )
{
	//06/20/03 don't allow duplicate points
	int numsegs = combo.GetCount() ;
	for (int i = 0; i < numsegs; i++ )
	{
		CString csInstr;
		combo.GetLBText(i, csInstr );
		int nCornerNum = GetItemNum( csInstr ) ;
		Vector3D v = GetCnrVector( csInstr ) ;
		if (v.x == crnr.x && v.y == crnr.y )//Matching coordinate set found
		{	//If match found, return corner number
			return nCornerNum ;
		}
	}

	return -1 ; //no match found
}

Vector3D CPolyDlg::GetCnrVector( CString cnrstr )
{
	//Extract 3D coordinates from vertex string.  String is assumed to be:
	//	"[cnrnum] [v.x] [v.y] [v.z] R G B"

	istrstream lin( cnrstr.GetBuffer( cnrstr.GetLength() ) ) ;

	//extract & discard corner number if necessary
	if( GetNumTokens( cnrstr ) > 6 )
	{
		int cnrnum ;
		lin >> cnrnum ;
	}

	Vector3D v ;
	lin >> v ; //put coords into temp Vector3D object
	cnrstr.ReleaseBuffer() ;

	return v ;
}

COLORREF CPolyDlg::GetCnrColor( CString& cnrstr )
{
	//extract R,G,B values from vertex string Vector3D. typical string is;
	//	"[cnrnum] [v.x] [v.y] [v.z] R G B"

	istrstream lin( cnrstr.GetBuffer( cnrstr.GetLength() ) ) ;

	//extract & discard corner number if necessary
	if( GetNumTokens( cnrstr ) > 6 )
	{
		int cnrnum ;
		lin >> cnrnum ;
	}

	Vector3D v ;
	lin >> v ; //put coords into temp Vector3D object

	int R, G, B ;
	lin >> R >> G >> B ;
	cnrstr.ReleaseBuffer() ;

	return RGB( R, G, B ) ;
}

void CPolyDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpdis ) 
{
	int colradius = 10 ; //color dot radius in pixels
	CBrush dotbrush ;
	COLORREF selcol = RGB(255,0,0) ; //use red color for selected value

	if( nIDCtl == IDC_BUTTON1 )
	{
		CDC dc ;
		dc.Attach( lpdis->hDC ) ;
		CRect rcItem = lpdis->rcItem ;
		int ht = rcItem.Height() ;
		int wdth = rcItem.Width() ;

		//dummy operation to save the old brush
		dotbrush.CreateSolidBrush( RGB(0,0,0) ) ;
		CBrush* pOldbrush = dc.SelectObject( &dotbrush ) ;
		dotbrush.DeleteObject() ;

		CPoint ctr = CPoint( rcItem.Width()/2, rcItem.Height()/2 ) ;

		if( m_Combo_Corner.GetCount() > 2 )
		{
			Vector3D v ;
			Vector3D vmin = Vector3D( FLT_MAX, FLT_MAX, FLT_MAX ) ;
			Vector3D vmax = Vector3D( -FLT_MAX, -FLT_MAX, -FLT_MAX ) ;
			int numcnrs = m_Combo_Corner.GetCount() ;
			for( int cnridx = 0; cnridx < numcnrs; cnridx++ )
			{
				CString cnrstr;
				m_Combo_Corner.GetLBText(cnridx, cnrstr );
				v = GetCnrVector( cnrstr ) ;
				vmin.x = min( vmin.x, v.x ) ;
				vmin.y = min( vmin.y, v.y ) ;
				vmax.x = max( vmax.x, v.x ) ;
				vmax.y = max( vmax.y, v.y ) ;
			}

			//include origin in extents calc so axis lines always visible
			vmin.x = min( vmin.x, 0.f ) ;
			vmin.y = min( vmin.y, 0.f ) ;
			vmax.x = max( vmax.x, 0.f ) ;
			vmax.y = max( vmax.y, 0.f ) ;

			
			vmin.z = vmax.z = 0.f ;

			Vector3D Vctr = (vmax + vmin ) / 2.f ;
			Vector3D v2 = vmax - Vctr ;
			float radius = v2.Abs() ;

			CPoint pt ;
			CString cnrstr;
			for( cnridx = 0; cnridx < numcnrs; cnridx++ )
			{
				m_Combo_Corner.GetLBText(cnridx, cnrstr );
				v = GetCnrVector( cnrstr ) ;
				COLORREF col = GetCnrColor( cnrstr ) ;
				dotbrush.CreateSolidBrush( col ) ;
				dc.SelectObject( &dotbrush ) ;
				
				pt.x = int( ( ( ( v.x - Vctr.x ) / radius ) + 1.f ) * wdth/2 ) ;
				pt.y = int( ( ( -( v.y - Vctr.y ) / radius ) + 1.f ) * ht/2 ) ;

				if( cnridx == 0 )
				{
					dc.MoveTo( pt ) ;
					dc.Ellipse( pt.x - colradius, pt.y - colradius, pt.x + colradius, pt.y + colradius ) ;
				}
				else
				{
					dc.LineTo( pt ) ;
					dc.Ellipse( pt.x - colradius, pt.y - colradius, pt.x + colradius, pt.y + colradius ) ;
				}
				
				dotbrush.DeleteObject() ;
			}

			
			//close the figure
			m_Combo_Corner.GetLBText(0, cnrstr );
			v = GetCnrVector( cnrstr ) ;
			pt.x = int( ( ( ( v.x - Vctr.x ) / radius ) + 1.f ) * wdth/2 ) ;
			pt.y = int( ( ( -( v.y - Vctr.y ) / radius ) + 1.f ) * ht/2 ) ;
			dc.LineTo( pt ) ;

			//draw the X/Y axis lines
			CPoint Xpt, Ypt, Cenpt ;
			Vector3D vx = Vector3D( radius*0.8f, 0.f, 0.f ) ;
			Vector3D vy = Vector3D( 0.f, radius*0.8f, 0.f ) ;

			Xpt.x = int( ( ( ( vx.x - Vctr.x ) / radius ) + 1.f ) * wdth/2 ) ;
			Xpt.y = int( ( ( -( vx.y - Vctr.y ) / radius ) + 1.f ) * ht/2 ) ;
			Ypt.x = int( ( ( ( vy.x - Vctr.x ) / radius ) + 1.f ) * wdth/2 ) ;
			Ypt.y = int( ( ( -( vy.y - Vctr.y ) / radius ) + 1.f ) * ht/2 ) ;
			Cenpt.x = int( ( ( ( 0.f - Vctr.x ) / radius ) + 1.f ) * wdth/2 ) ;
			Cenpt.y = int( ( ( -( 0.f - Vctr.y ) / radius ) + 1.f ) * ht/2 ) ;

			CPen axispen ;
			axispen.CreatePen( PS_SOLID, 0, RGB( 255, 0, 255 ) ) ;
			CPen* poldpen = (CPen*)dc.SelectObject( &axispen ) ;

			dc.MoveTo( Cenpt ) ;
			dc.LineTo( Xpt ) ;
			dc.MoveTo( Cenpt ) ;
			dc.LineTo( Ypt ) ;

			// Create default fonts      
			LOGFONT lf ;
			lf.lfHeight = -10 ;
			lf.lfWidth = 0 ;
			lf.lfEscapement = 0 ;
			lf.lfOrientation = 0 ;
			lf.lfWeight = FW_NORMAL ;
			lf.lfItalic = FALSE ;
			lf.lfUnderline = FALSE ;
			lf.lfStrikeOut = FALSE ;
			lf.lfCharSet = ANSI_CHARSET ;
			lf.lfOutPrecision = OUT_TT_PRECIS ;
			lf.lfClipPrecision = CLIP_CHARACTER_PRECIS ;
			lf.lfQuality = DEFAULT_QUALITY ;
			lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN | TMPF_TRUETYPE ;
			strcpy( lf.lfFaceName, "Courier New" ) ; 
			lf.lfWeight = FW_MEDIUM ;
			lf.lfHeight = lf.lfHeight = -15 ;


			CFont font, *poldfont ;
			font.CreateFontIndirect( &lf ) ; //added 2/9/00
			poldfont = (CFont*)dc.SelectObject( &font ) ;
			dc.SetTextColor( RGB( 255, 0, 255 ) ) ;
			dc.SetTextAlign( TA_CENTER | TA_BASELINE ) ; 
			dc.SetBkMode( TRANSPARENT ) ;
			dc.TextOut( Xpt.x, Xpt.y, "x" ) ; // write out axis text
			dc.TextOut( Ypt.x, Ypt.y, "y" ) ;
			dc.SetBkMode( OPAQUE ) ; //Added 11/24/96 to fix dashed line bug.  See notes pg 38
			dc.SelectObject( poldfont ) ;
			font.DeleteObject() ;
		}
	}
	CDialog::OnDrawItem(nIDCtl, lpdis);
}

HBRUSH CPolyDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	CWnd* pParent = pWnd->GetParent() ;
	if( pWnd->m_hWnd == m_ContourList.m_hWnd )
	{
		int numcontours =  m_ContourList.GetCount() ;
		int ctrselidx = m_ContourList.GetCurSel() ;
		BOOL bContourAvail = ( numcontours > 0 && ctrselidx >= 0 && ctrselidx < numcontours ) ;
		
		if( bContourAvail )
		{
			hbr = HBRUSH( m_OnBrush ) ;
		}
		else
		{
			hbr = HBRUSH( m_OffBrush ) ;
		}
	}

	return hbr;
}

void CPolyDlg::OnOK() 
{
	//Update current contour object from points list
	POSITION pos = m_ContourPtrList.FindIndex( m_LastContourSelIdx ) ;

	if( pos )
	{
		CContour* pCtr = (CContour*)m_ContourPtrList.GetAt( pos ) ;
		int numcnrs = pCtr->m_NumVertex ;

		for( int cnridx = 0; cnridx < numcnrs; cnridx++ )
		{
			GLdouble* pcnr = pCtr->m_ppVertexArray[ cnridx ] ; //pcnr pts to a GLdouble[6] array
			delete[] pcnr ;
			pcnr = NULL ;
		}
		delete[] pCtr->m_ppVertexArray ;
		pCtr->m_ppVertexArray = NULL ;

		numcnrs = m_Combo_Corner.GetCount() ;
		pCtr->m_ppVertexArray = new GLdouble*[ numcnrs ] ;
		pCtr->m_NumVertex = numcnrs ;

		for( cnridx = 0; cnridx < numcnrs; cnridx++ )
		{
			GLdouble* pcnr = new GLdouble[6] ; //pcnr pts to a GLdouble[6] array
			CString cnrstr ;
			m_Combo_Corner.GetLBText( cnridx, cnrstr ) ;
			Vector3D vcnr = GetCnrVector( cnrstr ) ;
			COLORREF col = GetCnrColor( cnrstr ) ;
			pcnr[0] = (GLdouble)vcnr.x ;
			pcnr[1] = (GLdouble)vcnr.y ;
			pcnr[2] = (GLdouble)vcnr.z ;
			pcnr[3] = (GLdouble)GetRValue( col ) / 255. ;
			pcnr[4] = (GLdouble)GetGValue( col ) / 255. ;
			pcnr[5] = (GLdouble)GetBValue( col ) / 255. ;
			
			pCtr->m_ppVertexArray[ cnridx ] = pcnr ;
		}

		m_Combo_Corner.ResetContent() ; //dont do this if last selidx was invalid
	}

	CDialog::OnOK();
}

BOOL CPolyDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//Purpose: Calls WinHelp for dlg.
	//Provenance: Copyright 07/05/03 G. Frank Paynter

	if( LOWORD(wParam) == ID_HELP )
	{
		CString str(AfxGetApp()->m_pszHelpFilePath) ;
		str += " >Second" ;
		::WinHelp( this->m_hWnd, str, HELP_CONTEXT, (0x20000 + CPolyDlg::IDD ) ) ;

		return 0 ;
	}

	return CDialog::OnCommand(wParam, lParam);
}

