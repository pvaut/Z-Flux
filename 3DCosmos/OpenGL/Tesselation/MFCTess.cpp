// MFCTess.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MFCTess.h"

#include "MainFrm.h"
#include "MFCTessDoc.h"
#include "MFCTessView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//Try at making a global SignOrDecimalPt function
BOOL GlobalSignOrDecimalPt( CString dlgtext )
{
	//Purpose: Check for leading decimal point or sign combinations
	//Provenance: Copyright 12/07/04 G. Frank Paynter
	//Inputs:
	//	dlgtext = dlg box control text
	//Ouputs:
	//	sts = TRUE for any of the common sign/dec pt combos; otherwise FALSE
	//Notes:
	//	4/3/99: avoids nuisance complaints from DDX_Text() float validation routines
	//	05/12/03 added "+.0" & "-.0" to the list of strings for which DDX validation should be skipped
	//	09/21/03 added "+0." & "-0." to the 'generalized' check
	//	09/21/03 mvd code to Necbsc.cpp as 'global' fcn
	//	09/21/03 abandoned global fcn try.  Need define a 'shim' cls btwn CDialog & XXXDlgXX clses
	//	09/02/04 rev to chk for alpha chars in remainder after truncation
	//	12/07/04 copied from EmbDlgFW::SignOrDecimalPt & adapted to consolidate all chking to global fcn 

	BOOL sts = FALSE ;

	if( dlgtext == "+" || dlgtext == "-" || dlgtext == "."
		|| dlgtext == "+0" || dlgtext == "-0" || dlgtext == "" 
		|| dlgtext == "+0." || dlgtext == "-0."
		|| dlgtext == "+." || dlgtext == "-." ) sts = TRUE ;

	//05/12/03 an attempt to be a little more general - rev 09/02/04
	else if( dlgtext.Find( "+.") == 0 || dlgtext.Find( "-." ) == 0 
		|| dlgtext.Find( "+0.") == 0 || dlgtext.Find( "-0." ) == 0 
		|| dlgtext.Find( "0.") == 0 )
	{
		int dotidx = dlgtext.Find( '.' ) ;
		dlgtext = dlgtext.Right( dlgtext.GetLength() - dotidx - 1 ) ; //remove up to & incl dec pt.
		int strlen = dlgtext.GetLength() ;
		if( atof( dlgtext ) == 0. )
		{
			//09/02/04 chk for alpha chars
			char* buf = dlgtext.GetBuffer( strlen ) ;
			for( int i = 0; i < strlen; i++ )
			{
				if( isalpha( buf[i] ) ) return FALSE ;
			}
			
			sts = TRUE ;
		}
	}

	return sts ;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CMFCTessApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CMFCTessApp

BEGIN_MESSAGE_MAP(CMFCTessApp, CWinApp)
	//{{AFX_MSG_MAP(CMFCTessApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	ON_COMMAND( ID_HELP, OnHelp ) //Required for ID_HELP processing
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMFCTessApp construction

CMFCTessApp::CMFCTessApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMFCTessApp object

CMFCTessApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMFCTessApp initialization

BOOL CMFCTessApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMFCTessDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CMFCTessView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	CAboutDlg dlg ;
	dlg.DoModal() ;

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CMFCTessApp message handlers


int CMFCTessApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWinApp::ExitInstance();
}
