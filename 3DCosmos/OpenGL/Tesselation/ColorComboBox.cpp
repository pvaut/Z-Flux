// ColorComboBox.cpp : implementation file
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
// And don't forget to #include ColorComboBox.h
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

#include "stdafx.h"
#include "ColorComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define RGB_DEFAULT_BACKGROUND RGB(0,0,0)
//#define RGB_DEFAULT_TEXT       RGB(255,255,255)
#define RGB_DEFAULT_BACKGROUND RGB(255,255,255)
#define RGB_DEFAULT_TEXT       RGB(0,0,0)

/////////////////////////////////////////////////////////////////////////////
// CColorComboBox

CColorComboBox::CColorComboBox (COLORREF backgroundColor, COLORREF textColor)
:m_bEnable( TRUE )
{
  if (0 == backgroundColor && 0 == textColor)
  {
    m_backgroundColor = RGB_DEFAULT_BACKGROUND;
    m_textColor       = RGB_DEFAULT_TEXT;

	m_OnBrush.CreateSolidBrush( GetSysColor( COLOR_WINDOW ) )  ;
	m_OffBrush.CreateSolidBrush( GetSysColor( COLOR_3DFACE ) )  ;
  }

  // Create a green brush for the background for the class of controls:
  m_brush.CreateSolidBrush (m_backgroundColor);
}

CColorComboBox::~CColorComboBox()
{
  m_brush.DeleteObject();
}


BEGIN_MESSAGE_MAP(CColorComboBox, CComboBox)
	//{{AFX_MSG_MAP(CColorComboBox)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorComboBox message handlers

BOOL CColorComboBox::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
  if (WM_CTLCOLOR != message && WM_CTLCOLOREDIT != message)
  {
    return CComboBox::OnChildNotify(message, wParam, lParam, pLResult);
  }
    
  // Set the text foreground to red:
  HDC hdcChild = (HDC)wParam;
  ::SetTextColor(hdcChild, m_textColor);

  // Set the text background to green:
  ::SetBkColor(hdcChild, m_backgroundColor);

  // Send what would have been the return value of OnCtlColor() - the
  // brush handle - back in pLResult:
  *pLResult = (LRESULT)(m_brush.GetSafeHandle());

  // Return TRUE to indicate that the message was handled:
  return TRUE;
}

void CColorComboBox::SetBackgroundColor(COLORREF color)
{
  // Create a green brush for the background for the class of controls:
  m_brush.DeleteObject();
  m_brush.CreateSolidBrush (color);

  m_backgroundColor = color;
}

void CColorComboBox::SetTextColor(COLORREF color)
{
  m_textColor = color;
}

////
//
// Changes the background color of an OPEN drop-down list
//
HBRUSH CColorComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here

  if( nCtlColor == CTLCOLOR_LISTBOX || nCtlColor == CTLCOLOR_EDIT || nCtlColor == CTLCOLOR_MSGBOX )
  {
    hbr = m_brush;
    pDC->SetTextColor (m_textColor);
		pDC->SetBkColor (m_backgroundColor);

	if( m_bEnable )
	{
		hbr = HBRUSH( m_OnBrush ) ;
	}
	else
	{
		hbr = HBRUSH( m_OffBrush ) ;
	}
  }

	// TODO: Return a different brush if the default is not desired
	return hbr; //m_brush;
}

