#include "stdafx.h"
#include "matrix.h"

#include "qfile.h"

#include "SC_func.h"
#include "SC_env.h"

#include "qxstoredbitmap.h"

#include "numrecip_svd.h"

//*********************************************************
// TMatrix
//*********************************************************

TMatrix::TMatrix()
{
	dim1=0;dim2=0;
}

void TMatrix::redim(int idim1, int idim2)
{
	if ((dim1==idim1)&&(dim2==idim2)) return;
	dim1=idim1;dim2=idim2;
/*	while (vals.G_count()>dim1) vals.del(vals.G_count()-1);
	for (int i1=0; i1<vals.G_count(); i1++)
	{
		while (vals[i1]->G_count()>dim2) vals[i1]->del(vals[i1]->G_count()-1);
		while (vals[i1]->G_count()<dim2) vals[i1]->add(0);
	}
	while (vals.G_count()<dim1)
	{
		Tdoublearray *row=new Tdoublearray;
		for (int i=0; i<dim2; i++) row->add(0);
		vals.add(row);
	}*/
	vals.reset();
	while (vals.G_count()<dim1)
	{
		Tdoublearray *row=new Tdoublearray;
		for (int i=0; i<dim2; i++) row->add(0);
		vals.add(row);
	}
}


void TMatrix::makeunit(int idim)
{
	redim(idim,idim);
	for (int i=0; i<idim; i++)
		for (int j=0; j<idim; j++)
		{
			if (i==j) Set(i,j,1);
			else Set(i,j,0);
		}
}

void TMatrix::addscalar(double offset)
{
	for (int i=0; i<dim1; i++)
		for (int j=0; j<dim2; j++)
			Set(i,j,Get(i,j)+offset);
}


double TMatrix::G_minval() const
{
	double minval=1.0e99;
	for (int i=0; i<dim1; i++)
		for (int j=0; j<dim2; j++)
			if (Get(i,j)<minval) minval=Get(i,j);
	return minval;
}

double TMatrix::G_maxval() const
{
	double maxval=-1.0e99;
	for (int i=0; i<dim1; i++)
		for (int j=0; j<dim2; j++)
			if (Get(i,j)>maxval) maxval=Get(i,j);
	return maxval;
}


void TMatrix::tostring(QString &str, int tpe) const
{
	if (tpe==0)
	{
		FormatString(str,_text("^1x^2 ("),dim1,dim2);
		QString st1;
		for (int i1=0; i1<min(10,dim1); i1++)
		{
			if (i1>0) str+=_qstr(",");
			str+=_qstr(" (");
			for (int i2=0; i2<min(10,dim2); i2++)
			{
				if (i2>0) str+=_qstr(", ");
				st1.formatscalar(Get(i1,i2));str+=st1;
			}
			str+=_qstr(") ");
		}
		str+=_qstr(")");
	}
	if (tpe==999)
	{
		QString st1;
		for (int i1=0; i1<min(10,dim1); i1++)
		{
			if (i1>0) str+=_qstr("\r\n");
			str+=_qstr("(");
			for (int i2=0; i2<min(10,dim2); i2++)
			{
				if (i2>0) str+=_qstr(", ");
				st1.formatscalar(Get(i1,i2));str+=st1;
			}
			str+=_qstr(")");
		}
	}
}

void TMatrix::fromstring(StrPtr str)
{
}

void TMatrix::operator=(const TMatrix &m)
{
	redim(m.dim1,m.dim2);
	for (int i1=0; i1<dim1; i1++)
		for (int i2=0; i2<dim2; i2++)
			Set(i1,i2,m.Get(i1,i2));
}

void TMatrix::copyfrom(const Thomogcoord *coord)
{
	redim(4,1);
	for (int i1=0; i1<dim1; i1++) Set(i1,0,coord->G_coord(i1));
}

void TMatrix::copyto(Thomogcoord *coord) const
{
	if ((dim1!=4)||(dim2!=1)) throw QError(_text("Convert matrix to point: invalid matrix dimensions"));
	for (int i1=0; i1<dim1; i1++) coord->Set_coord(i1,Get(i1,0));
}

void TMatrix::loadfromfile(StrPtr filename, int res1, int idx1min, int idx1max, int res2, int idx2min, int idx2max, int bytecount)
{
	int idx,ii;


	if ((qfind(filename,_qstr(".jpg"))>0)||(qfind(filename,_qstr(".JPG"))>0))//load as jpeg
	{
		Tbmp bitmap;
		bitmap.loadgdiplus(filename);
		if (bitmap.G_format()==3)
		{
			res1=bitmap.G_xres();
			res2=bitmap.G_yres();
			if (idx1max<=0) idx1max=res1-1;
			if (idx2max<=0) idx2max=res2-1;
			redim(idx1max-idx1min+1,idx2max-idx2min+1);
			for (int i1=idx1min; i1<=idx1max; i1++)
				for (int i2=idx2min; i2<=idx2max; i2++)
				{
					idx=i1+i2*res1;
					Set(i1-idx1min,i2-idx2min,(1.0*bitmap.G_val8rptr()[idx]+1.0*bitmap.G_val8gptr()[idx]+1.0*bitmap.G_val8bptr()[idx])/3.0);
				}
		}
		return;
	}

	if ((qfind(filename,_qstr(".img"))>0)||(qfind(filename,_qstr(".IMG"))>0)&&(bytecount==16))//load as binary 'image' file
	{
		if (idx1max<=0) idx1max=res1-1;
		if (idx2max<=0) idx2max=res2-1;

		redim(idx1max-idx1min+1,idx2max-idx2min+1);
		QBinfile file;
		file.openread(filename);

		Tchararray row;
		for (int i1=0; i1<res1*bytecount; i1++) row.add(0);

		//skip offset
		for (int i2=0; i2<idx2min; i2++) file.readblock(res1*bytecount,row.G_ptr());

		unsigned char a[2];
		for (int i2=0; i2<=idx2max-idx2min; i2++)
		{
			file.readblock(res1*bytecount,row.G_ptr());
			for (int i1=0; i1<=idx1max-idx1min; i1++)
			{
				ii=(idx1min+i1);
				if (ii<0) ii+=res1;
				ii=ii%res1;
				a[1]= (unsigned char)row[ii*2+0];
				a[0]= (unsigned char)row[ii*2+1];
				Set(idx1max-idx1min-i1,idx2max-idx2min-i2,*((__int16*)a));
			}
		}
		file.close();
		return;
	}


	throw QError(_text("Unrecognised data file for matrix loading"));

}



void TMatrix::calctranspose(const TMatrix *m)
{
	redim(m->G_dim1(),m->G_dim2());
	for (int i1=0; i1<dim1; i1++)
		for (int i2=0; i2<dim2; i2++)
			Set(i1,i2,m->Get(i2,i1));
}

void TMatrix::mulfrom(const TMatrix *m, double vl)
{
	redim(m->G_dim1(),m->G_dim2());
	for (int i1=0; i1<dim1; i1++)
		for (int i2=0; i2<dim2; i2++)
			Set(i1,i2,vl*m->Get(i1,i2));
}

void TMatrix::mulby(double vl)
{
	for (int i1=0; i1<dim1; i1++)
		for (int i2=0; i2<dim2; i2++)
			Set(i1,i2,vl*Get(i1,i2));
}


void TMatrix::add(const TMatrix *m1, const TMatrix *m2)
{
	if ((m1->G_dim1()!=m2->G_dim1())||(m2->G_dim2()!=m2->G_dim2()))
		throw QError(_text("Inconsistent dimensions for matrix addition"));
	redim(m1->G_dim1(),m1->G_dim2());
	for (int i1=0; i1<dim1; i1++)
		for (int i2=0; i2<dim2; i2++)
			Set(i1,i2,m1->Get(i1,i2)+m2->Get(i1,i2));
}

void TMatrix::subtract(const TMatrix *m1, const TMatrix *m2)
{
	if ((m1->G_dim1()!=m2->G_dim1())||(m2->G_dim2()!=m2->G_dim2()))
		throw QError(_text("Inconsistent dimensions for matrix addition"));
	redim(m1->G_dim1(),m1->G_dim2());
	for (int i1=0; i1<dim1; i1++)
		for (int i2=0; i2<dim2; i2++)
			Set(i1,i2,m1->Get(i1,i2)-m2->Get(i1,i2));
}

void TMatrix::multiply(const TMatrix *m1, const TMatrix *m2)
{
	if (m1->G_dim2()!=m2->G_dim1())
		throw QError(_text("Inconsistent dimensions for matrix multiplication"));
	redim(m1->G_dim1(),m2->G_dim2());
	int dimbetween=m2->G_dim1();
	int k;
	for (int i1=0; i1<dim1; i1++)
		for (int i2=0; i2<dim2; i2++)
		{
			double vl=0;
			for (k=0; k<dimbetween; k++)
				vl+=m1->Get(i1,k)*m2->Get(k,i2);
			Set(i1,i2,vl);
		}
}

void TMatrix::divide(const TMatrix *b, const TMatrix *A)
{
	if (A->G_dim1()!=b->G_dim1()) throw QError(_text("Matrix division: inconsistent number of rows"));

	TMatrix U,V,W,T1,T2,Utrans;
	Tdoublearray eigvals;
	A->SVD(&U,&eigvals,&V);
	W.redim(eigvals.G_count(),eigvals.G_count());
	Utrans.calctranspose(&U);
	for (int i=0; i<eigvals.G_count(); i++)
	{
		if (fabs(eigvals[i]/eigvals[0])<1.0e-11) throw QError(_text("Unable to divide by singular matrix"));
		W.Set(i,i,1.0/eigvals[i]);
	}
	T1.multiply(&V,&W);
	T2.multiply(&T1,&Utrans);
	multiply(&T2,b);

}




void TMatrix::mul(const Thomogcoord *arg, Thomogcoord *result) const
{
	if ((G_dim1()!=4)||(G_dim1()!=4)) throw QError(_qstr("Invalid matrix dimensions for multiplication with homogeneous coordinates"));
	for (int i1=0; i1<4; i1++)
	{
		double vl=0;
		for (int i2=0; i2<4; i2++) vl+=Get(i1,i2)*arg->G_coord(i2);
		result->Set_coord(i1,vl);
	}
}

void TMatrix::multransp(const Thomogcoord *arg, Thomogcoord *result) const
{
	if ((G_dim1()!=4)||(G_dim1()!=4)) throw QError(_qstr("Invalid matrix dimensions for multiplication with homogeneous coordinates"));
	for (int i1=0; i1<4; i1++)
	{
		double vl=0;
		for (int i2=0; i2<4; i2++) vl+=Get(i2,i1)*arg->G_coord(i2);
		result->Set_coord(i1,vl);
	}
}


void TMatrix::SVD(TMatrix *U, Tdoublearray *eigvals, TMatrix *V) const
{
	double **u,**v,*w;
	int i,j;

	u=(double**)malloc(sizeof(double*)*(dim1+1));
	for (i=0; i<=dim1; i++) u[i]=(double*)malloc(sizeof(double)*(dim2+1));
	v=(double**)malloc(sizeof(double*)*(dim2+1));
	for (i=0; i<=dim2; i++) v[i]=(double*)malloc(sizeof(double)*(dim2+1));
	w=(double*)malloc(sizeof(double)*(dim2+1));

	for (i=0; i<dim1; i++)
		for (j=0; j<dim2; j++)
			u[i+1][j+1]=Get(i,j);

	svdcmp(u,dim1,dim2,w,v);

	U->redim(dim1,dim2);
	for (i=0; i<dim1; i++)
		for (j=0; j<dim2; j++)
			U->Set(i,j,u[i+1][j+1]);

	eigvals->reset();
	for (i=0; i<dim2; i++) eigvals->add(w[i+1]);

	V->redim(dim2,dim2);
	for (i=0; i<dim2; i++)
		for (j=0; j<dim2; j++)
			V->Set(i,j,v[i+1][j+1]);

	for (i=0; i<=dim1; i++) free(u[i]);
	for (i=0; i<=dim2; i++) free(v[i]);
	free(u);free(v);free(w);
}

void TMatrix::calcinverse(const TMatrix *M)
{
	if (M->G_dim1()!=M->G_dim2()) throw QError(_text("Unable to invert non-rectangular matrix:"));
	TMatrix U,V,W,T1,Utrans;
	Tdoublearray eigvals;
	M->SVD(&U,&eigvals,&V);
	W.redim(eigvals.G_count(),eigvals.G_count());
	Utrans.calctranspose(&U);
	for (int i=0; i<eigvals.G_count(); i++)
	{
		if (fabs(eigvals[i]/eigvals[0])<1.0e-31)
			throw QError(_text("Unable to invert singular matrix"));
		W.Set(i,i,1.0/eigvals[i]);
	}
	T1.multiply(&V,&W);
	multiply(&T1,&Utrans);
}

void TMatrix::streamout(QBinTagWriter &writer)
{
	writer.write_int32(dim1);
	writer.write_int32(dim2);
	for (int i=0; i<dim1; i++)
		writer.write_block(vals[i]->G_ptr(),sizeof(double)*dim2);
}
void TMatrix::streamin(QBinTagReader &reader)
{
	int d1=reader.read_int32();
	int d2=reader.read_int32();
	redim(d1,d2);
	for (int i=0; i<dim1; i++)
		reader.read_block(vals[i]->G_ptr(),sizeof(double)*dim2);
}



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_matrix,matrix)
{
	setclasspath_fromtype(SC_valname_matrix);
	setreturntype(SC_valname_matrix);
	addvar(_qstr("dim1"),SC_valname_scalar);
	addvar(_qstr("dim2"),SC_valname_scalar,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TMatrix *m=G_valuecontent<TMatrix>(retval);
	int dim1=1,dim2=1;
	if (arglist->G_ispresent(0)) { dim1=arglist->get(0)->G_content_scalar()->G_intval(); dim2=dim1; }
	if (arglist->G_ispresent(1)) { dim2=arglist->get(1)->G_content_scalar()->G_intval(); }
	m->redim(dim1,dim2);
}
ENDFUNCTION(func_matrix)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_unitmatrix,unitmatrix)
{
	setclasspath_fromtype(SC_valname_matrix);
	setreturntype(SC_valname_matrix);
	addvar(_qstr("dim"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TMatrix *m=G_valuecontent<TMatrix>(retval);
	int dim1=arglist->get(0)->G_content_scalar()->G_intval();
	m->makeunit(dim1);
}
ENDFUNCTION(func_unitmatrix)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_matrixdim1,dim1)
{
	setcanassign();
	setmemberfunction(SC_valname_matrix);
	setreturntype(SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TMatrix *m=G_valuecontent<TMatrix>(owner);
	if (assigntoval!=NULL)
		m->redim(assigntoval->G_content_scalar()->G_intval(),m->G_dim2());
	retval->G_content_scalar()->copyfrom(m->G_dim1());
}
ENDFUNCTION(func_matrixdim1)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_matrixdim2,dim2)
{
	setcanassign();
	setmemberfunction(SC_valname_matrix);
	setreturntype(SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TMatrix *m=G_valuecontent<TMatrix>(owner);
	if (assigntoval!=NULL)
		m->redim(m->G_dim1(),assigntoval->G_content_scalar()->G_intval());
	retval->G_content_scalar()->copyfrom(m->G_dim2());
}
ENDFUNCTION(func_matrixdim2)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_matrixget,get)
{
	setcanassign();
	setmemberfunction(SC_valname_matrix);
	setreturntype(SC_valname_scalar);
	addvar(_qstr("idx1"),SC_valname_scalar);
	addvar(_qstr("idx2"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TMatrix *m=G_valuecontent<TMatrix>(owner);
	int idx1=arglist->get(0)->G_content_scalar()->G_intval();
	int idx2=arglist->get(1)->G_content_scalar()->G_intval();
	if (assigntoval!=NULL)
		m->Set(idx1,idx2,assigntoval->G_content_scalar()->G_val());
	retval->G_content_scalar()->copyfrom(m->Get(idx1,idx2));
}
ENDFUNCTION(func_matrixget)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_matrixaddscalar,AddScalar)
{
	setmemberfunction(SC_valname_matrix);
	addvar(_qstr("offset"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TMatrix *m=G_valuecontent<TMatrix>(owner);
	m->addscalar(arglist->get(0)->G_content_scalar()->G_val());
}
ENDFUNCTION(func_matrixaddscalar)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_matrixgetmin,GetMinVal)
{
	setmemberfunction(SC_valname_matrix);
	setreturntype(SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TMatrix *m=G_valuecontent<TMatrix>(owner);
	retval->G_content_scalar()->copyfrom(m->G_minval());
}
ENDFUNCTION(func_matrixgetmin)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_matrixgetmax,GetMaxVal)
{
	setmemberfunction(SC_valname_matrix);
	setreturntype(SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TMatrix *m=G_valuecontent<TMatrix>(owner);
	retval->G_content_scalar()->copyfrom(m->G_maxval());
}
ENDFUNCTION(func_matrixgetmax)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_matrix_transpose,transpose)
{
	setclasspath_fromtype(SC_valname_matrix);
	setreturntype(SC_valname_matrix);
	addvar(_qstr("arg"),SC_valname_matrix);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	(G_valuecontent<TMatrix>(retval))->calctranspose(G_valuecontent<TMatrix>(arglist->get(0)));
}
ENDFUNCTION(func_matrix_transpose)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_matrix_SVD,SVD)
{
	setmemberfunction(SC_valname_matrix);
	addvar(_qstr("U"),SC_valname_matrix);
	addvar(_qstr("W"),SC_valname_matrix);
	addvar(_qstr("V"),SC_valname_matrix);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tdoublearray eigvals;
	(G_valuecontent<TMatrix>(owner))->SVD(G_valuecontent<TMatrix>(arglist->get(0)),&eigvals,G_valuecontent<TMatrix>(arglist->get(2)));
	TMatrix *W=G_valuecontent<TMatrix>(arglist->get(1));
	W->redim(eigvals.G_count(),eigvals.G_count());
	for (int i=0; i<eigvals.G_count(); i++)
		W->Set(i,i,eigvals[i]);
}
ENDFUNCTION(func_matrix_SVD)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_mul_scalar_matrix,10,SC_valname_scalar,SC_valname_matrix,SC_valname_matrix)
{
	setclasspath_fromtype(SC_valname_matrix);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	double vl=val1->G_content_scalar()->G_val();
	TMatrix *m=G_valuecontent<TMatrix>(val2);
	G_valuecontent<TMatrix>(retval)->mulfrom(m,vl);
}
ENDOPERATOR(op_mul_scalar_matrix)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_div_scalar_matrix,11,SC_valname_scalar,SC_valname_matrix,SC_valname_matrix)
{
	setclasspath_fromtype(SC_valname_matrix);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	double vl=val1->G_content_scalar()->G_val();
	TMatrix *m=G_valuecontent<TMatrix>(val2);
	G_valuecontent<TMatrix>(retval)->calcinverse(m);
	G_valuecontent<TMatrix>(retval)->mulby(vl);
}
ENDOPERATOR(op_div_scalar_matrix)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_div_matrix_matrix,11,SC_valname_matrix,SC_valname_matrix,SC_valname_matrix)
{
	setclasspath_fromtype(SC_valname_matrix);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	TMatrix *m1=G_valuecontent<TMatrix>(val1);
	TMatrix *m2=G_valuecontent<TMatrix>(val2);
	G_valuecontent<TMatrix>(retval)->divide(m1,m2);
}
ENDOPERATOR(op_div_matrix_matrix)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_plus_matrix,8,SC_valname_matrix,SC_valname_matrix,SC_valname_matrix)
{
	setclasspath_fromtype(SC_valname_matrix);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	TMatrix *v1=G_valuecontent<TMatrix>(val1);
	TMatrix *v2=G_valuecontent<TMatrix>(val2);
	G_valuecontent<TMatrix>(retval)->add(v1,v2);
}
ENDOPERATOR(op_plus_matrix)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_min_matrix,9,SC_valname_matrix,SC_valname_matrix,SC_valname_matrix)
{
	setclasspath_fromtype(SC_valname_matrix);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	TMatrix *v1=G_valuecontent<TMatrix>(val1);
	TMatrix *v2=G_valuecontent<TMatrix>(val2);
	G_valuecontent<TMatrix>(retval)->subtract(v1,v2);
}
ENDOPERATOR(op_min_matrix)

///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_mul_matrix,10,SC_valname_matrix,SC_valname_matrix,SC_valname_matrix)
{
	setclasspath_fromtype(SC_valname_matrix);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	TMatrix *v1=G_valuecontent<TMatrix>(val1);
	TMatrix *v2=G_valuecontent<TMatrix>(val2);
	G_valuecontent<TMatrix>(retval)->multiply(v1,v2);
}
ENDOPERATOR(op_mul_matrix)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_matrix_loadfile,LoadFile)
{
	setmemberfunction(SC_valname_matrix);
	addvar(_qstr("FileName"),SC_valname_string);
	addvar(_qstr("Dim1"),SC_valname_scalar);
	addvar(_qstr("MinIdx1"),SC_valname_scalar);
	addvar(_qstr("MaxIdx1"),SC_valname_scalar);
	addvar(_qstr("Dim2"),SC_valname_scalar);
	addvar(_qstr("MinIdx2"),SC_valname_scalar);
	addvar(_qstr("MaxIdx2"),SC_valname_scalar);
	addvar(_qstr("ByteCount"),SC_valname_scalar);
}


void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TMatrix *mt=G_valuecontent<TMatrix>(owner);
	StrPtr filename=arglist->get(0)->G_content_string()->G_string();
	int dim1=arglist->get(1)->G_content_scalar()->G_intval();
	int minidx1=arglist->get(2)->G_content_scalar()->G_intval();
	int maxidx1=arglist->get(3)->G_content_scalar()->G_intval();

	int dim2=arglist->get(4)->G_content_scalar()->G_intval();
	int minidx2=arglist->get(5)->G_content_scalar()->G_intval();
	int maxidx2=arglist->get(6)->G_content_scalar()->G_intval();

	int bytecount=arglist->get(7)->G_content_scalar()->G_intval();

	mt->loadfromfile(filename,dim1,minidx1,maxidx1,dim2,minidx2,maxidx2,bytecount);
}
ENDFUNCTION(func_matrix_loadfile)
