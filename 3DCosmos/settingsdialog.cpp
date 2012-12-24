#include "stdafx.h"

#include "afxdisp.h"

#include "qxwin.h"
#include "qxbuttonbar.h"
#include "qxscrollbar.h"
#include "qxgrid.h"
#include "qfile.h"
#include "qtranslate.h"
#include "main.h"

#include "3DScene.h"

#include "objecttreeview.h"
#include "objectpropertyview.h"
#include "formattedstringview.h"
#include "scriptsourceview.h"

#include "SC_functree.h"

#include "displaydevices.h"

#include "settingsdialog.h"


Tsettingsdialog::Tsettingsdialog(Tmainwin *iparent) 
: CDialog(IDD_SETTINGS,iparent)
{
	parent=iparent;
}

Tsettingsdialog::~Tsettingsdialog()
{
}


BOOL Tsettingsdialog::OnInitDialog()
{
	GetDlgItem(IDC_SCRIPTSDIR)->SetWindowText(G_datadir());

	((CButton*)GetDlgItem(IDC_COLORSCHEME))->SetCheck(G_QXSys().colscheme_dark);

	QFileFind fnd;
	fnd.start(TFormatString(_qstr("^1\\languages\\*.*"),G_datadir()));
	QString dirname;
	bool isdir,ishidden;
	CComboBox *lbox=(CComboBox*)GetDlgItem(IDC_LANGUAGE);
	lbox->AddString(_qstr(""));
	while (fnd.getnext(dirname,isdir,ishidden))
	{
		if (isdir&&(!issame(dirname,_qstr(".")))&&(!issame(dirname,_qstr(".."))))
		{
			lbox->AddString(dirname);
			if (issame(dirname,QTranslate::Get().GetLanguage()))
				lbox->SetCurSel(lbox->GetCount()-1);
		}
	}

	CDialog::OnInitDialog();
	return TRUE;
}


void Tsettingsdialog::OnOK()
{
	CString cst;
	GetDlgItem(IDC_SCRIPTSDIR)->GetWindowText(cst);
	Set_datadir(cst);
	QParamStore(PARAMLOCATION_REGISTRY,_qstr("DataDirectory"),G_datadir());

	G_QXSys().colscheme_dark=((CButton*)GetDlgItem(IDC_COLORSCHEME))->GetCheck();

	CComboBox *lbox=(CComboBox*)GetDlgItem(IDC_LANGUAGE);
	CString lstr;
	lbox->GetWindowText(lstr);
	QTranslate::Get().SetLanguage(lstr);

	G_QXSys().writesettings();


	parent->invalidateall();
	CDialog::OnOK();
}
