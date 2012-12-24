#include "stdafx.h"
#include "utils.h"

void copyclipboard(StrPtr str)
{
	HGLOBAL hmem;
	StrChar *ptr;
	AfxGetMainWnd()->OpenClipboard();
	EmptyClipboard();
	hmem=GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,2*qstrlen(str)+2);
	ptr=(StrChar*)GlobalLock(hmem);
	if (ptr!=NULL)
	{
		_tcscpy(ptr,str);
		GlobalUnlock(hmem);
		SetClipboardData(CF_UNICODETEXT,hmem);
		CloseClipboard();
	}
}

bool pasteclipboard(QString &str)
{
	HGLOBAL hmem;
	StrChar *ptr;
	str.clear();
	AfxGetMainWnd()->OpenClipboard();
	hmem=GetClipboardData(CF_UNICODETEXT);
	CloseClipboard();
	if (hmem==NULL) return false;
	ptr=(StrChar*)GlobalLock(hmem);
	if (ptr==NULL) return false;
	str=ptr;
	GlobalUnlock(hmem);
	return true;
}



double random()
{
	return rand()*1.0/RAND_MAX;
}