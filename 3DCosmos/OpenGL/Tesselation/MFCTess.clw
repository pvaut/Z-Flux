; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CPolyDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "mfctess.h"
LastPage=0

ClassCount=10
Class1=CColorComboBox
Class2=CContourDlg
Class3=ESPDlgEdit
Class4=CMainFrame
Class5=CAboutDlg
Class6=CMFCTessApp
Class7=CMFCTessDoc
Class8=CMFCTessView
Class9=CPolyDlg
Class10=CVertexDlg

ResourceCount=8
Resource1=IDD_CONTOUR
Resource2=IDD_ABOUTBOX
Resource3=IDD_ADD2DCORNER
Resource4=IDR_POPUP
Resource5=IDD_POLYGON
Resource6=IDR_MAINFRAME
Resource7=IDD_ESPDLG
Resource8=IDD_OPENGLVERTEX

[CLS:CColorComboBox]
Type=0
BaseClass=CComboBox
HeaderFile=ColorComboBox.h
ImplementationFile=ColorComboBox.cpp

[CLS:CContourDlg]
Type=0
BaseClass=CDialog
HeaderFile=ContourDlg.h
ImplementationFile=ContourDlg.cpp

[CLS:ESPDlgEdit]
Type=0
BaseClass=CEdit
HeaderFile=ESPDlgEdit.h
ImplementationFile=ESPDlgEdit.cpp

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=MFCTess.cpp
ImplementationFile=MFCTess.cpp

[CLS:CMFCTessApp]
Type=0
BaseClass=CWinApp
HeaderFile=MFCTess.h
ImplementationFile=MFCTess.cpp
Filter=N
VirtualFilter=AC

[CLS:CMFCTessDoc]
Type=0
BaseClass=CDocument
HeaderFile=MFCTessDoc.h
ImplementationFile=MFCTessDoc.cpp

[CLS:CMFCTessView]
Type=0
BaseClass=CView
HeaderFile=MFCTessView.h
ImplementationFile=MFCTessView.cpp

[CLS:CPolyDlg]
Type=0
BaseClass=CDialog
HeaderFile=PolyDlg.h
ImplementationFile=PolyDlg.cpp
LastObject=IDC_COMBO_WALLPTS

[CLS:CVertexDlg]
Type=0
BaseClass=CDialog
HeaderFile=VertexDlg.h
ImplementationFile=VertexDlg.cpp

[DLG:IDD_CONTOUR]
Type=1
Class=CContourDlg
ControlCount=4
Control1=IDC_STATIC,static,1342308352
Control2=IDC_EDIT_NAME,edit,1350631552
Control3=IDOK,button,1342373889
Control4=IDCANCEL,button,1342242816

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_POLYGON]
Type=1
Class=CPolyDlg
ControlCount=19
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_LIST_FILE,listbox,1353777409
Control4=IDC_BTN_NEW,button,1342177280
Control5=IDC_BTN_REMPROF,button,1342177280
Control6=IDC_STATIC,button,1342177287
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_COMBO_WALLPTS,combobox,1344274433
Control10=IDC_BTN_ADD,button,1342242816
Control11=IDC_BTN_INSERT,button,1342242816
Control12=IDC_BTN_DELETE,button,1342242816
Control13=IDC_BTN_MODIFY,button,1342242816
Control14=IDC_STATIC,button,1342177287
Control15=IDC_BUTTON1,button,1342177291
Control16=ID_HELP,button,1342242816
Control17=IDC_STATIC,static,1342308352
Control18=IDC_STATIC,static,1342308352
Control19=IDC_STATIC,static,1342308352

[DLG:IDD_OPENGLVERTEX]
Type=1
Class=CVertexDlg
ControlCount=17
Control1=IDC_EDIT_X,edit,1350631552
Control2=IDC_STATIC,static,1342308864
Control3=IDC_EDIT_Y,edit,1350631552
Control4=IDC_STATIC,static,1342308864
Control5=IDC_EDIT_Z,edit,1350633600
Control6=IDC_STATIC,static,1342308864
Control7=IDC_STATIC,button,1342177287
Control8=IDC_EDIT_RED,edit,1350631552
Control9=IDC_STATIC,static,1342308864
Control10=IDC_EDIT_BLUE,edit,1350631552
Control11=IDC_STATIC,static,1342308864
Control12=IDC_EDIT_GREEN,edit,1350631552
Control13=IDC_STATIC,static,1342308864
Control14=IDC_BTN_COLOR,button,1342242816
Control15=IDC_STATIC,button,1342177287
Control16=IDOK,button,1342242817
Control17=IDCANCEL,button,1342242816

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
CommandCount=8

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_PRINT
Command6=ID_FILE_PRINT_PREVIEW
Command7=ID_FILE_PRINT_SETUP
Command8=ID_FILE_MRU_FILE1
Command9=ID_APP_EXIT
Command10=ID_EDIT_UNDO
Command11=ID_EDIT_CUT
Command12=ID_EDIT_COPY
Command13=ID_EDIT_PASTE
Command14=ID_VIEW_TOOLBAR
Command15=ID_VIEW_STATUS_BAR
Command16=ID_MENU_DEFPOLY
Command17=ID_APP_ABOUT
Command18=ID_HELP_FINDER
CommandCount=18

[MNU:IDR_POPUP]
Type=1
Class=?
Command1=IDM_POPUP_WINDPOS
Command2=IDM_POPUP_WINDNEG
Command3=IDM_POPUP_WINDODD
Command4=IDM_POPUP_WINDNONZERO
CommandCount=4

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_EDIT_COPY
Command2=ID_FILE_NEW
Command3=ID_FILE_OPEN
Command4=ID_FILE_PRINT
Command5=ID_FILE_SAVE
Command6=ID_EDIT_PASTE
Command7=ID_EDIT_UNDO
Command8=ID_EDIT_CUT
Command9=ID_HELP
Command10=ID_CONTEXT_HELP
Command11=ID_NEXT_PANE
Command12=ID_PREV_PANE
Command13=ID_EDIT_COPY
Command14=ID_EDIT_PASTE
Command15=ID_EDIT_CUT
Command16=ID_EDIT_UNDO
CommandCount=16

[DLG:IDD_ESPDLG]
Type=1
Class=?
ControlCount=2
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816

[DLG:IDD_ADD2DCORNER]
Type=1
Class=?
ControlCount=7
Control1=IDC_STTXT_XLABEL,static,1342308864
Control2=IDC_REF_EDITX,edit,1350631552
Control3=IDC_STTXT_YLABEL,static,1342308864
Control4=IDC_REF_EDITY,edit,1350631552
Control5=IDC_REF_COORDS,button,1342242823
Control6=IDOK,button,1342242817
Control7=IDCANCEL,button,1342242816

