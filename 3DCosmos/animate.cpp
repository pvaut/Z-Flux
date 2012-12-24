
#include "stdafx.h"
#include "qxwin.h"
#include "qxbuttonbar.h"
#include "qxscrollbar.h"
#include "qxgrid.h"
#include "qfile.h"
#include "main.h"

#include <process.h>

#include "opengl.h"
#include "3DScene.h"

#include "objecttreeview.h"
#include "objectpropertyview.h"
#include "scriptsourceview.h"

#include "SC_functree.h"

#include "displaydevices.h"

void posterror(StrPtr err);

Tcopyarray<TDisplayWindow>* G_displaywindows();

StrPtr G_datadir();
StrPtr G_scriptsdir();
StrPtr G_texturesdir();

class Tanimatethread
{
private:
	TSC_script *script;
	HANDLE threadhandle;
	QString lasterror;
	bool isrunning;
public:
	Tanimatethread()
	{
		threadhandle=NULL;
		script=NULL;
		isrunning=false;
	}
protected:
	void run();
public:
	static Tanimatethread& Get()
	{
		static Tanimatethread sset;
		return sset;
	}
	static void runthread(void *ptr)
	{
		((Tanimatethread*)ptr)->run();
	}
	static void runscript(TSC_script *iscript)
	{
		Get().script=iscript;
		Get().threadhandle=(HANDLE)_beginthread(runthread,0,(void*)&Get());
		SetThreadPriority(Get().threadhandle,THREAD_PRIORITY_HIGHEST);
	}
	bool G_isrunning()
	{
		return isrunning;
	}
};

bool IsAnimationRunning()
{
	return Tanimatethread::Get().G_isrunning();
}

void Tanimatethread::run()
{
	QString cerror;

	isrunning=true;
	lasterror.clear();
	try{
		while ((!script->exec_isfinished())&&(!script->G_exec_isbreak())&&(!G_3DCosmos().G_requeststop()))
		{
			if (!script->exec_stepone(cerror)) throw QError(cerror);
		}
		if (G_3DCosmos().G_requeststop())
		{
			G_3DCosmos().resetallsounds();
			script->exec_stop();
		}
	}
	catch(QError err)
	{
		lasterror=err.G_content();
		posterror(lasterror);
	}
	G_3DCosmos().Reset_requeststop();
	G_3DCosmos().resetallsounds();
	G_3DCosmos().releaseallcontexts();
	isrunning=false;
}


void Tmainwin::cmd_animatescript()
{
	TSC_script *script=G_curscript();
	if (script==NULL)
	{
		reporterror(this,_text("There is no script present"));
		return;
	}
	scriptaction_pre();
	animatescript(script);
}

void Tmainwin::animatescript(TSC_script *script)
{
	addlog(TFormatString(_text("Start animation of ^1"),script->G_source().G_filename()));
	QString error;
	G_3DCosmos().Reset_requeststop();
	script->exec_clearbreak();

	if (script->exec_isstarted())
	{
		reporterror(this,_text("Script is already running"));
		return;
	}
	if (!script->exec_start(error))
	{
		reporterror(this,error);
		return;
	}

	//bring all active display windows to front
	Tcopyarray<TDisplayWindow>* dispwins=G_displaywindows();
	for (int i=0; i<dispwins->G_count(); i++)
		dispwins->get(i)->SetForegroundWindow();
	if (dispwins->G_count()>0)
	{
		if (dispwins->get(0)->renderwindow!=NULL)
			dispwins->get(0)->renderwindow->SetFocus();
	}


	G_3DCosmos().releaseallcontexts();
	scriptaction_pre();
	Tanimatethread::runscript(script);
}

TSC_script autorunscript(&GetTSCenv());

void Tmainwin::runinitscript()
{
	QString error;


	addlog(_text("Looking up _init script"));


	bool initpresent=false;
	QString initfilename=G_scriptsdir();
	initfilename+=_qstr("\\_init.sci");
	try{
		QTextfile file;
		file.openread(initfilename);
		file.close();
		initpresent=true;
	}
	catch(...) {}

	bool autorunpresent=false;
	QString autorunfilename=G_scriptsdir();
	autorunfilename+=_qstr("\\_autorun.sci");
	try{
		QTextfile file;
		file.openread(autorunfilename);
		file.close();
		autorunpresent=true;
	}
	catch(...) {}


	if (initpresent)
	{
		addlog(_text("Running _init script"),+1);
		TSC_script script(&GetTSCenv());
		script.G_source().load(initfilename);
		if (!script.exec_run(NULL,error)) reporterror(error);
		if (!autorunpresent) SetForegroundWindow();
		addlog(_text("_init script completed"),-1);
	}

	if (autorunpresent)
	{
		addlog(_text("Running _autorun script"),+1);
		autorunscript.G_source().load(autorunfilename);
		animatescript(&autorunscript);
		addlog(_text("_autorun script completed"),-1);
//		if (!autorunscript.exec_run(NULL,error)) reporterror(error);
	}
}

