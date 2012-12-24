// Contour.cpp: implementation of the CContour class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MFCTess.h"
#include "Contour.h"
#include "strstrea.h"  
#include "forio.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CContour,CObject,1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CContour::CContour()
:m_ppVertexArray( NULL ),
 m_NumVertex( 0 ),
 m_csContourName( "Contour" )
{
//DEBUG!!
TRACE( "CContour: Creating CContour %p\n", this ) ;
//DEBUG!!

}

CContour::CContour( CContour* pOther )
:m_ppVertexArray( NULL ),
 m_NumVertex( 0 ),
 m_csContourName( "Contour" )
{
//DEBUG!!
TRACE( "CContour: Creating CContour %p From %p\n", this, pOther ) ;
//DEBUG!!

	m_csContourName = pOther->m_csContourName ;
	m_NumVertex = pOther->m_NumVertex ;
	m_ppVertexArray = new GLdouble*[ m_NumVertex ] ;
	for( int vertidx = 0; vertidx < m_NumVertex; vertidx++ )
	{
		m_ppVertexArray[vertidx] = new GLdouble[6] ;
		for( int idx = 0; idx < 6; idx++ )
		{
			m_ppVertexArray[vertidx][idx] = pOther->m_ppVertexArray[vertidx][idx] ;
		}
		TRACE( "vertex[%d] = (%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f)\n",vertidx,

			m_ppVertexArray[vertidx][0],m_ppVertexArray[vertidx][1],m_ppVertexArray[vertidx][2],
			m_ppVertexArray[vertidx][3],m_ppVertexArray[vertidx][4],m_ppVertexArray[vertidx][5]);
	}


}

CContour::~CContour()
{
	//delete any leftover objects
//DEBUG!!
TRACE( "CContour dtor: Deleting m_ppVertexArray %p containing %d vertices\n", m_ppVertexArray, m_NumVertex ) ;
//DEBUG!!

	
	for( int vertidx = 0; vertidx < m_NumVertex; vertidx++ )
	{
		GLdouble* pVertex = m_ppVertexArray[ vertidx ] ;

//DEBUG!!
TRACE( "     Deleting pVertex %p with vertex (%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f)\n",
			pVertex, pVertex[0], pVertex[1], pVertex[2], pVertex[3], pVertex[4], pVertex[5] ) ;
//DEBUG!!

		delete[] pVertex ;
	}

	delete[] m_ppVertexArray ;
	m_ppVertexArray = NULL ;

//DEBUG!!
TRACE( "CContour: Deleting CContour %p\n", this ) ;
//DEBUG!!

}


void CContour::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ostrstream os ;	// create a dynamic string buffer
		os.flags( os.flags() | ios::showpoint ) ; //set flags to show dec pt

		os << m_csContourName << endl ;
		os << m_NumVertex << endl ;

		//iterate through edgelist & build string containing all corner coords
		for ( int vertidx = 0; vertidx < m_NumVertex; vertidx++ ) 
		{
			GLdouble* pVertex = m_ppVertexArray[ vertidx ] ;
			os << pVertex[ 0 ] << ofc //x
				<< pVertex[1] << ofc  //y
				<< pVertex[2] << ofc  //z
				<< pVertex[3] << ofc  //R
				<< pVertex[4] << ofc  //G
				<< pVertex[5] << ofc  //B
				<< endl ; //terminate the string

////DEBUG!!
//TRACE( "    pVertex %p with vertex (%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f)\n",
//			pVertex, pVertex[0], pVertex[1], pVertex[2], pVertex[3], pVertex[4], pVertex[5] ) ;
////DEBUG!!

		}
		ar.WriteString( os.str() ) ;
	}
	else
	{
		BOOL bStatus = TRUE ;
		CString csReadline ;
		bStatus = ar.ReadString( csReadline ) ;

		//contour name
		if( bStatus )
		{
			csReadline.TrimLeft() ; csReadline.TrimRight() ;
			m_csContourName = csReadline ;
		}
		
		//number of vertices
		bStatus = ar.ReadString( csReadline ) ;
		if( bStatus )
		{
			csReadline.TrimLeft() ; csReadline.TrimRight() ;
			m_NumVertex = atoi( csReadline ) ;
		}

		
		//vertex info
		m_ppVertexArray = new GLdouble*[ m_NumVertex ] ;

////DEBUG!!
//TRACE( "     Creating new m_ppVertexArray[%d] %p\n", 
//	  m_NumVertex, m_ppVertexArray ) ;
////DEBUG!!		

		for( int cnridx = 0; cnridx < m_NumVertex; cnridx++ )
		{
			bStatus = ar.ReadString( csReadline ) ;
			if( bStatus )
			{
				int bufferlen = csReadline.GetLength(); //gets the length of this line
				istrstream is( csReadline.GetBuffer(bufferlen), bufferlen );
				GLdouble* pVertex = new GLdouble[6] ;
				is >> pVertex[0] >> ofc >> pVertex[1] >> ofc >> pVertex[2] >> ofc >> 
					pVertex[3] >> ofc >> pVertex[4] >> ofc >> pVertex[5] >> feol ;
				csReadline.ReleaseBuffer() ;
				m_ppVertexArray[ cnridx ] = pVertex ;

////DEBUG!!
//TRACE( "    new GLdouble[6] %p with vertex (%2.3f,%2.3f,%2.3f,%2.3f,%2.3f,%2.3f)\n",
//			pVertex, pVertex[0], pVertex[1], pVertex[2], pVertex[3], pVertex[4], pVertex[5] ) ;
////DEBUG!!
			}
		}
	}
}
