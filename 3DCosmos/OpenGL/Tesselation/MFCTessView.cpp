// MFCTessView.cpp : implementation of the CMFCTessView class
//

#include "stdafx.h"
#include "MFCTess.h"
#include "Contour.h"
#include <float.h>
#include <math.h>
#include <afxcoll.h>
#include "MFCTessDoc.h"
#include "MFCTessView.h"
//#include "PolyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMFCTessView

IMPLEMENT_DYNCREATE(CMFCTessView, CView)

BEGIN_MESSAGE_MAP(CMFCTessView, CView)
	//{{AFX_MSG_MAP(CMFCTessView)
	ON_WM_CREATE()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(IDM_POPUP_WINDNEG, OnPopupWindNeg)
	ON_UPDATE_COMMAND_UI(IDM_POPUP_WINDNEG, OnUpdatePopupWindNeg)
	ON_COMMAND(IDM_POPUP_WINDNONZERO, OnPopupWindNonZero)
	ON_UPDATE_COMMAND_UI(IDM_POPUP_WINDNONZERO, OnUpdatePopupWindNonZero)
	ON_COMMAND(IDM_POPUP_WINDODD, OnPopupWindOdd)
	ON_UPDATE_COMMAND_UI(IDM_POPUP_WINDODD, OnUpdatePopupWindOdd)
	ON_COMMAND(IDM_POPUP_WINDPOS, OnPopupWindPos)
	ON_UPDATE_COMMAND_UI(IDM_POPUP_WINDPOS, OnUpdatePopupWindPos)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMFCTessView construction/destruction

CMFCTessView::CMFCTessView()
:m_hRC( NULL ), //Rendering Context
 m_pDC( NULL ),  //Device Context
 m_Center( Vector3D( 0.f, 0.f, 0.f ) ),
 AxisSize( 0.f ),
 ShowAxis( FALSE ),
 Radius( 1.f ),
 m_WindingRule( (int)GLU_TESS_WINDING_POSITIVE )
{
	// TODO: add construction code here

}

CMFCTessView::~CMFCTessView()
{
	delete m_pDC ;
}

BOOL CMFCTessView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMFCTessView drawing

void CMFCTessView::OnDraw(CDC* pDC)
{
	CMFCTessDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here

	//some per-drawing init stuff
	glLoadIdentity();

	//setup drawing dimensions, etc
	CPoint vptorg = m_pDC->GetViewportOrg() ; //note m_pDC, NOT pDC
	CRect rect ;
	GetWindowRect( &rect ) ;
	int dx = rect.Width() ;
	int dy = rect.Height() ;
	glViewport( 0, 0, dx, dy );

	glMatrixMode( GL_MODELVIEW );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	// create bitmaps for the device context font's first 256 glyphs 
	HDC hdc = m_pDC->GetSafeHdc() ;
	wglUseFontBitmaps(hdc, 0, 256, 1000); 
	glListBase(1000);

	float R ;
	if( pDoc->m_ContourList.GetCount() > 0 )
	{
		CalculateExtents() ; // Collect overall figure extents (in meters)
		R = Radius ;

		//11/29/03 experiment to eliminate probs w/non-square windows
		float glleft = -R ;
		float gltop = +R ;
		float nearplane = -R ; //rev 07/25/03
		float farplane = R ; //rev 07/25/03

		float glleft2 = glleft ;
		float gltop2 = gltop ;
		float glright2 = ( glleft + 2*R ) ;
		float glbottom2 = ( gltop - 2*R ) ;

		//adjust for non-square viewing window
		float dxdy = (float)dx/(float)dy ;  //=1.00 for square window
		if( dxdy > 1 )
		{
			glOrtho( glleft2*dxdy , glright2*dxdy, glbottom2, gltop2, nearplane, farplane ) ;
		}
		else
		{
			glOrtho( glleft2 , glright2, glbottom2/dxdy, gltop2/dxdy, nearplane, farplane ) ;
		}
		
		glTranslatef( -m_Center.x, -m_Center.y, -m_Center.z ) ;
	}
	else
	{  
		CString txtstr ;
		glOrtho( -1.f , 1.f, -1.f, 1.f, -1.f, 1.f ) ;
		glTranslatef( 0.f, 0.f, 0.f ) ;
		txtstr = "Nothing to draw" ;
		glColor3f( 0.f, 0.f, 0.f ) ;
		glRasterPos3f(0.0F, 0.0F, 0.0F ); 
		glCallLists(txtstr.GetLength(), GL_UNSIGNED_BYTE, txtstr); 

		//clean up & exit
		glFlush(); 
		glDeleteLists(1000, 256) ; 
		SwapBuffers( m_pDC->GetSafeHdc() );
		return ;
	}

	glPushMatrix() ;

	//01/09/05 implement tessellation
	GLUtesselator *tobj;
	tobj = gluNewTess();

	//added 02/09/05 as part of mem leak fix
	while( gm_VertexPtrList.GetCount() > 0 )
	{
		GLdouble* pV = (GLdouble*)gm_VertexPtrList.RemoveHead() ;
		delete[] pV ;
		pV = NULL ;
	}

	gluTessCallback(tobj, GLU_TESS_BEGIN, (void (CALLBACK *) ())beginCallback);
	gluTessCallback(tobj, GLU_TESS_VERTEX, (void (CALLBACK *) ()) vertexCallback);
	gluTessCallback(tobj, GLU_TESS_END, (void (CALLBACK *) ())endCallback);
	gluTessCallback(tobj, GLU_TESS_ERROR,(void (CALLBACK *) ())errorCallback);
	gluTessCallback(tobj, GLU_TESS_COMBINE, (void (CALLBACK *) ())combineCallback);

	glShadeModel(GL_SMOOTH);    
	gluTessProperty(tobj, GLU_TESS_WINDING_RULE,
               m_WindingRule );

	//only one polygon, but multiple contours
	gluTessBeginPolygon(tobj, NULL);


	POSITION pos = pDoc->m_ContourList.GetHeadPosition() ;
	while ( pos ) 
	{
		CContour* pCtr = (CContour*)pDoc->m_ContourList.GetNext( pos ) ;
		ASSERT( pCtr != NULL && pCtr->m_NumVertex > 0 && pCtr->m_ppVertexArray != NULL ) ;
		int numcorners = pCtr->m_NumVertex ;

		gluTessBeginContour(tobj);

		for( int cnridx = 0; cnridx < numcorners; cnridx++ )
		{
			GLdouble* glvert = pCtr->m_ppVertexArray[cnridx];
			gluTessVertex( tobj, glvert, glvert ) ;
		   TRACE( "Draw: Sent vertex(%f,%f,%f,%f,%f,%f)\n", glvert[0],glvert[1],glvert[2],glvert[3],glvert[4],glvert[5] ) ;
		}

		gluTessEndContour(tobj);
		
	} //end of all contour definitions

		gluTessEndPolygon(tobj);

	//clean up & exit
	glFlush(); 
	glDeleteLists(1000, 256) ; // delete our 256 glyph display lists 
	SwapBuffers( m_pDC->GetSafeHdc() );
	gluDeleteTess( tobj ) ;

	//added 02/09/05 as part of mem leak fix
	while( gm_VertexPtrList.GetCount() > 0 )
	{
		GLdouble* pV = (GLdouble*)gm_VertexPtrList.RemoveHead() ;
		delete[] pV ;
		pV = NULL ;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMFCTessView printing

BOOL CMFCTessView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMFCTessView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMFCTessView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CMFCTessView diagnostics

#ifdef _DEBUG
void CMFCTessView::AssertValid() const
{
	CView::AssertValid();
}

void CMFCTessView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCTessDoc* CMFCTessView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCTessDoc)));
	return (CMFCTessDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMFCTessView message handlers

int CMFCTessView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
// Initialize OpenGL when window is created.
	if ( !InitOpenGL() )
	{
		MessageBox( "Error setting up OpenGL!", "Init Error!",
			MB_OK | MB_ICONERROR );
		return -1;
	}
	
	return 0;
}

// Standard OpenGL Init Stuff
BOOL CMFCTessView::InitOpenGL()
{
	//Get a DC for the Client Area
	m_pDC = new CClientDC(this);

	//Failure to Get DC
	if( m_pDC == NULL )
		return FALSE;

	if( !SetupPixelFormat() )
		return FALSE;

    //Create Rendering Context
	m_hRC = ::wglCreateContext( m_pDC->GetSafeHdc() );

    //Failure to Create Rendering Context
    if( m_hRC == 0 )
        return FALSE;

	//Make the RC Current
	if( ::wglMakeCurrent( m_pDC->GetSafeHdc(), m_hRC ) == FALSE )
		return FALSE;

	// Usual OpenGL stuff
	glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
	glClearDepth(1.0f);
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_TEXTURE_2D );
	glLineWidth( 2.0f );
	glPointSize( 3.0f );

	return TRUE;
}

BOOL CMFCTessView::SetupPixelFormat()
{
	static PIXELFORMATDESCRIPTOR pfd = 
	{
		sizeof(PIXELFORMATDESCRIPTOR),    // size of this pfd
		1,                                // version number
		PFD_DRAW_TO_WINDOW |              // support window
		PFD_SUPPORT_OPENGL |              // support OpenGL
		PFD_DOUBLEBUFFER,                 // double buffered
		PFD_TYPE_RGBA,                    // RGBA type
		24,                               // 24-bit color depth
		0, 0, 0, 0, 0, 0,                 // color bits ignored
		0,                                // no alpha buffer
		0,                                // shift bit ignored
		0,                                // no accumulation buffer
		0, 0, 0, 0,                       // accumulation bits ignored
		16,                               // 16-bit z-buffer
		0,                                // no stencil buffer
		0,                                // no auxiliary buffer
		PFD_MAIN_PLANE,                   // main layer
		0,                                // reserved
		0, 0, 0                           // layer masks ignored
	};

   int m_nPixelFormat = ::ChoosePixelFormat( m_pDC->GetSafeHdc(), &pfd );

    if ( m_nPixelFormat == 0 )
        return FALSE;

    return ::SetPixelFormat( m_pDC->GetSafeHdc(), m_nPixelFormat, &pfd );
}

//01/06/05 tesselation support functions
void CALLBACK CMFCTessView::beginCallback(GLenum which)
{
   glBegin(which);
}

void CALLBACK CMFCTessView::errorCallback(GLenum errorCode)
{
   const GLubyte *estring;

   estring = gluErrorString(errorCode);
   fprintf(stderr, "Tessellation Error: %s\n", estring);
   exit(0);
}

void CALLBACK CMFCTessView::endCallback(void)
{
   glEnd();
}

void CALLBACK CMFCTessView::vertexCallback(GLvoid *vertex)
{
	GLdouble *pointer;

	pointer = (GLdouble *) vertex;
	glColor3dv(pointer+3);
	glVertex3dv(pointer);

	TRACE( "vertexCallback: (%f,%f,%f)\n",pointer[0],pointer[1],pointer[2] ) ;
}

/*  combineCallback is used to create a new vertex when edges
 *  intersect.  coordinate location is trivial to calculate,
 *  but weight[4] may be used to average color, normal, or texture
 *  coordinate data.  In this program, color is weighted.
 */
CPtrList CMFCTessView::gm_VertexPtrList ; //static memvar used to keep track of newly allocated vertices
void CALLBACK CMFCTessView::combineCallback(GLdouble coords[3], 
                     GLdouble *vertex_data[4],
                     GLfloat weight[4], GLdouble **dataOut )
{
	GLdouble *vertex = new GLdouble[6] ;
	gm_VertexPtrList.AddTail( vertex ) ; //keep track for later delete[] at bottom of CMFCTessView::OnDraw() 

	vertex[0] = coords[0];
	vertex[1] = coords[1];
	vertex[2] = coords[2];
	vertex[3] = vertex[4] = vertex[5] = 0. ; //01/13/05 bugfix

	*dataOut = vertex;
//   TRACE( "combine: returning (%f,%f,%f,%f,%f,%f)\n",vertex[0],vertex[1],vertex[2],vertex[3],vertex[4],vertex[5] ) ;
}


void CMFCTessView::CalculateExtents() 
{                    
//Purpose: To calculate total polygon extents and center
//Provenance: G. Frank Paynter 01/16/05

	POSITION pos ;
	Vector3D vmin = Vector3D( FLT_MAX, FLT_MAX, FLT_MAX ) ;	//smallest 3D point in accumulated view
	Vector3D vmax = -vmin ;	//largest 3D point in accumulated view
	CMFCTessDoc* pDoc = GetDocument() ;
	CContour* pCtr ;
	int numcnrs ;
  
	//Iterate thru contour list, accumulating extents for each obj to be displayed
	pos = pDoc->m_ContourList.GetHeadPosition() ;
	while ( pos ) 
	{
		pCtr = (CContour*)pDoc->m_ContourList.GetNext( pos ) ;
		ASSERT( pCtr != NULL && pCtr->m_NumVertex > 0 && pCtr->m_ppVertexArray != NULL ) ;
		numcnrs = pCtr->m_NumVertex ;

		for( int cnridx = 0; cnridx < numcnrs; cnridx++ )
		{
			GLdouble* pVertex = pCtr->m_ppVertexArray[ cnridx ] ;
			Vector3D vcnr = Vector3D( (float)pVertex[0], (float)pVertex[1], (float)pVertex[2] ) ;
			vmin.x = min( vmin.x, vcnr.x ) ;
			vmin.y = min( vmin.y, vcnr.y ) ;
			vmin.z = min( vmin.z, vcnr.z ) ;
			vmax.x = max( vmax.x, vcnr.x ) ;
			vmax.y = max( vmax.y, vcnr.y ) ;
			vmax.z = max( vmax.z, vcnr.z ) ;
		}
  } 

	//Compute radius and center of minimum size enclosing sphere
	  m_Center = ( vmin + vmax ) / 2.f ;  
	  Vector3D r = vmax - m_Center ;
	  Radius = r.x * r.x + r.y * r.y + r.z * r.z ;
	  Radius = (float)sqrt( Radius ) ;
}                        

void CMFCTessView::OnRButtonDown(UINT nFlags, CPoint point) 
{

	CMFCTessDoc* pDoc = GetDocument() ;
	if( !pDoc->m_ContourList.IsEmpty() )
	{
		// make sure window is active
		GetParentFrame()->ActivateFrame();
		CPoint ScreenPoint = point; //initialize ScreenPoint as client location

		ClientToScreen( &ScreenPoint ); //then convert to screen coords
		//ScreenToClient( &point ); //convert to client coords

		CMenu menu;
		if ( menu.LoadMenu(IDR_POPUP) )
		{
			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);

			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				ScreenPoint.x, ScreenPoint.y,
				AfxGetMainWnd()); // use main window for cmds
		}
	}
	
	CView::OnRButtonDown(nFlags, point);
}

void CMFCTessView::OnPopupWindNeg() 
{
	m_WindingRule = (int)GLU_TESS_WINDING_NEGATIVE ;
	Invalidate() ;
}

void CMFCTessView::OnUpdatePopupWindNeg(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_WindingRule == (int)GLU_TESS_WINDING_NEGATIVE ) ;	
}

void CMFCTessView::OnPopupWindNonZero() 
{
	m_WindingRule = (int)GLU_TESS_WINDING_NONZERO ;
	Invalidate() ;
}

void CMFCTessView::OnUpdatePopupWindNonZero(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_WindingRule == (int)GLU_TESS_WINDING_NONZERO ) ;	
}

void CMFCTessView::OnPopupWindOdd() 
{
	m_WindingRule = (int)GLU_TESS_WINDING_ODD ;
	Invalidate() ;
}

void CMFCTessView::OnUpdatePopupWindOdd(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_WindingRule == (int)GLU_TESS_WINDING_ODD ) ;	
}

void CMFCTessView::OnPopupWindPos() 
{
	m_WindingRule = (int)GLU_TESS_WINDING_POSITIVE ;
	Invalidate() ;
}

void CMFCTessView::OnUpdatePopupWindPos(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_WindingRule == (int)GLU_TESS_WINDING_POSITIVE ) ;	
}
