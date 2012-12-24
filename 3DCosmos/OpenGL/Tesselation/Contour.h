// Contour.h: interface for the CContour class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTOUR_H__B56D48E4_99A5_4E94_BF5C_769A76027B9B__INCLUDED_)
#define AFX_CONTOUR_H__B56D48E4_99A5_4E94_BF5C_769A76027B9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MFCTess.h"

class CContour : public CObject
{

public:
	CContour();
	CContour( CContour* pOther ) ;
	virtual ~CContour();

//Attributes
public:
	GLdouble** m_ppVertexArray ;
	int m_NumVertex ;
	CString m_csContourName ;

//Implementation
	virtual void Serialize(CArchive& ar) ;
   
DECLARE_SERIAL( CContour )

};

#endif // !defined(AFX_CONTOUR_H__B56D48E4_99A5_4E94_BF5C_769A76027B9B__INCLUDED_)
