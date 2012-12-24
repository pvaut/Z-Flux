#pragma once
#include <process.h>
#include "QError.h"

class Qthread
{
private:
	HANDLE threadhandle;
	HANDLE finishevent;
	static void runthread(void *obj)
	{
		((Qthread*)obj)->run();
	}
private:
	void run()
	{
		implementation();
		SetEvent(finishevent);
	}
protected:
	virtual void implementation()=0;

public:
	Qthread()
	{
		finishevent=CreateEvent(NULL,false,false,NULL);
//		if (finishevent==NULL) throw_GetLastError();
	}
	~Qthread()
	{
		CloseHandle(finishevent);
	}
	void start()
	{
		threadhandle=(HANDLE)_beginthread(runthread,0,(void*)this);
	}
	void waitforfinished()
	{
		WaitForSingleObject(finishevent,INFINITE);
	}
};
