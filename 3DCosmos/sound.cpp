#include "stdafx.h"
#include "3DScene.h"
#include "displaydevices.h"

#include "sound.h"


//*********************************************************************
// TSound
//*********************************************************************

TSound::TSound()
{
	opened=false;
	currentvolume=500;
	fader_active=false;
//	playing=addparam(_qstr("Playing"),SC_valname_boolean)->content.G_content_boolean();playing->copyfrom(false);
//	framereduction=addparam(_qstr("FrameReduction"),SC_valname_scalar)->content.G_content_scalar();framereduction->copyfrom(1);
}

void TSound::SetID(StrPtr iID)
{
	ID=iID;
}


TSound::~TSound()
{
	close();
}

void TSound::setfile(StrPtr ifilename)
{
	filename=ifilename;
	open();
}

void ExecSoundCommand(StrPtr cmd);


void TSound::exec(StrPtr cmd)
{
	ExecSoundCommand(cmd);
}


void TSound::open()
{
	if (opened) throw QError(_text("Sound is already opened"));
	exec(TFormatString(_qstr("open ^1 alias ^2 wait"),filename,ID));
	opened=true;
	fader_active=false;
//	changevolume(500);
}

void TSound::start()
{
	if (!opened) throw QError(_text("Sound is not opened"));
	exec(TFormatString(_qstr("play ^1 from 0"),ID));
}


void TSound::stop()
{
	if (opened)
		exec(TFormatString(_qstr("stop ^1"),ID));
	opened=false;
}

void TSound::close()
{
	if (!opened) return;
	exec(TFormatString(_qstr("close ^1"),ID));
	opened=false;
}

void TSound::pauze()
{
	if (!opened) throw QError(_text("Sound is not opened"));
	exec(TFormatString(_qstr("pause ^1"),ID));
}

void TSound::resume()
{
	if (!opened) throw QError(_text("Sound is not opened"));
	exec(TFormatString(_qstr("resume ^1"),ID));
}

void TSound::setvol(int ivol)
{
	if (!opened) throw QError(_text("Sound is not opened"));
	exec(TFormatString(_qstr("setaudio ^1 volume to ^2"),ID,ivol));
}

void TSound::changevolume(int ivol)
{
	currentvolume=ivol;
	setvol(ivol);
}

void TSound::fadevolume(int inewvol, double idurat)
{
	if (idurat<=0) return;
	fader_active=true;
	fader_startvol=currentvolume;fader_endvol=inewvol;
	fader_starttime=G_3DCosmos().G_elapsedrendertime();
	fader_durat=idurat;
}


void TSound::updaterender()
{
	if (fader_active)
	{
		double fr=(G_3DCosmos().G_elapsedrendertime()-fader_starttime)/fader_durat;
		if (fr>1)
		{
			fr=1;
			fader_active=false;
		}
		changevolume(fader_startvol+fr*(fader_endvol-fader_startvol));
	}
}



bool TSound::G_param_optionlist(StrPtr paramname, Tarray<QString> &list)
{
	return false;
}

void TSound::paramchanged(StrPtr iname)
{
}

TObjectTreeItem* TSound::G_parent()
{
	return T3DCosmos::Get().G_soundlist();
}


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_sound_create,addsound)
{
	setclasspath_fromtype(SC_valname_sound);
	setreturntype(SC_valname_sound);
	addvar(_qstr("FileName"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSound *snd=G_3DCosmos().G_soundlist()->addsound();
	snd->setfile(arglist->get(0)->G_content_string()->G_string());
	retval->encapsulate(snd);
}
ENDFUNCTION(func_sound_create)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_sound_start,Start)
{
	setmemberfunction(SC_valname_sound);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSound *snd=G_valuecontent<TSound>(owner);
	snd->start();
}
ENDFUNCTION(func_sound_start)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_sound_stop,Stop)
{
	setmemberfunction(SC_valname_sound);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSound *snd=G_valuecontent<TSound>(owner);
	snd->stop();
}
ENDFUNCTION(func_sound_stop)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_sound_pauze,Pauze)
{
	setmemberfunction(SC_valname_sound);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSound *snd=G_valuecontent<TSound>(owner);
	snd->pauze();
}
ENDFUNCTION(func_sound_pauze)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_sound_resume,Resume)
{
	setmemberfunction(SC_valname_sound);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSound *snd=G_valuecontent<TSound>(owner);
	snd->resume();
}
ENDFUNCTION(func_sound_resume)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_sound_setvol,SetVolume)
{
	setmemberfunction(SC_valname_sound);
	addvar(_qstr("Volume"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSound *snd=G_valuecontent<TSound>(owner);
	int vl=arglist->get(0)->G_content_scalar()->G_intval();
	snd->changevolume(vl);
}
ENDFUNCTION(func_sound_setvol)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_sound_fadevolume,FadeVolume)
{
	setmemberfunction(SC_valname_sound);
	addvar(_qstr("Volume"),SC_valname_scalar);
	addvar(_qstr("Duration"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSound *snd=G_valuecontent<TSound>(owner);
	int newvol=arglist->get(0)->G_content_scalar()->G_intval();
	double durat=arglist->get(1)->G_content_scalar()->G_val();
	snd->fadevolume(newvol,durat);
}
ENDFUNCTION(func_sound_fadevolume)
