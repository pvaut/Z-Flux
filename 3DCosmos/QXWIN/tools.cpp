
#include "stdafx.h"

#include<stdio.h>
#include<math.h>
#include<string.h>

#include "tools.h"


//*************************************************
//   Sort using strings
//*************************************************
/*
void sort_string(int n, char** str, int* idx)
{
	int i,ir,j,l;
	int rraidx;

	if (n<2) return;
	l=(n>>1)+1;
	ir=n;
	for(;;) {
		if (l>1) {
			rraidx=idx[--l];
		} else {
			rraidx=idx[ir];
			idx[ir]=idx[1];
			if (--ir == 1) {
				idx[1]=rraidx;
				break;
			}
		}
		i=l;
		j=l+l;
		while (j <= ir) {
			if ( (j<ir) && (stricmp(str[idx[j]],str[idx[j+1]])<0) ) j++;
			if (stricmp(str[rraidx],str[idx[j]])<0) {
				idx[i]=idx[j];
				i=j;
				j <<= 1;
			} else j=ir+1;
		}
		idx[i]=rraidx;
	}
}



//*************************************************
//   Sort using strings(first) and ints(second)
//*************************************************

void sort_stringandint(int n, char** str, int* val, int* idx)
{
	int i,ir,j,l;
	int rraidx;

	if (n<2) return;
	l=(n>>1)+1;
	ir=n;
	for(;;) {
		if (l>1) {
			rraidx=idx[--l];
		} else {
			rraidx=idx[ir];
			idx[ir]=idx[1];
			if (--ir == 1) {
				idx[1]=rraidx;
				break;
			}
		}
		i=l;
		j=l+l;
		while (j <= ir) {
			if ( (j<ir) && 
				 ( 
				    (stricmp(str[idx[j]],str[idx[j+1]])<0)||
				    ((stricmp(str[idx[j]],str[idx[j+1]])==0)&&
					 (val[idx[j]]<val[idx[j+1]]))
				 ) 
			   ) j++;
			if (
				    (stricmp(str[rraidx],str[idx[j]])<0)||
				    ((stricmp(str[rraidx],str[idx[j]])==0)&&
				     (val[rraidx]<val[idx[j]]))
			   )
			{
				idx[i]=idx[j];
				i=j;
				j <<= 1;
			} else j=ir+1;
		}
		idx[i]=rraidx;
	}
}
*/



//*************************************************
//   Sort using ints
//*************************************************

void sort_int(int n, int* val, int* ra)
{
	int i,ir,j,l;
	int rra;

	if (n<2) return;
	l=(n>>1)+1;
	ir=n;
	for(;;) {
		if (l>1) {
			rra=ra[--l];
		} else {
			rra=ra[ir];
			ra[ir]=ra[1];
			if (--ir == 1) {
				ra[1]=rra;
				break;
			}
		}
		i=l;
		j=l+l;
		while (j <= ir) {
			if ( (j<ir) && (val[ra[j]]<val[ra[j+1]]) ) j++;
			if (val[rra]<val[ra[j]]) {
				ra[i]=ra[j];
				i=j;
				j <<= 1;
			} else j=ir+1;
		}
		ra[i]=rra;
	}
}



//*************************************************
//   Sort using doubles
//*************************************************

void sort_double(int n, double* val, int* ra)
{
	int i,ir,j,l;
	int rra;

	if (n<2) return;
	l=(n>>1)+1;
	ir=n;
	for(;;) {
		if (l>1) {
			rra=ra[--l];
		} else {
			rra=ra[ir];
			ra[ir]=ra[1];
			if (--ir == 1) {
				ra[1]=rra;
				break;
			}
		}
		i=l;
		j=l+l;
		while (j <= ir) {
			if ( (j<ir) && (val[ra[j]]<val[ra[j+1]]) ) j++;
			if (val[rra]<val[ra[j]]) {
				ra[i]=ra[j];
				i=j;
				j <<= 1;
			} else j=ir+1;
		}
		ra[i]=rra;
	}
}


void sort_doubleandint(int n, double* sortval, int* val, int* idx)
{
	int i,ir,j,l;
	int rraidx;

	if (n<2) return;
	l=(n>>1)+1;
	ir=n;
	for(;;) {
		if (l>1) {
			rraidx=idx[--l];
		} else {
			rraidx=idx[ir];
			idx[ir]=idx[1];
			if (--ir == 1) {
				idx[1]=rraidx;
				break;
			}
		}
		i=l;
		j=l+l;
		while (j <= ir) {
			if ( (j<ir) && 
				 ( 
				    (sortval[idx[j]]<sortval[idx[j+1]])||
				    ((sortval[idx[j]]==sortval[idx[j+1]])&&
					 (val[idx[j]]<val[idx[j+1]]))
				 ) 
			   ) j++;
			if (
				    (sortval[rraidx]<sortval[idx[j]])||
				    ((sortval[rraidx]==sortval[idx[j]])&&
				     (val[rraidx]<val[idx[j]]))
			   )
			{
				idx[i]=idx[j];
				i=j;
				j <<= 1;
			} else j=ir+1;
		}
		idx[i]=rraidx;
	}
}


void intarray_sort(Tintarray &ar_vals, Tintarray &ar_idx)
{
	int *ptr_idx;

	ptr_idx=ar_idx.G_ptr();
	ptr_idx--;
	sort_int(ar_vals.G_count(),ar_vals.G_ptr(),ptr_idx);
}

void doublearray_sort(Tdoublearray &ar_vals, Tintarray &ar_idx)
{
	int *ptr_idx;

	ptr_idx=ar_idx.G_ptr();
	ptr_idx--;
	sort_double(ar_vals.G_count(),ar_vals.G_ptr(),ptr_idx);
}

/*
void stringarray_sort(Titemarray<char*> &ar_vals, Tintarray &ar_idx)
{
	int *ptr_idx;

	ptr_idx=ar_idx.G_ptr();
	ptr_idx--;
	sort_string(ar_vals.G_count(),ar_vals.G_ptr(),ptr_idx);
}

void stringintarray_sort(Titemarray<char*> &ar_vals1, Tintarray &ar_vals2, Tintarray &ar_idx)
{
	int *ptr_idx;

	ptr_idx=ar_idx.G_ptr();
	ptr_idx--;
	sort_stringandint(ar_vals1.G_count(),ar_vals1.G_ptr(),ar_vals2.G_ptr(),ptr_idx);
}
*/


/*
int findstring(const char *str, const char *findstr, int casesens)
{
	int i,rng,len;

	len=strlen(findstr);
	if (len<=0) return 0;
	rng=strlen(str)-len;
	if (!casesens)
	{
		for (i=0; i<=rng; i++)
			if (strnicmp(&str[i],findstr,len)==0) return i;
	}
	else
	{
		for (i=0; i<=rng; i++)
			if (strncmp(&str[i],findstr,len)==0) return i;
	}
	return -1;
}
*/

#define tSWAP(a,b) temp=(a);(a)=(b);(b)=temp;

template<class A>
A select_kth_smallest(int k, int n, A *arr)//zero start index
{
	int i,ir,j,l,mid;
	A a,temp;

	arr--;k++;//trick to make zero-based

	l=1;
	ir=n;
	for(;;) {
		if (ir<=l+1) {
			if (ir == l+1 && arr[ir] < arr[l]) {
				tSWAP(arr[l],arr[ir])
			}
			return arr[k];
		} else {
			mid=(l+ir) >> 1;
			tSWAP(arr[mid],arr[l+1])
			if (arr[l+1] > arr[ir]) {
				tSWAP(arr[l+1],arr[ir])
			}
			if (arr[l] > arr[ir]) {
				tSWAP(arr[l],arr[ir])
			}
			if (arr[l+1] > arr[l]) {
				tSWAP(arr[l+1],arr[l])
			}
			i=l+1;
			j=ir;
			a=arr[l];
			for (;;) {
				do i++; while (arr[i] < a);
				do j--; while (arr[j] > a);
				if (j<i) break;
				tSWAP(arr[i],arr[j])
			}
			arr[l]=arr[j];
			arr[j]=a;
			if (j >= k) ir=j-1;
			if (j<= k) l=i;
		}
	}
}


double select_kth_smallest_double(int k, int n, double *arr)//zero start index
{
	return select_kth_smallest(k,n,arr);
}

double calcmedian_double(int n, double *arr)
{
	double v1,v2;
	int mid;
	if (n==0) return 0.0;
	if (n%2==0)
	{
		mid=n/2;
		v1=select_kth_smallest_double(mid-1,n,arr);
		v2=select_kth_smallest_double(mid-0,n,arr);
		return (v1+v2)/2;
	}
	else
	{
		mid=n/2;
		v1=select_kth_smallest_double(mid,n,arr);
		return v1;
	}
}



int select_kth_smallest_int(int k, int n, int *arr)//zero start index
{
	return select_kth_smallest(k,n,arr);
}

int calcmedian_int(int n, int *arr)
{
	int v1,v2;
	int mid;
	if (n==0) return 0;
	if (n%2==0)
	{
		mid=n/2;
		v1=select_kth_smallest_int(mid-1,n,arr);
		v2=select_kth_smallest_int(mid-0,n,arr);
		return (v1+v2)/2;
	}
	else
	{
		mid=n/2;
		v1=select_kth_smallest_int(mid,n,arr);
		return v1;
	}
}


//*************************************************
// Tbaltree_item
//*************************************************

class Tbaltree_item : public CObject
{
	friend class Tbaltree;
private:
	Tbaltree_item *l,*r;
	short bal;
	QString key;
	int index;
public:
	Tbaltree_item(StrPtr ikey);
	~Tbaltree_item();
};

Tbaltree_item::Tbaltree_item(StrPtr ikey)
{
	key=ikey;
	index=0;
	l=NULL;
	r=NULL;
	bal=0;
}

Tbaltree_item::~Tbaltree_item()
{
	if (l!=NULL) delete l;
	if (r!=NULL) delete r;
}



//*************************************************
// Tbaltree
//*************************************************

Tbaltree::Tbaltree()
{
	root=NULL;
}

Tbaltree::~Tbaltree()
{
	if (root!=NULL) delete root;
}

void Tbaltree::balanceleft(Tbaltree_item*& p, short& h)
{
	Tbaltree_item *p1,*p2;

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
}


void Tbaltree::balanceright(Tbaltree_item*& p, short& h)
{
	Tbaltree_item *p1,*p2;

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
}


void Tbaltree::searchsub(Tbaltree_item*& ppnode, StrPtr skey, short& h,
						Tbaltree_item*& solnode, int autoadd)
{
	short int k;
	if (ppnode==NULL)
	{
		h=0;
		if (autoadd) { h=1; ppnode=new Tbaltree_item(skey); }
		solnode=ppnode;
		return;
	}
	else
	{
		k=qstricmp(skey,ppnode->key);
		if (k<0)
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
		if (k>0)
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
		if (k==0)	{ solnode=ppnode; h=0; }
	}
}


Tbaltree_item* Tbaltree::searchadd(StrPtr skey)
{
	short h;
	Tbaltree_item *solnode;

	searchsub(root,skey,h,solnode,1);
	return solnode;
}


Tbaltree_item* Tbaltree::search(StrPtr skey)
{
	short h;
	Tbaltree_item *solnode;

	searchsub(root,skey,h,solnode,0);
	return solnode;
}





void Tbaltree::reset()
{
	delete root;
	root=NULL;
}

void Tbaltree::add(StrPtr str, int idx)
{
	Tbaltree_item* item=searchadd(str);
	if (item!=NULL) item->index=idx+1;
}

int Tbaltree::get(StrPtr str)
{
	Tbaltree_item* item=search(str);
	if (item!=NULL) return item->index-1;
	return -1;
}






//******************************************************
// Ttextindex
//******************************************************


Ttextindex::Ttextindex()
{
	int i;
	unsigned char ch;

	base=4;
	wordsize=3;

	maxnr=0;

	for (i=0; i<256; i++) charconvert[i]=0;
	for (ch='a'; ch<='z'; ch++) charconvert[ch]=1+(ch-'a')%((1<<base)-1);
	for (ch='A'; ch<='Z'; ch++) charconvert[ch]=1+(ch-'A')%((1<<base)-1);
	for (ch='0'; ch<='9'; ch++) charconvert[ch]=1+(ch-'0')%((1<<base)-1);


	andmask=1<<(base*wordsize);
	andmask=andmask-1;

	for (i=0; i<1<<(base*wordsize); i++)
	{
		index.add(new Tsortedarray<unsigned char>);
		inuse.add(1);
	}
}

Ttextindex::~Ttextindex()
{
}

void Ttextindex::addline(int nr, const char *txt, int txtlen)
{
	int val;
	int i;
	unsigned char ch;


	maxnr=max(maxnr,nr);
	val=0;
	for (i=0; i<txtlen; i++)
	{
		val=val<<base;
		val=val&andmask;
		ch=charconvert[txt[i]];
		val=val+ch;
		if ((i>=wordsize-1)&&(charconvert[txt[i]]>0)&&(charconvert[txt[i-1]]>0)&&(charconvert[txt[i-2]]>0))
		{
			if (index.get(val)->findfirst(nr)<0)
				index.get(val)->add(nr);
		}
	}
}

void Ttextindex::filter()
{
	int i;

	for (i=0; i<1<<(base*wordsize); i++)
	{
		if (index.get(i)->G_count()>=maxnr/2)
		{
			inuse.set(i,0);
			index.get(i)->forget();
		}
	}

	int totct;
	totct=0;
	for (i=0; i<1<<base*wordsize; i++) totct+=index.get(i)->G_count();

	i=i;
	i=i;
}


bool Ttextindex::ispresent(int nr, const char *txt, int txtlen)
{
	int val;
	int i;
	unsigned char ch;



	if (txtlen<wordsize) return 1;

	val=0;
	for (i=0; i<txtlen; i++)
	{
		val=val<<base;
		val=val&andmask;
		ch=charconvert[txt[i]];
		val=val+ch;
		if ((i>=wordsize-1)&&(charconvert[txt[i]]>0)&&(charconvert[txt[i-1]]>0)&&(charconvert[txt[i-2]]>0))
			if ((inuse.get(val))&&(index.get(val)->findfirst(nr)<0)) return 0;
	}
	return 1;
}



void Ttextindex::writefile(FILE *fp)
{
	int i,ct;

	filter();
	for (i=0; i<1<<(base*wordsize); i++)
	{
		if (!inuse.get(i))
		{
			ct=99999999;
			fwrite(&ct,sizeof(int),1,fp);
		}
		else
		{
			ct=index.get(i)->G_count();
			fwrite(&ct,sizeof(int),1,fp);
			fwrite(index.get(i)->G_ptr(),1,ct,fp);
		}
	}
}


void Ttextindex::readfile(FILE *fp)
{
	int i,ct;

	for (i=0; i<1<<(base*wordsize); i++)
	{
		fread(&ct,sizeof(int),1,fp);
		if (ct==99999999)
		{
			inuse.set(i,0);
		}
		else
		{
			if ((ct<0)||(ct>=257)) Beep(1000,50);
			inuse.set(i,1);
			index.get(i)->redim(ct);
			fread(index.get(i)->G_ptr(),1,ct,fp);
		}
	}
}


void strcpy(CString &str, const char *vl)
{
	str=vl;
}
void strcat(CString &str, const char *vl)
{
	str+=vl;
}

//double sqr(double vl) { return vl*vl; }