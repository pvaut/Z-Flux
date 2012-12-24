// ColorComboBox.h : header file
//
// Copyright (C) 2002-2004  William Braynen
// Contact information can be found at www.samdurak.com
//
// The CColorComboBox MFC class consists of two files:
// ColorComboBox.cpp and ColorComboBox.h.  It is a subclass of
// CComboBox and allows to change the background and text colors of a
// Windows combo box control.  However, it changes the text color of
// all the text and does not implement multi-colored text or
// multi-colored backgrounds.
//
// Use this class just as you would a regular CComboBox class.
// Just replace 'CComboBox' in the declaration by 'CColorComboBox'.
// And don't forget to #include ColorComboBox.h (this header file).
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#if !defined(AFX_COLORCOMBOBOX_H__AC00514A_D5EA_489F_ADA6_7A8D177CB51C__INCLUDED_)
#define AFX_COLORCOMBOBOX_H__AC00514A_D5EA_489F_ADA6_7A8D177CB51C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CColorComboBox window

class CColorComboBox : public CComboBox
{
// Construction
public:
	CColorComboBox (COLORREF backgroundColor = 0, COLORREF textColor = 0);

// Attributes
public:
	BOOL m_bEnable ; //gfp addition

protected:
	//corner list enable/disable colors
	CBrush m_OnBrush, m_OffBrush ;

// Operations
public:
	void SetBackgroundColor (COLORREF color);
	void SetTextColor (COLORREF color);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorComboBox)
	public:
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorComboBox();

	// Generated message map functions
protected:
  CBrush m_brush;
  COLORREF m_backgroundColor;
  COLORREF m_textColor;
	//{{AFX_MSG(CColorComboBox)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORCOMBOBOX_H__AC00514A_D5EA_489F_ADA6_7A8D177CB51C__INCLUDED_)

