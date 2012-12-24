#pragma once

#include "qstring.h"

class QBinStreamOut
{
public:
	virtual ~QBinStreamOut() {}
	virtual void write_block(const void *ptr, int len)=0;
	virtual void write_shortstring(StrPtr istr)=0;
	virtual void write_string(StrPtr istr)=0;
	virtual void write_int32(__int32 ivl)=0;
	virtual void write_double(double ivl)=0;
	virtual void write_bool(bool ivl)=0;
};


class QBinStreamIn
{
public:
	virtual ~QBinStreamIn() {}
	virtual void read_block(void *ptr, int len)=0;
	virtual void read_shortstring(QString &istr)=0;
	virtual void read_string(QString &istr)=0;
	virtual __int32 read_int32()=0;
	virtual double read_double()=0;
	virtual bool read_bool()=0;
};