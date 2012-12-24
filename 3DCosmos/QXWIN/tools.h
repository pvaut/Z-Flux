#pragma once

#include<map>
#include "binstream.h"

#include "qstring.h"
//#include "qerror.h"

#define Pi 3.141592653589

class Tcpl_internal_index;




//array that holds pointers to objects, and deletes them afterwards
template <class A>
class Tarray
{
private:
	A **obj;
	int count;
	int buflen;
public:
	Tarray()
	{
		count=0;
		buflen=100;
		obj=(A**)malloc(sizeof(A*)*buflen);
	};
	~Tarray()
	{
		for (int i=0; i<count; i++) 
			if (obj[i]!=NULL) delete obj[i];
		free(obj);
	};
	void reset()
	{
		for (int i=0; i<count; i++)
			if (obj[i]!=NULL) delete obj[i];
		count=0;
	};
	void makebuffer(int buffersize)
	{
		if (buffersize<=buflen) return;
		buflen=buffersize;
		obj=(A**)realloc(obj,sizeof(A*)*buflen);
	}
	int add(A *iobj)
	{
		if (count>=buflen)
		{
			buflen+=buflen/10+10;
			obj=(A**)realloc(obj,sizeof(A*)*buflen);
		}
		count++;
		obj[count-1]=iobj;
		return count-1;
	}
	int addexact(A *iobj)
	{
		if (count>=buflen)
		{
			buflen++;
			obj=(A**)realloc(obj,sizeof(A*)*buflen);
		}
		count++;
		obj[count-1]=iobj;
		return count-1;
	}
	void del(int nr)
	{
		int i;
		if ((nr<0)|(nr>=count)) return;
		if (obj[nr]!=NULL) delete obj[nr];
		count--;
		for (i=nr; i<count; i++) obj[i]=obj[i+1];
	}
	A* get(int nr)
	{
		if ((nr<0)|(nr>=count)) return NULL;
		return obj[nr];
	}
	void set(int nr, A *iobj)
	{
		if ((nr<0)|(nr>=count)) return;
		obj[nr]=iobj;
	}
	void insertbefore(int nr, A *iobj)
	{
		int i;
		add(NULL);
		for (i=G_count()-1; i>nr; i--) set(i,get(i-1));
		set(nr,iobj);
	}
	A* operator[](int nr) const
	{
		if ((nr<0)|(nr>=count)) return NULL;
		return obj[nr];
	}
	int G_count() const { return count; };
};


//array that holds objects
template <class A>
class Titemarray
{
private:
	A *obj;
	int count;
	int buflen;
public:
	Titemarray()
	{
		count=0;
		buflen=100;
		obj=(A*)malloc(sizeof(A)*buflen);
	};
	~Titemarray()
	{
		free(obj);
	};
	void reset()
	{
		count=0;
	};
	void resize(int newsize)
	{
		count=newsize;buflen=newsize;
		obj=(A*)realloc(obj,sizeof(A)*buflen);
	}
	int add(const A &iobj)
	{
		if (count>buflen-10)
		{
			buflen+=buflen/10+10;
			obj=(A*)realloc(obj,sizeof(A)*buflen);
		}
		count++;
		obj[count-1]=iobj;
		return count-1;
	}
	void insertbefore(int nr, A &iobj)
	{
		int i;
		add(get(count-1));
		for (i=G_count()-1; i>nr; i--) set(i,get(i-1));
		set(nr,iobj);
	}
	void del(int nr)
	{
		int i;
		if ((nr<0)|(nr>=count)) return;
		count--;
		for (i=nr; i<count; i++) obj[i]=obj[i+1];
	}
	A& get(int nr)
	{
		if ((nr<0)|(nr>=count)) return obj[0];
		return obj[nr];
	}
	void set(int nr, const A &iobj)
	{
		if ((nr<0)|(nr>=count)) return;
		obj[nr]=iobj;
	}
	int G_count() const { return count; };
	A* G_ptr() { return obj; };
	A& operator[](int nr) const
	{
		if ((nr<0)|(nr>=count)) return obj[0];
		return obj[nr];
	}

	void streamout(QBinStreamOut &writer)
	{
		writer.write_int32(G_count());
		writer.write_block(G_ptr(),sizeof(A)*G_count());
	}
	void streamin(QBinStreamIn &reader)
	{
		reset();
		int len=reader.read_int32();
		resize(len);
		reader.read_block(G_ptr(),sizeof(A)*G_count());
	}
};



//array that holds objects and uses minimal buffer space
template <class A>
class Tcheapitemarray
{
private:
	A *obj;
	int count;
	int buflen;
public:
	Tcheapitemarray()
	{
		count=0;
		buflen=1;
		obj=(A*)malloc(sizeof(A)*buflen);
	};
	~Tcheapitemarray()
	{
		free(obj);
	};
	void reset()
	{
		count=0;
	};
	void dim(int icount)
	{
		count=icount;
		if (count>=buflen)
		{
			buflen=count+1;
			obj=(A*)realloc(obj,sizeof(A)*buflen);
		}
	}
	int add(A &iobj)
	{
		if (count>=buflen)
		{
			buflen+=buflen/10+1;
			obj=(A*)realloc(obj,sizeof(A)*buflen);
		}
		count++;
		obj[count-1]=iobj;
		return count-1;
	}
	void del(int nr)
	{
		int i;
		if ((nr<0)|(nr>=count)) return;
		count--;
		for (i=nr; i<count; i++) obj[i]=obj[i+1];
	}
	A& get(int nr)
	{
		if ((nr<0)|(nr>=count)) return obj[0];
		return obj[nr];
	}
	void set(int nr, A &iobj)
	{
		if ((nr<0)|(nr>=count)) return;
		obj[nr]=iobj;
	}
	int G_count() const { return count; };
	A* G_ptr() { return obj; };
	A& operator[](int nr) const
	{
		if ((nr<0)|(nr>=count)) return obj[0];
		return obj[nr];
	}
	void streamout(QBinStreamOut &writer)
	{
		writer.write_int32(G_count());
		writer.write_block(G_ptr(),sizeof(A)*G_count());
	}
	void streamin(QBinStreamIn &reader)
	{
		reset();
		int len=reader.read_int32();
		dim(len);
		reader.read_block(G_ptr(),sizeof(A)*G_count());
	}
};



//same as Tarray, without deleting members
template <class A>
class Tcopyarray
{
private:
	A **obj;
	int count;
	int buflen;
public:
	Tcopyarray()
	{
		count=0;
		buflen=100;
		obj=(A**)malloc(sizeof(A*)*buflen);
	};
	~Tcopyarray()
	{
		if (obj!=NULL) free(obj);
	};
	void reset()
	{
		count=0;
	};
	void freeall()
	{
		free(obj);obj=NULL;
	}
	void redim(int newcount)
	{
		if (buflen<newcount+10)
		{
			buflen=newcount+newcount/10+10;
			obj=(A**)realloc(obj,sizeof(A*)*buflen);
		}
		for (int i=count; i<newcount; i++) obj[i]=NULL;
		count=newcount;
	};
	int add(A *iobj)
	{
		if (count>buflen-10)
		{
			buflen+=buflen/10+10;
			obj=(A**)realloc(obj,sizeof(A*)*buflen);
		}
		count++;
		obj[count-1]=iobj;
		return count-1;
	}
	void insertbefore(int nr, A *iobj)
	{
		if ((nr<0)||(nr>count)) return;
		int i;
		add(iobj);
		for (i=G_count()-1; i>nr; i--) obj[i]=obj[i-1];
		obj[nr]=iobj;
	}
	void del(int nr)
	{
		int i;
		if ((nr<0)|(nr>=count)) return;
		count--;
		for (i=nr; i<count; i++) obj[i]=obj[i+1];
	}
	A* get(int nr)
	{
		if ((nr<0)|(nr>=count)) return NULL;
		return obj[nr];
	}
	void set(int nr, A *iobj)
	{
		if ((nr<0)|(nr>=count)) return;
		obj[nr]=iobj;
	}
	int G_count() const { return count; };
	A** G_ptr() { return obj; };
	A* operator[](int nr) const
	{
		if ((nr<0)|(nr>=count)) return NULL;
		return obj[nr];
	}
};



//same as Tarray, without deleting members (cheap version)
template <class A>
class Tcheapcopyarray
{
private:
	A **obj;
	int count;
	int buflen;
public:
	Tcheapcopyarray()
	{
		count=0;
		buflen=1;
		obj=(A**)malloc(sizeof(A*)*buflen);
	};
	~Tcheapcopyarray()
	{
		free(obj);
	};
	void reset()
	{
		count=0;
	};
	void redim(int newcount)
	{
		if (buflen<newcount+10)
		{
			buflen=newcount+newcount/10+5;
			obj=(A**)realloc(obj,sizeof(A*)*buflen);
		}
		for (int i=count; i<newcount; i++) obj[i]=NULL;
		count=newcount;
	};
	int add(A *iobj)
	{
		if (count>buflen-10)
		{
			buflen+=buflen/10+10;
			obj=(A**)realloc(obj,sizeof(A*)*buflen);
		}
		count++;
		obj[count-1]=iobj;
		return count-1;
	}
	void del(int nr)
	{
		int i;
		if ((nr<0)|(nr>=count)) return;
		count--;
		for (i=nr; i<count; i++) obj[i]=obj[i+1];
	}
	A* get(int nr)
	{
		if ((nr<0)|(nr>=count)) return NULL;
		return obj[nr];
	}
	void set(int nr, A *iobj)
	{
		if ((nr<0)|(nr>=count)) return;
		obj[nr]=iobj;
	}
	int G_count() const { return count; };
	A** G_ptr() { return obj; };
	A* operator[](int nr) const
	{
		if ((nr<0)|(nr>=count)) return NULL;
		return obj[nr];
	}
};



class Tintarray
{
private:
	__int32 *obj;
	__int32 dummy;
	int count;
	int buflen;
public:
	Tintarray()
	{
		dummy=0;
		count=0;
		buflen=100;
		obj=(__int32*)malloc(sizeof(__int32)*buflen);
	};
	~Tintarray()
	{
		free(obj);
	};
	void reset()
	{
		count=0;
	};
	void redim(int newcount)
	{
		if (buflen<newcount+10)
		{
			buflen=newcount+newcount/2+100;
			obj=(__int32*)realloc(obj,sizeof(__int32)*buflen);
		}
		for (int i=count; i<newcount; i++) obj[i]=0;
		count=newcount;
	};
	int add(__int32 vl)
	{
		if (count>buflen-10)
		{
			buflen+=buflen/10+10;
			obj=(__int32*)realloc(obj,sizeof(__int32)*buflen);
		}
		count++;
		obj[count-1]=vl;
		return count-1;
	}
	void del(int nr)
	{
		int i;
		if ((nr<0)|(nr>=count)) return;
		count--;
		for (i=nr; i<count; i++) obj[i]=obj[i+1];
	}
	__int32 get(int nr)
	{
		if ((nr<0)|(nr>=count)) return -1;
		return obj[nr];
	}
	void set(int nr, __int32 vl)
	{
		if ((nr<0)|(nr>=count)) return;
		obj[nr]=vl;
	}
	int G_count() const { return count; };
	__int32* G_ptr() { return obj; };
	__int32& operator[](int nr)
	{
		if ((nr<0)|(nr>=count)) return dummy;
		return obj[nr];
	}
	__int32 operator[](int nr) const
	{
		if ((nr<0)|(nr>=count)) return dummy;
		return obj[nr];
	}

	void streamout(QBinStreamOut &writer)
	{
		writer.write_int32(G_count());
		writer.write_block(G_ptr(),sizeof(__int32)*G_count());
	}

	void streamin(QBinStreamIn &reader)
	{
		reset();
		int len=reader.read_int32();
		redim(len);
		reader.read_block(G_ptr(),sizeof(__int32)*G_count());
	}

};




class Tchararray
{
private:
	char *obj;
	char dummy;
	int count;
	int buflen;
public:
	Tchararray()
	{
		dummy=0;
		count=0;
		buflen=100;
		obj=(char*)malloc(sizeof(char)*buflen);
	};
	~Tchararray()
	{
		free(obj);
	};
	void reset()
	{
		count=0;
	};
	int add(char vl)
	{
		if (count>buflen-10)
		{
			buflen+=buflen/3+100;
			obj=(char*)realloc(obj,sizeof(char)*buflen);
		}
		count++;
		obj[count-1]=vl;
		return count-1;
	}
	void redim(int newcount)
	{
		if (buflen<newcount+10)
		{
			buflen=newcount+newcount/2+100;
			obj=(char*)realloc(obj,sizeof(char)*buflen);
		}
		for (int i=count; i<newcount; i++) obj[i]=0;
		count=newcount;
	};
	void del(int nr)
	{
		int i;
		if ((nr<0)|(nr>=count)) return;
		count--;
		for (i=nr; i<count; i++) obj[i]=obj[i+1];
	}
	char get(int nr)
	{
		if ((nr<0)|(nr>=count)) return -1;
		return obj[nr];
	}
	void set(int nr, char vl)
	{
		if ((nr<0)|(nr>=count)) return;
		obj[nr]=vl;
	}
	int G_count() { return count; };
	char* G_ptr() { return obj; };
	char& operator[](int nr)
	{
		if ((nr<0)|(nr>=count)) return dummy;
		return obj[nr];
	}
};



class Tdoublearray
{
private:
	double dummy;
	double *obj;
	int count;
	int buflen;
public:
	Tdoublearray()
	{
		count=0;
		buflen=100;
		obj=(double*)malloc(sizeof(double)*buflen);
		dummy=0;
	};
	~Tdoublearray()
	{
		free(obj);
	};
	void reset()
	{
		count=0;
	};
	void redim(int newcount)
	{
		if (buflen<newcount+10)
		{
			buflen=newcount+newcount/2+100;
			obj=(double*)realloc(obj,sizeof(double)*buflen);
		}
		for (int i=count; i<newcount; i++) obj[i]=0;
		count=newcount;
	};
	int add(double vl)
	{
		if (count>buflen-10)
		{
			buflen+=buflen/3+100;
			obj=(double*)realloc(obj,sizeof(double)*buflen);
		}
		count++;
		obj[count-1]=vl;
		return count-1;
	}
	void del(int nr)
	{
		int i;
		if ((nr<0)|(nr>=count)) return;
		count--;
		for (i=nr; i<count; i++) obj[i]=obj[i+1];
	}
	double get(int nr)
	{
		if ((nr<0)|(nr>=count)) return -1;
		return obj[nr];
	}
	double get2(int nr)
	{
		if ((nr<0)|(nr>=count)) return 0;
		return obj[nr];
	}
	void set(int nr, double vl)
	{
		if ((nr<0)|(nr>=count)) return;
		obj[nr]=vl;
	}
	int G_count() const { return count; };
	double* G_ptr() { return obj; };
	double& operator[](int nr)
	{
		if ((nr<0)|(nr>=count)) return dummy;
		return obj[nr];
	}
	double operator[](int nr) const
	{
		if ((nr<0)|(nr>=count)) return dummy;
		return obj[nr];
	}

	void streamout(QBinStreamOut &writer)
	{
		writer.write_int32(G_count());
		writer.write_block(G_ptr(),sizeof(double)*G_count());
	}

	void streamin(QBinStreamIn &reader)
	{
		reset();
		int len=reader.read_int32();
		redim(len);
		reader.read_block(G_ptr(),sizeof(double)*G_count());
	}

};




template <class A>
class Tsortedarray
{
private:
	A *data;
	int count,buflen;
	void bracket(A val, int &ps1, int &ps2)
	{
		int ps=0;
		if (count<=0) { ps1=-1; ps2=0; return; }
		if (val<=data[0]) { ps1=-1; ps2=0; return; }
		if (val>data[count-1]) { ps1=count-1; ps2=count; return; }

		ps1=0;ps2=count-1;
		while(ps2-ps1>1)
		{
			ps=(ps1+ps2)/2;
			if (data[ps]>=val) ps2=ps;
			else ps1=ps;
		}
	};
public:
	Tsortedarray()
	{
		count=0;
		buflen=100;
		data=(A*)malloc(sizeof(A)*buflen);
	};
	~Tsortedarray()
	{
		free(data);
	};
	int findfirst(A val)
	{
		int p1,p2;

		bracket(val,p1,p2);
		if ((p2<count)&&(data[p2]==val)) return p2; else return -1;
	};
	void forget()
	{
		free(data);
		count=0;
		buflen=10;
		data=(A*)malloc(sizeof(A)*buflen);
	};
	void redim(int newcount)
	{
		if (buflen<newcount+10)
		{
			buflen=newcount+newcount/2+100;
			data=(A*)realloc(data,sizeof(A)*buflen);
		}
		count=newcount;
	};
	void add(A val)
	{
		int p1,p2;
		if (count>buflen-10)
		{
			buflen+=buflen/3+100;
			data=(A*)realloc(data,sizeof(A)*buflen);
		}
		bracket(val,p1,p2);
		count++;
		if (count>1)
		{
			if (p2<count)
				memmove(&data[p2+1],&data[p2],sizeof(A)*(count-p2));
			data[p2]=val;
		}
		else data[0]=val;
	};
	int G_count() { return count; };
	A* G_ptr() { return data; };
};

void intarray_sort(Tintarray &ar_vals, Tintarray &ar_idx);
void doublearray_sort(Tdoublearray &ar_vals, Tintarray &ar_idx);
void stringarray_sort(Titemarray<char*> &ar_vals, Tintarray &ar_idx);
void stringintarray_sort(Titemarray<char*> &ar_vals1, Tintarray &ar_vals2, Tintarray &ar_idx);
int findstring(const char *str, const char *findstr, int casesens);
double select_kth_smallest_double(int k, int n, double *arr);//zero start index
double calcmedian_double(int n, double *arr);//zero based index
int select_kth_smallest_int(int k, int n, int *arr);//zero start index
int calcmedian_int(int n, int *arr);//zero based index


class Tbaltree
{
	friend class Tbaltree_item;
private:
	Tbaltree_item *root;
	void balanceleft(Tbaltree_item*& p, short& h);
	void balanceright(Tbaltree_item*& p, short& h);
	void searchsub(Tbaltree_item*& ppnode, StrPtr skey, short& h,
		Tbaltree_item*& solnode, int autoadd);
	Tbaltree_item* searchadd(StrPtr skey);
	Tbaltree_item* search(StrPtr skey);
public:
	Tbaltree();
	~Tbaltree();
public:
	void reset();
	void add(StrPtr str, int idx);
	int get(StrPtr str);
};




template <class A>
class TBtree_item : public CObject
{
public:
	TBtree_item *l,*r;
	short bal;
	A key;
	int index;
public:
	TBtree_item(A ikey)
	{
		key=ikey;
		index=0;
		l=NULL;
		r=NULL;
		bal=0;
	};
	~TBtree_item()
	{
		if (l!=NULL) delete l;
		if (r!=NULL) delete r;
	};
};


template <class A>
class TBtree : public CObject
{
private:
	TBtree_item<A> *root;

private:
	void balanceleft(TBtree_item<A>*& p, short& h)
	{
		TBtree_item<A> *p1,*p2;

		p1=p->l;
		if (p1->bal==-1)
		{
			p->l=p1->r;
			p1->r=p;
			p->bal=0;
			p=p1;
		}
		else
		{
			p2=p1->r;
			p1->r=p2->l;
			p2->l=p1;
			p->l=p2->r;
			p2->r=p;
			if (p2->bal==-1) p->bal=1; else p->bal=0;
			if (p2->bal==+1) p1->bal=-1; else p1->bal=0;
			p=p2;
		}
		p->bal=0;
		h=0;
	};

	void balanceright(TBtree_item<A>*& p, short& h)
	{
		TBtree_item<A> *p1,*p2;

		p1=p->r;
		if (p1->bal==1)
		{
			p->r=p1->l;
			p1->l=p;
			p->bal=0;
			p=p1;
		}
		else
		{
			p2=p1->l;
			p1->l=p2->r;
			p2->r=p1;
			p->r=p2->l;
			p2->l=p;
			if (p2->bal==+1) p->bal=-1; else p->bal=0;
			if (p2->bal==-1) p1->bal=1; else p1->bal=0;
			p=p2;
		}
		p->bal=0;
		h=0;
	};

	void searchsub(TBtree_item<A>*& ppnode, A skey, short& h,
		TBtree_item<A>*& solnode, int autoadd)
	{
		if (ppnode==NULL)
		{
			h=0;
			if (autoadd) { h=1; ppnode=new TBtree_item<A>(skey); }
			solnode=ppnode;
			return;
		}
		else
		{
			if (skey==ppnode->key)	{ solnode=ppnode; h=0; }
			else {
				if (skey<ppnode->key)
				{
					searchsub(ppnode->l,skey,h,solnode,autoadd);
					if (h>0)
					{
						if (ppnode->bal==+1) { ppnode->bal=0; h=0;	}
						else 
						{
							if (ppnode->bal== 0) { ppnode->bal=-1; } 
							else 
							{
								if (ppnode->bal==-1) balanceleft(ppnode,h);
							} 
						}
					}
				}
				else
				{
					searchsub(ppnode->r,skey,h,solnode,autoadd);
					if (h>0)
					{
						if (ppnode->bal==-1) { ppnode->bal=0; h=0; }
						else
						{
							if (ppnode->bal== 0) { ppnode->bal=1; } 
							else 
							{
								if (ppnode->bal==+1) balanceright(ppnode,h);
							}
						}
					}
				}
			}


		}
	};
	TBtree_item<A>* searchadd(A skey)
	{
		short h;
		TBtree_item<A> *solnode;

		searchsub(root,skey,h,solnode,1);
		return solnode;
	};
	TBtree_item<A>* search(A skey)
	{
		short h;
		TBtree_item<A> *solnode;

		searchsub(root,skey,h,solnode,0);
		return solnode;
	}
public:
	TBtree()
	{
		root=NULL;
	};
	~TBtree()
	{
		if (root!=NULL) delete root;
	};

public:
	void reset()
	{
		delete root;
		root=NULL;
	};
	void add(A str, int idx)
	{
		TBtree_item<A>* item=searchadd((A)str);
		if (item!=NULL) item->index=idx+1;
	};
	int get(A str)
	{
		TBtree_item<A>* item=search((A)str);
		if (item!=NULL) return item->index-1;
		return -1;
	};
};




class Ttextindex
{
private:
	int base,wordsize,andmask;
	int maxnr;
	char charconvert[256];
	Tarray<Tsortedarray<unsigned char> > index;
	Tchararray inuse;
public:
	Ttextindex();
	~Ttextindex();
	void addline(int nr, const char *txt, int txtlen);
	bool ispresent(int nr, const char *txt, int txtlen);
	void filter();
	void writefile(FILE *fp);
	void readfile(FILE *fp);
};


class Tprogress
{
public:
	virtual void settext(const char* txt)=0;
	virtual void setpos(double val)=0;
};

void strcpy(CString &str, const char *vl);
void strcat(CString &str, const char *vl);


double sqr(double vl);

void message(StrPtr content);

void reporttimer(StrPtr conttent);

void debugmessage(const char* content);


#define TOL 1.0e-9
