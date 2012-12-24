#pragma once

#include "resource.h"

#include "qxwin.h"
#include "SC_script.h"


StrPtr G_softwarename();

StrPtr G_datadir();
StrPtr G_scriptsdir();
void Set_datadir(StrPtr idir);

class TQXgrid;
class TQXheadercomp_buttonbar;
class Tobjecttreeview;
class Tobjectpropertyview;
class Tscriptsourceview;
class Tscriptvarview;
class Toutputview;
class Tformattedstringview;

class Tmainwin : public TQXwin
{
private:
	Tarray<TSC_script> scripts;
public:
	int G_scriptcount() { return scripts.G_count(); }
	TSC_script* G_script(int nr) { return scripts[nr]; }
public:
	TQXgrid* grid;
	Tobjecttreeview *objtreeview;
	Tobjecttreeview *functreeview;
	Tformattedstringview *funcdescrview;
	Tobjectpropertyview *propertyview;
	Tscriptsourceview *scriptsourceview;
	Tscriptvarview *scriptvarview;
	Toutputview *outputview;
	TQXheadercomp_buttonbar *functiontreeviewbuttons,*objtreeviewbuttons,*propertyviewbuttons,*scriptsourceviewbuttons;
	QString postederror,soundcommand,soundcommanderror;
	bool soundcommandexecuted;
	DECLARE_MESSAGE_MAP();
public:
	Tmainwin();
	~Tmainwin();
	void openactivescripts();
	void buildwindef(TQXwindef &windef);
	void createviews();
	virtual void OnCommand(UINT id);
	void KeyPressed(UINT nChar, UINT nRepCnt, UINT nFlags);
	void CharPressed(UINT nChar, UINT nRepCnt, UINT nFlags );
	virtual void PostCreate();
	virtual void OnDestroy();
	virtual bool DoClose();
	long reportpostederror(UINT ui, long ln);
	long executesoundcommand(UINT ui, long ln);
public:
	TSC_script* G_curscript();
	TQXheadercomp_tab* G_sourcetabbar();
public:
	void scriptaction_pre();
	void scriptaction_post();
	void cmd_addscene();
	void cmd_addobject(int objnr);
	void cmd_refreshproperties();
	void cmd_newscript();
	void cmd_loadscript();
	void cmd_savescriptas();
	void cmd_savescript();
	void cmd_closescript();
	void cmd_undo();
	void cmd_redo();
	void cmd_script_copy();
	void cmd_script_paste();
	void cmd_script_find();
	void cmd_settings();
	void cmd_stepscript();
	void exec_pumpmessage();
	bool exec_run(QString &cerror);
	void cmd_runscript();
	void cmd_stopscript();
	void cmd_animatescript();
	void animatescript(TSC_script *script);
	void runinitscript();
};