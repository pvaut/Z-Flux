// MFCTessDoc.h : interface of the CMFCTessDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFCTESSDOC_H__A6C74D62_EF4D_4FF2_9B08_FBA15F5E05FA__INCLUDED_)
#define AFX_MFCTESSDOC_H__A6C74D62_EF4D_4FF2_9B08_FBA15F5E05FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CContour ; //fwd decl
class CMFCTessDoc : public CDocument
{
protected: // create from serialization only
	CMFCTessDoc();
	DECLARE_DYNCREATE(CMFCTessDoc)

// Attributes
public:
	CObList m_ContourList ; //list of pointers to GLdouble arrays (pointers)

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMFCTessDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMFCTessDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMFCTessDoc)
	afx_msg void OnMenuDefPoly();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCTESSDOC_H__A6C74D62_EF4D_4FF2_9B08_FBA15F5E05FA__INCLUDED_)
