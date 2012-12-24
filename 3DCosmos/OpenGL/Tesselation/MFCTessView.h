// MFCTessView.h : interface of the CMFCTessView class
//
/////////////////////////////////////////////////////////////////////////////
#include "Vector3d.h"


#if !defined(AFX_MFCTESSVIEW_H__BD9D0D03_4E9B_4009_89A7_CDBF2EAD577B__INCLUDED_)
#define AFX_MFCTESSVIEW_H__BD9D0D03_4E9B_4009_89A7_CDBF2EAD577B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMFCTessView : public CView
{
protected: // create from serialization only
	CMFCTessView();
	DECLARE_DYNCREATE(CMFCTessView)

// Attributes
public:
	CMFCTessDoc* GetDocument();

protected:
//Required OpenGL memvars
	HGLRC m_hRC; //Rendering Context
	CDC* m_pDC;  //Device Context

	Vector3D m_Center ;
	float AxisSize ;
	BOOL ShowAxis ;
	float Radius ;
	int m_WindingRule ;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMFCTessView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

	BOOL InitOpenGL();
	BOOL SetupPixelFormat();
	void CalculateExtents() ;

	//tesselation support functions
	static void CALLBACK beginCallback(GLenum which) ;
	static void CALLBACK endCallback(void) ;
	static void CALLBACK errorCallback(GLenum errorCode) ;
	static void CALLBACK vertexCallback(GLvoid *vertex) ;
	static void CALLBACK combineCallback(GLdouble coords[3], 
						 GLdouble *vertex_data[4],
						 GLfloat weight[4], GLdouble **dataOut ) ;

	static CPtrList gm_VertexPtrList ; //must be static for use in combineCallback()


// Implementation
public:
	virtual ~CMFCTessView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMFCTessView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPopupWindNeg();
	afx_msg void OnUpdatePopupWindNeg(CCmdUI* pCmdUI);
	afx_msg void OnPopupWindNonZero();
	afx_msg void OnUpdatePopupWindNonZero(CCmdUI* pCmdUI);
	afx_msg void OnPopupWindOdd();
	afx_msg void OnUpdatePopupWindOdd(CCmdUI* pCmdUI);
	afx_msg void OnPopupWindPos();
	afx_msg void OnUpdatePopupWindPos(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in MFCTessView.cpp
inline CMFCTessDoc* CMFCTessView::GetDocument()
   { return (CMFCTessDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCTESSVIEW_H__BD9D0D03_4E9B_4009_89A7_CDBF2EAD577B__INCLUDED_)
