// MFCTessDoc.cpp : implementation of the CMFCTessDoc class
//

#include "stdafx.h"
#include "MFCTess.h"

#include "MFCTessDoc.h"
#include "PolyDlg.h"
#include "Contour.h"
#include "strstrea.h"  

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMFCTessDoc

IMPLEMENT_DYNCREATE(CMFCTessDoc, CDocument)

BEGIN_MESSAGE_MAP(CMFCTessDoc, CDocument)
	//{{AFX_MSG_MAP(CMFCTessDoc)
	ON_COMMAND(ID_MENU_DEFPOLY, OnMenuDefPoly)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMFCTessDoc construction/destruction

CMFCTessDoc::CMFCTessDoc()
{
	// TODO: add one-time construction code here

}

CMFCTessDoc::~CMFCTessDoc()
{
	//02/09/05 added as part of mem leak fix
	while ( m_ContourList.GetCount() > 0 )
	{
		CContour* pCtr = (CContour*)m_ContourList.RemoveHead() ;
		delete pCtr ;
		pCtr = NULL ;
	}
}

BOOL CMFCTessDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMFCTessDoc serialization


void CMFCTessDoc::Serialize(CArchive& ar)
{
	int numctrs = 0 ;
	
	if (ar.IsStoring())
	{
		//01/14/05: don't delete objs on store - done in load or destructor
		numctrs = m_ContourList.GetCount() ;
		if( numctrs > 0 )
		{
			POSITION pos = m_ContourList.GetHeadPosition() ;
			while( pos )
			{
				CString hdrstr = "\nCONTOUR: \n" ;
				ar.WriteString( hdrstr ) ;
				CContour* pCtr = (CContour*)m_ContourList.GetNext( pos );
				pCtr->Serialize( ar ) ;
			}
		}

	}
	else
	{
		//first remove/delete any existing contour objects
		numctrs = m_ContourList.GetCount() ;
		if( numctrs > 0 )
		{
			while ( m_ContourList.GetCount() > 0 )
			{
				CContour* pCtr = (CContour*)m_ContourList.RemoveHead() ;
				delete pCtr ;
				pCtr = NULL ;
			}
		}

		//now load contours from file, creating new objs as necessary
		CString instr ;
		BOOL bStatus = TRUE ;
		while( bStatus )
		{
			bStatus = ar.ReadString( instr ) ;
			while( bStatus && instr.Find( "CONTOUR:" ) < 0 )
			{
				bStatus = ar.ReadString( instr ) ;
			}
			
			if( bStatus )
			{
				//at this point, instr contains "CONTOUR: "
				CContour* pCtr = new CContour ;
				m_ContourList.AddTail( pCtr ) ;
				pCtr->Serialize( ar ) ;
			}

		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMFCTessDoc diagnostics

#ifdef _DEBUG
void CMFCTessDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMFCTessDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMFCTessDoc commands

void CMFCTessDoc::OnMenuDefPoly() 
{
	CPolyDlg dlg ;

	//transfer countour pointers to dlg list
	//don't remove, as user may select CANCEL
	//01/15/05 must create copies via new to avoid problems
	POSITION pos = m_ContourList.GetHeadPosition() ;
	while ( pos ) 
	{
		CContour* pCtr = (CContour*)m_ContourList.GetNext( pos ) ;
		CContour* pDlgCtr = new CContour( pCtr ) ;
		dlg.m_ContourPtrList.AddTail( pDlgCtr ) ;

////DEBUG!!
//TRACE( "CContour %p xferred to dialog\n", pDlgCtr ) ;
//for( int vertidx = 0; vertidx < pCtr->m_NumVertex; vertidx++ )
//{
//	GLdouble* pVertex = pCtr->m_ppVertexArray[ vertidx ] ;
//TRACE( "    pVertex %p with vertex (%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f)\n",
//			pVertex, pVertex[0], pVertex[1], pVertex[2], pVertex[3], pVertex[4], pVertex[5] ) ;
//}
////DEBUG!!
	}

	if( dlg.DoModal() == IDOK )
	{
		//user selected OK, so now remove & delete any existing contour objects
		while ( m_ContourList.GetCount() > 0 ) 
		{
			CContour* pCtr = (CContour*)m_ContourList.RemoveHead() ;
			delete pCtr ;
			pCtr = NULL ;
		}

		ASSERT( m_ContourList.GetCount() == 0 ) ;

		//transfer pointers from dialog to doc
		POSITION pos = dlg.m_ContourPtrList.GetHeadPosition() ;
		while ( pos ) 
		{
			CContour* pCtr = (CContour*)dlg.m_ContourPtrList.GetNext( pos ) ;
			m_ContourList.AddTail( pCtr ) ;
		

////DEBUG!!
//TRACE( "CContour %p xferred from dialog\n", pCtr ) ;
//for( int vertidx = 0; vertidx < pCtr->m_NumVertex; vertidx++ )
//{
//	GLdouble* pVertex = pCtr->m_ppVertexArray[ vertidx ] ;
//TRACE( "    pVertex %p with vertex (%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f)\n",
//			pVertex, pVertex[0], pVertex[1], pVertex[2], pVertex[3], pVertex[4], pVertex[5] ) ;
//}
////DEBUG!!
		}

	}

	UpdateAllViews(NULL);

}
