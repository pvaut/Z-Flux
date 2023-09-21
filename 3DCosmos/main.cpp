#include "stdafx.h"

#include "afxdisp.h"
#include "dbghelp.h"
#include "mmsystem.h"

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

#include "video.h"


#include <gdiplus.h>
//using namespace Gdiplus;

StrPtr G_softwarename()
{
	return _text("Z-Flux");
}

void GetMACaddress(QString &addressname);


void loadstockcyclorbits();


Tmainwin *mainwin=NULL;

QString startupdir,datadir,scriptsdir,texturesdir,logfilename,MACaddr;
bool logfilestarted=false;
int logindent=0;

StrPtr G_startupdir() { return startupdir; }
StrPtr G_datadir() { return datadir; }
StrPtr G_scriptsdir() { return scriptsdir; }
StrPtr G_texturesdir() { return texturesdir; }
StrPtr G_MACaddr() { return MACaddr; }

void Set_datadir(StrPtr idir)
{
	datadir=idir;
	scriptsdir=datadir;scriptsdir+=_qstr("\\Scripts");
	texturesdir=datadir;texturesdir+=_qstr("\\textures");
}

void addlog(StrPtr content, int incrpos)
{
	if (incrpos<0) logindent+=incrpos;
	if (logindent<0) logindent=0;
	if ((logfilestarted)&&(qstrlen(logfilename)>0))
	{
		FILE *fp=_wfopen(logfilename,_qstr("a"));
		if (fp!=NULL)
		{
			QString tmstr;
			TSC_time tm;tm.fillwithnowUT();
			tm.tostring(tmstr,0);
			fwprintf(fp,tmstr);
			for (int ii=0; ii<=logindent; ii++) fwprintf(fp,_qstr(" | "));
			fwprintf(fp,content);
			fwprintf(fp,_qstr("\n"));
			fclose(fp);
		}
	}
	if (incrpos>0) logindent+=incrpos;
}

void initlog()
{
	logfilename=datadir+_qstr("\\log.txt");
	FILE *fp=_wfopen(logfilename,_qstr("w"));
	if (fp==NULL)
	{
		reporterror(TFormatString(_text("Unable to create log file ^1"),logfilename));
		logfilename="";
	}
	fclose(fp);
	addlog(_text("================================= START LOG FILE ========================================"));
	logfilestarted=true;
}



void message(StrPtr content)
{
	::MessageBox(NULL,content,_text("MSG"),MB_OK);
}

void debugmessage(const char* content)
{
	CString st;st=content;
	message(st);
}

DWORD lasttick=0;

void reporttimer(StrPtr content)
{
	DWORD newtick=GetTickCount();
	if (newtick<lasttick+60*1000)
	{
		message(TFormatString(_text("^1 ^2s"),content,(newtick-lasttick)/1000.0));
	}
	lasttick=GetTickCount();
}



void posterror(StrPtr err)
{
	if (mainwin==NULL)
	{
		ASSERT(false);
		return;
	}
	mainwin->postederror=err;
	mainwin->PostMessage(WM_USER+1,0,0);
}

void ExecSoundCommand(StrPtr cmd)
{
	if (mainwin==NULL)
	{
		ASSERT(false);
		return;
	}

	mainwin->soundcommand=cmd;
	mainwin->soundcommandexecuted=false;

	mainwin->executesoundcommand(0,0);

/*	mainwin->PostMessage(WM_USER+2,0,0);
	int ctr=0;
	while ((!mainwin->soundcommandexecuted)&&(ctr<50))
	{
		Sleep(10);
		ctr++;
	}*/

	if (qstrlen(mainwin->soundcommanderror)>0) throw QError(mainwin->soundcommanderror);
}




//////////////////////////////////////////////////////////////////////
// Tfinddialog
//////////////////////////////////////////////////////////////////////

CString lastfindstring;

class Tfinddialog : public CDialog
{
private:
	Tmainwin *parent;
public:
	Tfinddialog(Tmainwin *iparent);
	~Tfinddialog();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
public:
//	DECLARE_MESSAGE_MAP();
};

/*
BEGIN_MESSAGE_MAP(Tfinddialog,CDialog)
  ON_WM_DRAWITEM()
  ON_WM_MEASUREITEM()
  ON_BN_CLICKED(201,renamefield)
  ON_BN_CLICKED(202,renameexper)
  ON_BN_CLICKED(203,details)
  ON_LBN_SELCHANGE(102,update)
END_MESSAGE_MAP()
*/


Tfinddialog::Tfinddialog(Tmainwin *iparent) 
: CDialog(IDD_SEARCH,iparent)
{
	parent=iparent;
}

Tfinddialog::~Tfinddialog()
{
}


BOOL Tfinddialog::OnInitDialog()
{
	GetDlgItem(101)->SetWindowText(lastfindstring);
	CDialog::OnInitDialog();
	return TRUE;
}


void Tfinddialog::OnOK()
{
	GetDlgItem(101)->GetWindowText(lastfindstring);
	CString st;
	GetDlgItem(101)->GetWindowText(st);
	mainwin->scriptsourceview->cmd_find(st);
}

void Tfinddialog::OnCancel()
{
	GetDlgItem(101)->GetWindowText(lastfindstring);
	CDialog::OnCancel();
}



class Tedittextdialog : public CDialog
{
private:
	QString &inpstr;
	QString str;
public:
	Tedittextdialog(QString &iinpstr) : inpstr(iinpstr), CDialog(IDD_EDITTEXT,AfxGetMainWnd())
	{
		str=inpstr;
		str.replace(_qstr("\n"),_qstr("\r\n"));
	}
	~Tedittextdialog()
	{
	}
	virtual BOOL OnInitDialog()
	{
		GetDlgItem(IDC_EDIT1)->SetWindowText(str);
		return TRUE;
	}
	virtual void OnOK()
	{
		CString cstr;
		GetDlgItem(IDC_EDIT1)->GetWindowText(cstr);
		str=(StrPtr)cstr;
		str.replace(_qstr("\r\n"),_qstr("\n"));
		inpstr=str;
		CDialog::OnOK();
	}
	virtual void OnCancel()
	{
		CDialog::OnCancel();
	}
public:
};

bool EditText(QString &str)
{
	Tedittextdialog dlg(str);
	if (dlg.DoModal()==IDOK) return true;
	else return false;
}



typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
									CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
									CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
									CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
									);

long _stdcall exceptfilter_silent(_EXCEPTION_POINTERS *info)
{
	return EXCEPTION_EXECUTE_HANDLER;
}


long _stdcall exceptfilter(_EXCEPTION_POINTERS *info)
{
	LONG retval=EXCEPTION_CONTINUE_SEARCH;

	//load dbghelp module
	HMODULE hdll=NULL;
	StrChar debughelppath[1999];
	if (GetModuleFileName(NULL,debughelppath,999))
	{
		StrChar *pslash=_tcsrchr(debughelppath,'\\');
		if (pslash)
		{
			_tcscpy_s(pslash+1,999,_qstr("DBGHELP.DLL"));
			hdll=::LoadLibrary(debughelppath);
		}
	}
	if (hdll==NULL) hdll=::LoadLibrary(_qstr("DBGHELP.DLL"));

	QString dumpcomment,dumppath;

	if (hdll==NULL) dumpcomment=_text("Could not write dump: unable to find DBGHELP.DLL");
	else
	{
		MINIDUMPWRITEDUMP pdump = (MINIDUMPWRITEDUMP)::GetProcAddress( hdll, "MiniDumpWriteDump" );
		if (pdump==NULL) strcpy(dumpcomment,"Could not write dump: too old version of DBGHELP.DLL");
		else
		{
			dumppath=_qstr("except.dmp");
			HANDLE hfile=::CreateFile(dumppath,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
			if (hfile==NULL) FormatString(dumpcomment,_text("Could not write dump: unable to save file '^1'"),dumppath);
			else
			{
				_MINIDUMP_EXCEPTION_INFORMATION ExInfo;
				ExInfo.ThreadId=::GetCurrentThreadId();
				ExInfo.ExceptionPointers=info;
				ExInfo.ClientPointers=NULL;
				BOOL ok=pdump(GetCurrentProcess(),GetCurrentProcessId(),hfile,MiniDumpNormal,&ExInfo,NULL,NULL);
				if (!ok) FormatString(dumpcomment,_text("Failed to save dump file to '^1'"),dumppath);
				else
				{
					FormatString(dumpcomment,_text("Saved dump file to '^1'"),dumppath);
					retval=EXCEPTION_EXECUTE_HANDLER;
				}
				::CloseHandle(hfile);
			}
		}
	}

	QString dumpinfo;
	dumpinfo=_text("An unrecoverable error occurred.\n");dumpinfo+=dumpcomment;
	::MessageBox(0,dumpinfo,_text("Error"),MB_TASKMODAL|MB_ICONSTOP|MB_OK);

	return retval;
}



//////////////////////////////////////////////////////////////////////
// Tmainwin
//////////////////////////////////////////////////////////////////////


BEGIN_MESSAGE_MAP(Tmainwin,TQXwin)
    ON_MESSAGE(WM_USER+1,reportpostederror)
    ON_MESSAGE(WM_USER+2,executesoundcommand)
END_MESSAGE_MAP()


Tmainwin::Tmainwin()
{
	TDisplayAdapterlist::Get().load();
}

Tmainwin::~Tmainwin()
{
	addlog(_text("Destroying all existing display windows"),+1);
	Tcopyarray<TDisplayWindow> *wins=G_displaywindows();
	for (int i=0; i<wins->G_count(); i++)
		wins->get(i)->DestroyWindow();
	addlog(_text("All existing display windows are destroyed"),-1);


	addlog(_text("Saving open scripts list"),+1);
	QTextfile fl;
	QString filename,st;
	FormatString(filename,_qstr("^1\\openscripts.txt"),G_datadir());
	fl.openwrite(filename);
	FormatString(st,_qstr("^1"),G_sourcetabbar()->G_curtab());
	fl.write(st);fl.writeline();
	for (int i=0; i<scripts.G_count(); i++)
	{
		fl.write(scripts[i]->G_source().G_filename());
		fl.writeline();
	}
	fl.close();

	addlog(_text("Open scripts list saved"),-1);

	addlog(_text("Closing log file"));
	logfilestarted=false;
}

long Tmainwin::reportpostederror(UINT ui, long ln)
{
	MessageBox(postederror,_text("Error"),MB_ICONEXCLAMATION|MB_OK);
	return 0;
}


long Tmainwin::executesoundcommand(UINT ui, long ln)
{
	StrChar rt[2000],errstr[2000];
	QString error;
	MCIERROR err;
	soundcommanderror=_qstr("");
	err=mciSendString(soundcommand,rt,999,NULL);
	if (err!=0)
	{
		mciGetErrorString(err,errstr,1999);
		error=errstr;
		error+=_qstr(" (");
		error+=soundcommand;
		error+=_qstr(")");
		soundcommanderror=error;
	}
	soundcommandexecuted=true;
	return 0;
}


void Tmainwin::createviews()
{
	CString str;

	addlog(_text("Creating views"),+1);

	//create object tree view & associated headers
	addlog(_text("Creating object tree view"));
	objtreeview=new Tobjecttreeview(_qstr("Object tree"),this,&T3DCosmos::Get());
	addview(objtreeview);
	objtreeviewbuttons=new TQXheadercomp_buttonbar(_qstr("Object tree tools"),this);
	addheadercomponent(objtreeviewbuttons);
	TQXheadercomp_button *addbutton=objtreeviewbuttons->addbutton(-1,IDB_ADD,_text("Add new objects..."));
	addbutton->addsubitem(99,0,_text("Create new scene"));
	addbutton->addsubitem(0,0,_qstr("-"));
	for (int i=0; i<T3DCosmos::Get().G_objectcatalog_count(); i++)
		addbutton->addsubitem(100+i,0,T3DCosmos::Get().G_objectcatalog_object(i)->G_classname());

	//scroll bars for object tree view
	{
		TQXheadercomp_scrollbar *sc1=new TQXheadercomp_scrollbar(QString("ObjTreeScrollV"));
		addheadercomponent(sc1);
		TQXheadercomp_scrollbar *sc2=new TQXheadercomp_scrollbar(QString("ObjTreeScrollH"));
		addheadercomponent(sc2);
		sc1->addview(objtreeview,DIR_VERT);
		sc2->addview(objtreeview,DIR_HOR);
		objtreeview->updatescrollbars();
	}
	propertyviewbuttons=new TQXheadercomp_buttonbar(_qstr("Object property tools"),this);
	addheadercomponent(propertyviewbuttons);
	propertyviewbuttons->addbutton(201,IDB_PROCEED,_text("Refresh"));




	//create object property view
	addlog(_text("Creating object properties view"));
	propertyview=new Tobjectpropertyview(QString("Object properties"),this);
	addview(propertyview);
	{
		TQXheadercomp_scrollbar *sc1=new TQXheadercomp_scrollbar(QString("ObjPropScrollV"));
		addheadercomponent(sc1);
		TQXheadercomp_scrollbar *sc2=new TQXheadercomp_scrollbar(QString("ObjPropScrollH"));
		addheadercomponent(sc2);
		sc1->addview(propertyview,DIR_VERT);
		sc2->addview(propertyview,DIR_HOR);
	}


	//create script source view
	addlog(_text("Creating script source view"));
	scriptsourceview=new Tscriptsourceview(QString("Source"),this);
	addview(scriptsourceview);
	{
		TQXheadercomp_scrollbar *sc1=new TQXheadercomp_scrollbar(QString("ScriptSourceScrollV"));
		addheadercomponent(sc1);
		TQXheadercomp_scrollbar *sc2=new TQXheadercomp_scrollbar(QString("ScriptSourceScrollH"));
		addheadercomponent(sc2);
		sc1->addview(scriptsourceview,DIR_VERT);
		sc2->addview(scriptsourceview,DIR_HOR);
	}
	scriptsourceviewbuttons=new TQXheadercomp_buttonbar(_qstr("Script tools"),this);
	addheadercomponent(scriptsourceviewbuttons);
	scriptsourceviewbuttons->addbutton(351,IDB_NEW,_text("New"));
	scriptsourceviewbuttons->addbutton(352,IDB_LOAD,_text("Open"));
	scriptsourceviewbuttons->addbutton(353,IDB_SAVE,_text("Save (Ctrl+S)"));
	scriptsourceviewbuttons->addbutton(354,IDB_CLOSEFILE,_text("Close"));
	scriptsourceviewbuttons->addbutton(IDB_UNDO,IDB_UNDO,_text("Undo (Ctrl+Z)"));
	scriptsourceviewbuttons->addbutton(IDB_REDO,IDB_REDO,_text("Redo (Ctrl+Y)"));
	scriptsourceviewbuttons->addbutton(IDB_CLIPBOARD_COPY,IDB_CLIPBOARD_COPY,_text("Copy to clipboard (Ctrl+C)"));
	scriptsourceviewbuttons->addbutton(IDB_CLIPBOARD_PASTE,IDB_CLIPBOARD_PASTE,_text("Paste from clipboard (Ctrl+V)"));
	scriptsourceviewbuttons->addbutton(IDB_SEARCH,IDB_SEARCH,_text("Find (Ctrl+F)"));
	scriptsourceviewbuttons->addbutton(301,IDB_PROCEED,_text("Step (Shift+F5)"));
	scriptsourceviewbuttons->addbutton(303,IDB_PROCEEDALL,_text("Run (Ctrl+F5)"));
	scriptsourceviewbuttons->addbutton(302,IDB_STOP,_text("Stop"));
	scriptsourceviewbuttons->addbutton(304,IDB_ANIMATE,_text("Animate (F5)"));

	scriptsourceviewbuttons->addbutton(305,IDB_SETTINGS,_text("Settings"));

	//create script var view
	addlog(_text("Creating variables view"));
	scriptvarview=new Tscriptvarview(QString("Variables"),scriptsourceview);
	addview(scriptvarview);
	scriptsourceview->Set_varview(scriptvarview);
	{
		TQXheadercomp_scrollbar *sc1=new TQXheadercomp_scrollbar(QString("ScriptVarScrollV"));
		addheadercomponent(sc1);
		TQXheadercomp_scrollbar *sc2=new TQXheadercomp_scrollbar(QString("ScriptVarScrollH"));
		addheadercomponent(sc2);
		sc1->addview(scriptvarview,DIR_VERT);
		sc2->addview(scriptvarview,DIR_HOR);
	}

	//create output view
	addlog(_text("Creating output view"));
	outputview=new Toutputview(QString("Output"),scriptsourceview);
	addview(outputview);
	{
		TQXheadercomp_scrollbar *sc1=new TQXheadercomp_scrollbar(QString("OutputScrollV"));
		addheadercomponent(sc1);
		TQXheadercomp_scrollbar *sc2=new TQXheadercomp_scrollbar(QString("OutputScrollH"));
		addheadercomponent(sc2);
		sc1->addview(outputview,DIR_VERT);
		sc2->addview(outputview,DIR_HOR);
	}

	//create function view
	addlog(_text("Creating function tree tools"));
	functiontreeviewbuttons=new TQXheadercomp_buttonbar(_qstr("Function tree tools"),this);
	addheadercomponent(functiontreeviewbuttons);
	functiontreeviewbuttons->addbutton(IDB_BACK,IDB_BACK,_text("Back"));
	functiontreeviewbuttons->addbutton(IDB_FORWARD,IDB_FORWARD,_text("Forward"));


	GetTSCenv().buildfunctiontree();
	addlog(_text("Creating function tree view"));
	functreeview=new Tobjecttreeview(_qstr("Function tree"),this,GetTSCenv().G_functreeroot());
	addview(functreeview);
	{
		TQXheadercomp_scrollbar *sc1=new TQXheadercomp_scrollbar(QString("FuncTreeScrollV"));
		addheadercomponent(sc1);
		TQXheadercomp_scrollbar *sc2=new TQXheadercomp_scrollbar(QString("FuncTreeScrollH"));
		addheadercomponent(sc2);
		sc1->addview(functreeview,DIR_VERT);
		sc2->addview(functreeview,DIR_HOR);
		functreeview->updatescrollbars();
	}

	//create function description view
	addlog(_text("Creating function description view"));
	funcdescrview=new Tformattedstringview(QString("Function description"),this);
	addview(funcdescrview);
	{
		TQXheadercomp_scrollbar *sc1=new TQXheadercomp_scrollbar(QString("FuncDescrScrollV"));
		addheadercomponent(sc1);
		sc1->addview(funcdescrview,DIR_VERT);
	}



	addlog(_text("Views created"),-1);

}



void Tmainwin::buildwindef(TQXwindef &windef)
{
	addlog(_text("Building window definition"),+1);
	{
		TQXwinpaneldef *panel0=windef.rootpanel.addpanel(QString("Root"),0.3,TQXwinpaneldef::SUB_TAB);


		{
			TQXwinpaneldef *panel=panel0->addpanel(QString("Functions"),0.35,TQXwinpaneldef::SUB_VERT);
			{
				TQXwinpaneldef *fpanel=panel->addpanel(QString("Function tree"),0.65,TQXwinpaneldef::SUB_VERT);
				fpanel->addheadercomponent(functiontreeviewbuttons->G_name(),SIDE_TOP);
				fpanel->addheadercomponent(QString("FuncTreeScrollV"),SIDE_RIGHT);
				fpanel->addheadercomponent(QString("FuncTreeScrollH"),SIDE_BOTTOM);
			}
			{
				TQXwinpaneldef *spanel=panel->addpanel(QString("Function description"),0.35);
				spanel->addheadercomponent(QString("FuncDescrScrollV"),SIDE_RIGHT);
			}
		}

		{
			TQXwinpaneldef *panel=panel0->addpanel(QString("Objects"),0.3,TQXwinpaneldef::SUB_VERT);
//			panel->hastitlebar=true;
			{
				TQXwinpaneldef *spanel=panel->addpanel(QString("Object tree"),0.4);
//				spanel->hastitlebar=true;
				spanel->addheadercomponent(objtreeviewbuttons->G_name(),SIDE_TOP);
				spanel->addheadercomponent(QString("ObjTreeScrollV"),SIDE_RIGHT);
				spanel->addheadercomponent(QString("ObjTreeScrollH"),SIDE_BOTTOM);
			}
			{
				TQXwinpaneldef *spanel=panel->addpanel(QString("Object properties"),0.4);
				spanel->hastitlebar=true;
				spanel->addheadercomponent(propertyviewbuttons->G_name(),SIDE_TOP);
				spanel->addheadercomponent(QString("ObjPropScrollV"),SIDE_RIGHT);
				spanel->addheadercomponent(QString("ObjPropScrollH"),SIDE_BOTTOM);
			}
		}



	}


	{
		TQXwinpaneldef *panel=windef.rootpanel.addpanel(QString("Scripts"),0.65,TQXwinpaneldef::SUB_VERT);
		{
			TQXwinpaneldef *spanel=panel;
			//TQXwinpaneldef *spanel=panel->addpanel(QString("Scripts"),0.6,TQXwinpaneldef::SUB_VERT);
			{
				spanel->addheadercomponent(scriptsourceviewbuttons->G_name(),SIDE_TOP);
				{
					TQXwinpaneldef *scriptsourcepanel=spanel->addpanel(QString("Source"),0.6);
					scriptsourcepanel->hastitlebar=false;
					scriptsourcepanel->hascustomtabs=true;
					scriptsourcepanel->addheadercomponent(QString("ScriptSourceScrollV"),SIDE_RIGHT);
					scriptsourcepanel->addheadercomponent(QString("ScriptSourceScrollH"),SIDE_BOTTOM);
				}

				{
					TQXwinpaneldef *panel2=spanel->addpanel(QString("aa"),0.4,TQXwinpaneldef::SUB_TAB);

					{
						TQXwinpaneldef *scriptvarpanel=panel2->addpanel(QString("Variables"),0.4);
//						scriptvarpanel->hastitlebar=true;
						scriptvarpanel->addheadercomponent(QString("ScriptVarScrollV"),SIDE_RIGHT);
						scriptvarpanel->addheadercomponent(QString("ScriptVarScrollH"),SIDE_BOTTOM);
					}
					{
						TQXwinpaneldef *outputpanel=panel2->addpanel(QString("Output"),0.4);
//						outputpanel->hastitlebar=true;
						outputpanel->addheadercomponent(QString("OutputScrollV"),SIDE_RIGHT);
						outputpanel->addheadercomponent(QString("OutputScrollH"),SIDE_BOTTOM);
					}
				}
			}
		}
/*		{
			TQXwinpaneldef *spanel=panel->addpanel(QString("Panel 2"),0.6);
		}
		{
			TQXwinpaneldef *spanel=panel->addpanel(QString("P 3-2"),0.6);
		}*/
	}
	addlog(_text("Window definition built"),-1);
}


void Tmainwin::openactivescripts()
{
	addlog(_text("Opening active scripts"),+1);
	TQXpanel *sourcepanel=G_panel_byname(_qstr("Source"));
	if (sourcepanel==NULL)
	{
		ASSERT(false);
		return;
	}
	sourcepanel->customtabs_init(scriptsourceview);

	try{
		QTextfile fl;
		QString filename,line;
		FormatString(filename,_qstr("^1\\openscripts.txt"),G_datadir());
		fl.openread(filename);
		fl.readline(line);
		int activescriptnr=qstr2int(line);
		while(!fl.isend())
		{
			fl.readline(line);
			if (line.G_length()>0)
			{
				try{
					TSC_script *script1=new TSC_script(&GetTSCenv());
					script1->G_source().load(line);
					scripts.add(script1);
					sourcepanel->customtabs_add(script1->G_source().G_filename());
				} catch(QError err)
				{
					QString errstr;
					FormatString(errstr,_text("Unable to open script file ^1: ^2"),line,err.G_content());
					reporterror(errstr);
				}
			}
		}
		fl.close();
		if ((activescriptnr>=0)&&(activescriptnr<scripts.G_count()))
			G_sourcetabbar()->change_tab(activescriptnr);
		else
			scriptsourceview->Set_script(scripts[0]);
		addlog(_text("Active scripts opened"),-1);
	} catch(QError &err)
	{
		reporterror(err.G_content());
	}
}


void Tmainwin::PostCreate()
{
	openactivescripts();

	runinitscript();
}

void Tmainwin::OnDestroy()
{
	addlog(_text("Destroying source code window"),+1);
	T3DCosmos::Get().cleanup();
	addlog(_text("Source code window destroyed"),-1);
}

bool Tmainwin::DoClose()
{
	for (int i=0; i<scripts.G_count(); i++)
	{
		if (scripts[i]->G_source().G_ismodified())
		{
			G_sourcetabbar()->change_tab(i);
			QString st;
			FormatString(st,_text("Script '^1' has been modified. Do you want to save it now?"),scripts[i]->G_source().G_filename());
			int rs=MessageBox(st,_text("Warning"),MB_ICONQUESTION|MB_YESNOCANCEL);
			if (rs==IDCANCEL) return false;
			if (rs==IDYES)
			{
				cmd_savescript();
				if (scripts[i]->G_source().G_ismodified()) return false;
			}
		}
	}
	return true;
}



TSC_script* Tmainwin::G_curscript()
{
	return scriptsourceview->G_script();
}

TQXheadercomp_tab* Tmainwin::G_sourcetabbar()
{
	TQXpanel *sourcepanel=G_panel_byname(_qstr("Source"));
	if (sourcepanel==NULL)
	{
		ASSERT(false);
		return NULL;
	}
//	sourcepanel->customtabs_add(scripts[scripts.G_count()-1]->G_source().G_filename());
	TQXheadercomp_tab *tabbar=sourcepanel->G_tabbar();
	ASSERT(tabbar!=NULL);
	return tabbar;
}


void Tmainwin::cmd_addscene()
{
	objtreeview->Set_cursor(T3DCosmos::Get().addscene());
	objtreeview->redraw();
	objtreeview->updatescrollbars();
}


void Tmainwin::cmd_addobject(int objnr)
{
	ASSERT((objnr>=0)&&(objnr<T3DCosmos::Get().G_objectcatalog_count()));
	if (objtreeview->G_cursor()==NULL)
	{
		MessageBox(_text("No object is selected"),_text("Error"),MB_ICONEXCLAMATION|MB_OK);
		return;
	}
	T3DScene *selscene=T3DCosmos::Get().findscene(objtreeview->G_cursor());
	T3DObject *selobj=T3DCosmos::Get().findobject(objtreeview->G_cursor());
	if ((selobj==NULL)&&(selscene==NULL))
	{
		MessageBox(_text("The current selection is not an object or a scene"),_text("Error"),MB_ICONEXCLAMATION|MB_OK);
		return;
	}
	if ((selobj!=NULL)&&(!selobj->G_acceptsubobjects()))
	{
		MessageBox(_text("The selected object does not allow subobjects"),_text("Error"),MB_ICONEXCLAMATION|MB_OK);
		return;
	}
	T3DObject *newobj=T3DCosmos::Get().G_objectcatalog_object(objnr)->MakeInstance();
	if (selobj!=NULL) selobj->addsubobject(newobj);
	if (selscene!=NULL) selscene->addobject(newobj);
	objtreeview->Set_cursor(newobj);
	objtreeview->redraw();
	objtreeview->updatescrollbars();
	return;
}

void Tmainwin::cmd_refreshproperties()
{
	try{
//		propertyview->tranfercontent();
		objtreeview->Set_cursor(NULL);
	}
	catch (QError &err)
	{
		reporterror(this,err);
	}
}


void Tmainwin::cmd_newscript()
{
	scripts.add(new TSC_script(&GetTSCenv()));
	TQXpanel *sourcepanel=G_panel_byname(_qstr("Source"));
	G_sourcetabbar()->addtab(scripts[scripts.G_count()-1]->G_source().G_filename());
	G_sourcetabbar()->change_tab(scripts.G_count()-1);
}


void Tmainwin::cmd_loadscript()
{
	OPENFILENAME openf;
	StrChar filters[10000],filename[30000],filetitle[30000];
	int i,sz;

	_tcscpy_s(filters,9999,_qstr("Script files (*.SCI)|*.SCI||"));
	sz=qstrlen(filters);
	for (i=0; i<=sz; i++) if (filters[i]=='|') filters[i]=0;
	for (i=0; i<=15999; i++) filename[i]=0;
	for (i=0; i<=15999; i++) filename[i]=0;

	openf.lStructSize=sizeof(openf);
	openf.hwndOwner=m_hWnd;
	openf.lpstrFilter=filters;
	openf.lpstrCustomFilter=NULL;
	openf.nMaxCustFilter=0;
	openf.nFilterIndex=1;
	openf.lpstrFile=filename;
	openf.nMaxFile=29999;
	openf.lpstrFileTitle=filetitle;
	openf.nMaxFileTitle=29999;
	openf.lpstrInitialDir=_qstr("");
	openf.lpstrTitle=_text("Load script file");
	openf.Flags=OFN_LONGNAMES|OFN_LONGNAMES|OFN_EXPLORER;
	openf.lpstrDefExt=_qstr("SCI");
	openf.lpfnHook=NULL;
	openf.lpTemplateName=0;

	if (GetOpenFileName(&openf)==TRUE)
	{
		TSC_script *script=new TSC_script(&GetTSCenv());
		script->G_source().load(filename);
		scripts.add(script);
		G_sourcetabbar()->addtab(scripts[scripts.G_count()-1]->G_source().G_filename());
		G_sourcetabbar()->change_tab(scripts.G_count()-1);
	}
}


void Tmainwin::cmd_savescriptas()
{
	OPENFILENAME openf;
	StrChar filters[600],filename[30000],filetitle[30000];
	char css[1000];
	int i,sz,ok;

	TSC_script *script=scriptsourceview->G_script();
	if (script==NULL) return;

	_tcscpy_s(filters,599,_qstr("Script files (*.SCI)|*.SCI||"));
	sz=qstrlen(filters);
	for (i=0; i<=sz; i++) if (filters[i]=='|') filters[i]=0;
	for (i=0; i<=15999; i++) filename[i]=0;
	_tcscpy_s(filename,29999,_qstr(""));

	openf.lStructSize=sizeof(openf);
	openf.hwndOwner=m_hWnd;
	openf.lpstrFilter=filters;
	openf.lpstrCustomFilter=NULL;
	openf.nMaxCustFilter=0;
	openf.nFilterIndex=1;
	openf.lpstrFile=filename;
	openf.nMaxFile=29999;
	openf.lpstrFileTitle=filetitle;
	openf.nMaxFileTitle=29999;
	openf.lpstrInitialDir=_qstr("");
	openf.lpstrTitle=_qstr("Save script file as");
	openf.Flags=OFN_LONGNAMES|OFN_LONGNAMES|OFN_EXPLORER;
	openf.lpstrDefExt=_qstr("SCI");
	openf.lpfnHook=NULL;
	openf.lpTemplateName=0;

	if (GetSaveFileName(&openf)==TRUE)
	{
		script->G_source().saveas(filename);
		TQXheadercomp_tab *tabbar=G_sourcetabbar();
		int scriptnr=tabbar->G_curtab();
		tabbar->Set_tabname(scriptnr,filename);
	}
}

void Tmainwin::cmd_savescript()
{
	TSC_script *script=scriptsourceview->G_script();
	if (script==NULL) return;
	if (qstrlen(script->G_source().G_filename())==0) cmd_savescriptas();
	else
	{
		script->G_source().save();
	}
}

void Tmainwin::cmd_closescript()
{
	TQXheadercomp_tab *tabbar=G_sourcetabbar();
	int scriptnr=tabbar->G_curtab();
	if ((scriptnr<0)||(scriptnr>=scripts.G_count())) return;
	scriptsourceview->Set_script(NULL);
	scripts.del(scriptnr);
	tabbar->deltab(scriptnr);
}

void Tmainwin::cmd_undo()
{
	TSC_script *script=scriptsourceview->G_script();
	if (script==NULL) return;
	script->G_source().cmd_undo();
	scriptsourceview->update_postchange();
}

void Tmainwin::cmd_redo()
{
	TSC_script *script=scriptsourceview->G_script();
	if (script==NULL) return;
	script->G_source().cmd_redo();
	scriptsourceview->update_postchange();
}

void Tmainwin::cmd_script_copy()
{
	TSC_script *script=scriptsourceview->G_script();
	if (script==NULL) return;
	script->G_source().cmd_copy();
}

void Tmainwin::cmd_script_paste()
{
	TSC_script *script=scriptsourceview->G_script();
	if (script==NULL) return;
	script->G_source().cmd_paste();
	scriptsourceview->update_postchange();
}

void Tmainwin::cmd_script_find()
{
	Tfinddialog *dlg = new Tfinddialog(this);
	dlg->Create(IDD_SEARCH,this);
	dlg->ShowWindow(SW_SHOW);
}

void Tmainwin::cmd_settings()
{
	Tsettingsdialog dlg(this);
	dlg.DoModal();
}


void Tmainwin::scriptaction_pre()
{
	objtreeview->G_root()->expanded=false;
	cmd_refreshproperties();
}

void Tmainwin::scriptaction_post()
{
	scriptsourceview->update_postchange();
	scriptsourceview->scrollinside_debugline();
	scriptvarview->updatescrollbars();
	scriptvarview->redraw();
	outputview->updatescrollbars();
	outputview->redraw();
	objtreeview->redraw();
}


void Tmainwin::cmd_stepscript()
{
	if (G_curscript()==NULL)
	{
		reporterror(this,_text("There is no script present"));
		return;
	}
	scriptaction_pre();
	QString error;
	if (!G_curscript()->exec_isstarted())
	{
		if (!G_curscript()->exec_start(error))
		{
			scriptaction_post();
			scriptsourceview->scrollinside_sel();
			reporterror(this,error);
		}
		scriptaction_post();
	}
	else
	{
		if (!G_curscript()->exec_stepone(error)) reporterror(this,error);
		scriptaction_post();
	}
}



Tcopyarray<TDisplayWindow>* G_displaywindows();

void Tmainwin::exec_pumpmessage()
{
    MSG msg;

	while (PeekMessage((LPMSG) &msg,m_hWnd, 
			0, 0, PM_REMOVE)) { 
		{ 
			TranslateMessage((LPMSG) &msg); 
			DispatchMessage((LPMSG) &msg); 
		} 
	}

	Tcopyarray<TDisplayWindow> *winlist=G_displaywindows();
	for (int wnr=0; wnr<winlist->G_count(); wnr++)
	{
		TDisplayWindow *win=winlist->get(wnr);
		while (PeekMessage((LPMSG) &msg,win->m_hWnd, 
				0, 0, PM_REMOVE)) { 
			{ 
				TranslateMessage((LPMSG) &msg); 
				DispatchMessage((LPMSG) &msg); 
			} 
		}
	}

}

bool Tmainwin::exec_run(QString &cerror)
{
	TSC_script *curscript=G_curscript();
	if (curscript==NULL)
	{
		cerror=_text("There is no script present");
		return false;
	}

	if (!curscript->exec_isstarted())
	{
		if (!curscript->exec_start(cerror)) return false;
	}
	curscript->exec_clearbreak();
	while ((!curscript->exec_isfinished())&&(!curscript->G_exec_isbreak())&&(!G_3DCosmos().G_requeststop()))
	{
		exec_pumpmessage();
		if (!curscript->exec_stepone(cerror)) return false;
	}
	return true;
}


void Tmainwin::cmd_runscript()
{
	if (G_curscript()==NULL)
	{
		reporterror(this,_text("There is no script present"));
		return;
	}
	G_3DCosmos().Reset_requeststop();
	QString error;
	scriptaction_pre();
	if (!exec_run(error))
//	if (!G_curscript()->exec_run(error))
	{
		scriptaction_post();
		scriptsourceview->scrollinside_sel();
		reporterror(this,error);
	}
	else scriptaction_post();
}


void Tmainwin::cmd_stopscript()
{
	if (G_curscript()==NULL)
	{
		reporterror(this,_text("There is no script present"));
		return;
	}
	G_curscript()->exec_stop();
	scriptsourceview->redraw();
	scriptvarview->redraw();
}


void Tmainwin::OnCommand(UINT id)
{

	if (id==99) cmd_addscene();
	if ((id>=100)&&((int)id<100+T3DCosmos::Get().G_objectcatalog_count())) cmd_addobject(id-100);

	if (id==201) cmd_refreshproperties();

	if (id==351) cmd_newscript();
	if (id==352) cmd_loadscript();
	if (id==353) cmd_savescript();
	if (id==354) cmd_closescript();

	if (id==IDB_UNDO) cmd_undo();
	if (id==IDB_REDO) cmd_redo();

	if (id==IDB_CLIPBOARD_COPY) cmd_script_copy();
	if (id==IDB_CLIPBOARD_PASTE) cmd_script_paste();
	if (id==IDB_SEARCH) cmd_script_find();

	if (id==IDB_BACK) functreeview->cmd_cursorback();
	if (id==IDB_FORWARD) functreeview->cmd_cursorforward();

	if (id==301) cmd_stepscript();
	if (id==302) cmd_stopscript();
	if (id==303) cmd_runscript();
	if (id==304) cmd_animatescript();

	if (id==305) cmd_settings();
}

void Tmainwin::KeyPressed(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool shiftpressed=((GetKeyState(VK_SHIFT)>>1)!=0);
	bool controlpressed=((GetKeyState(VK_CONTROL)>>1)!=0);

	if (nChar==VK_F5)
	{
		if ((!controlpressed)&&(!shiftpressed)) cmd_animatescript();
		if ((!controlpressed)&&(shiftpressed)) cmd_stepscript();
		if ((controlpressed)&&(!shiftpressed)) cmd_runscript();
	}

}


void Tmainwin::CharPressed(UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if (nChar=='F'-'A'+1) { cmd_script_find(); }
}


class myapp : public CWinApp
{
	ULONG_PTR gditoken;
public:
	myapp() : gditoken(NULL)
	{

	}
	~myapp()
	{
		Gdiplus::GdiplusShutdown(gditoken);
	}
	virtual BOOL InitInstance()	{
		Gdiplus::GdiplusStartupInput input;
		Gdiplus::GdiplusStartup(&gditoken, &input, NULL);
		SetUnhandledExceptionFilter(exceptfilter);

		try{
			SetRegistryKey(G_softwarename());

			//boost process priority
			SetPriorityClass(GetCurrentProcess(),HIGH_PRIORITY_CLASS);

			AfxInitRichEdit();
	//		AfxEnableControlContainer();
	//		AfxOleInit();

			{//create documents folder
				QString dir;
				GetDocumentsDir(dir);dir+=_qstr("\\");dir+=G_softwarename();
				CreateDirectory(dir,NULL);
			}


			CWinApp::InitInstance();

			//for 3D Connexion
			if (!AfxOleInit())
			{
				AfxMessageBox(L"OLE Init failed");
				return FALSE;
			}
			AfxEnableControlContainer();


			StrChar locstartupdir[600];
			GetCurrentDirectory(599,locstartupdir);
			startupdir=locstartupdir;
			datadir=startupdir+_qstr("\\Data");
			{
				QString storeddatadir;
				QParamRead(PARAMLOCATION_REGISTRY,_qstr("DataDirectory"),storeddatadir);
				if (qstrlen(storeddatadir)>0)
					datadir=storeddatadir;
			}

			initlog();
			addlog(TFormatString(_text("Data directory: ^1"),datadir));

			scriptsdir=datadir;scriptsdir+=_qstr("\\Scripts");
			texturesdir=datadir;texturesdir+=_qstr("\\textures");

			addlog(_text("Fetching MAC address"));
			GetMACaddress(MACaddr);
			addlog(MACaddr);

			//T3DCosmos::Get().LoadSettings();

			G_3DCosmos().LoadSettings();
			G_3DCosmos().init();


			loadstockcyclorbits();

			G_QXSys().startup();

			addlog(_text("Loading languages"),+1);
			QTranslate::Get().loadlanguages();
			addlog(_text("Loaded languages"),-1);

			addlog(_text("Creating source code window"),+1);
			mainwin= new Tmainwin();
			mainwin->createwindow(NULL,QString("Source code window"));
			addlog(_text("Source code window created"),-1);
			addlog(_text(""));
			addlog(_text(""));
			mainwin->ShowWindow(m_nCmdShow);
			m_pMainWnd=mainwin;

		}
		catch(QError &err)
		{
			reporterror(err.G_content());
		}

		return TRUE;
	}
	virtual int ExitInstance()
	{
		T3DCosmos::Get().cleanup();
		return 0;
	}

};


myapp app;



void G_highlightfuncid(QString &ID)
{
	ID.clear();
	if (mainwin->functreeview->G_cursor()==NULL) return;
	mainwin->functreeview->G_cursor()->G_fullID(ID);
}


void ExitProgram()
{
/*	if (IsAnimationRunning())
	{
		G_3DCosmos().Set_requeststop();
		int ctr=0;
		for (bool stopped=false; (!stopped)&&(ctr<50);)
		{
			Sleep(100);ctr++;
			if (!IsAnimationRunning()) stopped=true;
		}
	}*/
	_exit(-1);
//	mainwin->DestroyWindow();
}


void ShutDownWindows()
{
//	adderror("Starting system restart");


	HANDLE hToken; // handle to process token 
	TOKEN_PRIVILEGES tkp; // pointer to token structure 
	OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken); 
	// Get the LUID for shutdown privilege. 
	LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid); 
	tkp.PrivilegeCount = 1; // one privilege to set 
	// Get shutdown privilege for this process. 
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken,FALSE, &tkp,0,(PTOKEN_PRIVILEGES) NULL, 0); 
	// Cannot test the return value of AdjustTokenPrivileges.
	if(GetLastError()!= ERROR_SUCCESS) 
	{
//		adderror("AdjustTokenPrivileges enable failed.");
		return;
	}

	if (!InitiateSystemShutdown(NULL,_text("Shutdown by software"),0,true,false))
	{
/*		unsigned long lasterror=GetLastError();
		char css[1000];
		sprintf(css,"System shutdown failed: code %d",lasterror);
		adderror(css);
		char *mess=NULL;
		int rs;
		rs=FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ARGUMENT_ARRAY,
						   NULL,lasterror,LANG_NEUTRAL,(LPTSTR)&mess,0,NULL );
		if (mess!=NULL)
		{
			adderror(mess);
			LocalFree(mess);
		}*/
		return;
	}
//	adderror("System shutdown executed");
	_exit(-1);
}


void SetMonitorStatus(bool istatus)
{
	if (istatus==false)
	{//monitor off
		Sleep(500); // Eliminate user's interaction for 500 ms
		mainwin->SendMessage(WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM) 2);
	}
	else
	{//monitor on
		mainwin->SendMessage(WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM) -1);
	}
}


void ExecuteApplication(StrPtr commandline, StrPtr startupdir, bool waitforcompletion)
{

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );
	// Start the child process.
	if( !CreateProcess( NULL, // No module name (use command line).
		(StrChar*)commandline, // Command line.
		NULL,             // Process handle not inheritable.
		NULL,             // Thread handle not inheritable.
		FALSE,            // Set handle inheritance to FALSE.
		0,                // No creation flags.
		NULL,             // Use parent's environment block.
		startupdir,       // Startup directory
		&si,              // Pointer to STARTUPINFO structure.
		&pi )             // Pointer to PROCESS_INFORMATION structure.
	) throw QError(TFormatString(_text("Unable to start application '^1'"),commandline));

	if (waitforcompletion)
	{
		// Wait until child process exits.
		WaitForSingleObjectEx(pi.hThread,INFINITE,TRUE);
	}

	// Close process and thread handles.
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
}
