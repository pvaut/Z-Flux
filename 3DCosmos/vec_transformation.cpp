#include "stdafx.h"
#include "valnames.h"
#include "vecmath.h"
#include "vec_transformation.h"

#include "SC_func.h"
#include "SC_env.h"

#include "3Dscene.h"

//*********************************************************
// Taffinetransformation
//*********************************************************

Taffinetransformation::Taffinetransformation()
{
	reset();
}

void Taffinetransformation::reset()
{
	M.makeunit(4);
	Minv.makeunit(4);
	isrescaling=false;
}

void Taffinetransformation::maketranslation(const Tvector *v)
{
	reset();
	M.Set(0,3,v->G_x());
	M.Set(1,3,v->G_y());
	M.Set(2,3,v->G_z());
	Minv.Set(0,3,-v->G_x());
	Minv.Set(1,3,-v->G_y());
	Minv.Set(2,3,-v->G_z());
}

void Taffinetransformation::makerotation(const Tvector *rotdir, double angle)
{
	Tvector dx,dy,dz,dxr,dyr,dzr;

	reset();
	dx.copyfrom(1,0,0);dy.copyfrom(0,1,0);dz.copyfrom(0,0,1);
	dxr.rotatefrom(&dx,rotdir,angle);
	dyr.rotatefrom(&dy,rotdir,angle);
	dzr.rotatefrom(&dz,rotdir,angle);
	M.Set(0,0,dxr.G_x());M.Set(1,0,dxr.G_y());M.Set(2,0,dxr.G_z());
	M.Set(0,1,dyr.G_x());M.Set(1,1,dyr.G_y());M.Set(2,1,dyr.G_z());
	M.Set(0,2,dzr.G_x());M.Set(1,2,dzr.G_y());M.Set(2,2,dzr.G_z());

	dx.copyfrom(1,0,0);dy.copyfrom(0,1,0);dz.copyfrom(0,0,1);
	dxr.rotatefrom(&dx,rotdir,-angle);
	dyr.rotatefrom(&dy,rotdir,-angle);
	dzr.rotatefrom(&dz,rotdir,-angle);
	Minv.Set(0,0,dxr.G_x());Minv.Set(1,0,dxr.G_y());Minv.Set(2,0,dxr.G_z());
	Minv.Set(0,1,dyr.G_x());Minv.Set(1,1,dyr.G_y());Minv.Set(2,1,dyr.G_z());
	Minv.Set(0,2,dzr.G_x());Minv.Set(1,2,dzr.G_y());Minv.Set(2,2,dzr.G_z());
}

void Taffinetransformation::makerotation_X(double angle)
{
	reset();
	M.Set(1,1, cos(angle)); M.Set(1,2, sin(angle));
	M.Set(2,1,-sin(angle)); M.Set(2,2, cos(angle));
	angle=-angle;
	Minv.Set(1,1, cos(angle)); Minv.Set(1,2, sin(angle));
	Minv.Set(2,1,-sin(angle)); Minv.Set(2,2, cos(angle));
}

void Taffinetransformation::makerotation_Y(double angle)
{
	reset();
	M.Set(0,0, cos(angle)); M.Set(0,2,-sin(angle));
	M.Set(2,0, sin(angle)); M.Set(2,2, cos(angle));
	angle=-angle;
	Minv.Set(0,0, cos(angle)); Minv.Set(0,2,-sin(angle));
	Minv.Set(2,0, sin(angle)); Minv.Set(2,2, cos(angle));
}

void Taffinetransformation::makerotation_Z(double angle)
{
	reset();
	M.Set(0,0, cos(angle)); M.Set(0,1, sin(angle));
	M.Set(1,0,-sin(angle)); M.Set(1,1, cos(angle));
	angle=-angle;
	Minv.Set(0,0, cos(angle)); Minv.Set(0,1, sin(angle));
	Minv.Set(1,0,-sin(angle)); Minv.Set(1,1, cos(angle));
}


void Taffinetransformation::updateinverse()
{
	Minv.calcinverse(&M);
}



void Taffinetransformation::operator=(const Taffinetransformation &v)
{
	M=v.M;
	Minv=v.Minv;
	isrescaling=v.isrescaling;
}

void Taffinetransformation::translatefrom(const Taffinetransformation *tf, const Tvector *v)
{
	Taffinetransformation tf1;tf1.maketranslation(v);
	multiply(tf,&tf1);
}

void Taffinetransformation::rotatefrom(const Taffinetransformation *tf, const Tvector *rotdir, double angle)
{
	Taffinetransformation tf1;tf1.makerotation(rotdir,angle);
	multiply(tf,&tf1);
}

void Taffinetransformation::invert()
{
	double vl;
	for (int i1=0; i1<4; i1++)
		for (int i2=0; i2<4; i2++)
		{
			vl=M.Get(i1,i2);
			M.Set(i1,i2,Minv.Get(i1,i2));
			Minv.Set(i1,i2,vl);
		}
}



void Taffinetransformation::scale(double iscalefactor)
{
	M.Set(0,0,M.Get(0,0)*iscalefactor);M.Set(1,0,M.Get(1,0)*iscalefactor);M.Set(2,0,M.Get(2,0)*iscalefactor);
	M.Set(0,1,M.Get(0,1)*iscalefactor);M.Set(1,1,M.Get(1,1)*iscalefactor);M.Set(2,1,M.Get(2,1)*iscalefactor);
	M.Set(0,2,M.Get(0,2)*iscalefactor);M.Set(1,2,M.Get(1,2)*iscalefactor);M.Set(2,2,M.Get(2,2)*iscalefactor);

	Minv.Set(0,0,Minv.Get(0,0)/iscalefactor);Minv.Set(1,0,Minv.Get(1,0)/iscalefactor);Minv.Set(2,0,Minv.Get(2,0)/iscalefactor);
	Minv.Set(0,1,Minv.Get(0,1)/iscalefactor);Minv.Set(1,1,Minv.Get(1,1)/iscalefactor);Minv.Set(2,1,Minv.Get(2,1)/iscalefactor);
	Minv.Set(0,2,Minv.Get(0,2)/iscalefactor);Minv.Set(1,2,Minv.Get(1,2)/iscalefactor);Minv.Set(2,2,Minv.Get(2,2)/iscalefactor);

	isrescaling=true;
}





void Taffinetransformation::copyfrom(const TMatrix *m)
{
	if ((m->G_dim1()!=4)||(m->G_dim2()!=4)) throw QError(_text("Convert matrix to transformation: invalid matrix dimension"));
	M=*m;
	Minv.calcinverse(&M);
	isrescaling=true;
}

void Taffinetransformation::copyto(TMatrix *m) const
{
	*m=M;
}

void Taffinetransformation::Set_origin(const Tvertex *v)
{
/*	TMatrix Mo,Minvo; test - not needed
	Mo=M;
	Minvo=Minv;*/


	M.Set(0,3,v->G3_x());
	M.Set(1,3,v->G3_y());
	M.Set(2,3,v->G3_z());
	M.Set(3,3,1);

	Tvector v0;v0.copyfrom(*v);
	Tvector v2;
	Minv.mul(&v0,&v2);
	Minv.Set(0,3,-v2.G_x());
	Minv.Set(1,3,-v2.G_y());
	Minv.Set(2,3,-v2.G_z());
	Minv.Set(3,3,1);

/*test - not needed
	{
		TMatrix tmp,unit;
		unit.makeunit(4);
		tmp.multiply(&Mo,&Minvo);
		double maxdf=0;
		for (int i=0; i<4; i++)
			for (int j=0; j<4; j++)
			{
				double df=fabs(tmp.Get(i,j)-unit.Get(i,j));
				if (df>maxdf) maxdf=df;
			}
		if (maxdf>0.001)
			int i=0;
	}

	{
		TMatrix tmp,unit;
		unit.makeunit(4);
		tmp.multiply(&M,&Minv);
		double maxdf=0;
		for (int i=0; i<4; i++)
			for (int j=0; j<4; j++)
			{
				double df=fabs(tmp.Get(i,j)-unit.Get(i,j));
				if (df>maxdf) maxdf=df;
			}
		if (maxdf>0.001)
			int i=0;
	}
*/
}

void Taffinetransformation::Set_origin_vector(const Tvector *v)
{
	Tvertex orig;orig.copyfrom(*v);
	Set_origin(&orig);
}


void Taffinetransformation::Set_xaxis(const Tvector *rt)
{
	M.Set(0,0,rt->G_x());
	M.Set(1,0,rt->G_y());
	M.Set(2,0,rt->G_z());
	try{
		Minv.calcinverse(&M);
	}
	catch (QError &err) {}
	isrescaling=true;
}

void Taffinetransformation::Set_yaxis(const Tvector *rt)
{
	M.Set(0,1,rt->G_x());
	M.Set(1,1,rt->G_y());
	M.Set(2,1,rt->G_z());
	try{
		Minv.calcinverse(&M);
	}
	catch (QError &err) {}
	isrescaling=true;
}

void Taffinetransformation::Set_zaxis(const Tvector *rt)
{
	M.Set(0,2,rt->G_x());
	M.Set(1,2,rt->G_y());
	M.Set(2,2,rt->G_z());
	try{
		Minv.calcinverse(&M);
	}
	catch (QError &err) {}
	isrescaling=true;
}


double Taffinetransformation::G_scalefactor() const
{
	double fc=0;
	fc+=sqr(M.Get(0,0))+sqr(M.Get(0,1))+sqr(M.Get(0,2));
	fc+=sqr(M.Get(1,0))+sqr(M.Get(1,1))+sqr(M.Get(1,2));
	fc+=sqr(M.Get(2,0))+sqr(M.Get(2,1))+sqr(M.Get(2,2));
	return fc/3;
}

void Taffinetransformation::G_origin(Tvertex *origin) const
{
	origin->S4_x(M.Get(0,3));
	origin->S4_y(M.Get(1,3));
	origin->S4_z(M.Get(2,3));
	origin->S4_u(M.Get(3,3));
}

void Taffinetransformation::G_origin_vector(Tvector *origin) const
{
	origin->S_x(M.Get(0,3));
	origin->S_y(M.Get(1,3));
	origin->S_z(M.Get(2,3));
}


void Taffinetransformation::G_xaxis(Tvector *rt) const
{
	rt->S_x(M.Get(0,0));
	rt->S_y(M.Get(1,0));
	rt->S_z(M.Get(2,0));
}

void Taffinetransformation::G_yaxis(Tvector *rt) const
{
	rt->S_x(M.Get(0,1));
	rt->S_y(M.Get(1,1));
	rt->S_z(M.Get(2,1));
}

void Taffinetransformation::G_zaxis(Tvector *rt) const
{
	rt->S_x(M.Get(0,2));
	rt->S_y(M.Get(1,2));
	rt->S_z(M.Get(2,2));
}


void Taffinetransformation::multiply(const Taffinetransformation *tf1, const Taffinetransformation *tf2)
{
	M.multiply(&tf1->M,&tf2->M);
	Minv.multiply(&tf2->Minv,&tf1->Minv);
	isrescaling=tf1->isrescaling||tf2->isrescaling;
}

void Taffinetransformation::getangles(double &ax, double &ay, double &az) const
{
	ax=asin(-M.Get(2,1));
	ay=atan2(M.Get(2,0),M.Get(2,2));
	az=atan2(M.Get(0,1),M.Get(1,1));
	//Ax = Sin-1(-a32), Ay = Tan-1(a31/a33), and Az = Tan-1(a12/a22).
}

double reducang(double a)
{
	while (a<-Pi) a+=2*Pi;
	while (a>Pi) a-=2*Pi;
	return a;
}

void Taffinetransformation::weight(const Taffinetransformation *tf1, const Taffinetransformation *tf2, double fr)
{
	double ax1,ay1,az1,ax2,ay2,az2,ax,ay,az;

	double w1=1-fr;
	double w2=fr;

	tf1->getangles(ax1,ay1,az1);
	tf2->getangles(ax2,ay2,az2);

	double axdff=reducang(ax2-ax1);
	double aydff=reducang(ay2-ay1);
	double azdff=reducang(az2-az1);

	ax=ax1+fr*axdff;
	ay=ay1+fr*aydff;
	az=az1+fr*azdff;

	Taffinetransformation tfz;tfz.makerotation_Z(az);
	Taffinetransformation tfx;tfx.makerotation_X(ax);
	Taffinetransformation tfy;tfy.makerotation_Y(ay);

	Taffinetransformation tmp;
	tmp.multiply(&tfz,&tfx);
	multiply(&tmp,&tfy);

	M.Set(0,3, w1*tf1->M.Get(0,3) + w2*tf2->M.Get(0,3) );
	M.Set(1,3, w1*tf1->M.Get(1,3) + w2*tf2->M.Get(1,3) );
	M.Set(2,3, w1*tf1->M.Get(2,3) + w2*tf2->M.Get(2,3) );

	Minv.Set(0,3, w1*tf1->Minv.Get(0,3) + w2*tf2->Minv.Get(0,3) );
	Minv.Set(1,3, w1*tf1->Minv.Get(1,3) + w2*tf2->Minv.Get(1,3) );
	Minv.Set(2,3, w1*tf1->Minv.Get(2,3) + w2*tf2->Minv.Get(2,3) );

	int ii=0;
}


void Taffinetransformation::mul(const Thomogcoord *arg, Thomogcoord *result) const
{
	M.mul(arg,result);
}

void Taffinetransformation::mulinv(const Thomogcoord *arg, Thomogcoord *result) const
{
	Minv.mul(arg,result);
}

void Taffinetransformation::mulnormvec(const Tvector *arg, Tvector *result) const
{
	Minv.multransp(arg,result);
}


void Taffinetransformation::tostring(QString &str, int tpe)
{
	M.tostring(str,tpe);
}

void Taffinetransformation::fromstring(StrPtr icontent)
{
	M.fromstring(icontent);
	Minv.calcinverse(&M);
}

void Taffinetransformation::streamout(double *vals)
{
	for (int i1=0; i1<4; i1++)
		for (int i2=0; i2<4; i2++)
			vals[4*i1+i2]=M.Get(i2,i1);
}

void Taffinetransformation::streamout(QBinTagWriter &writer)
{
	M.streamout(writer);
	Minv.streamout(writer);
}
void Taffinetransformation::streamin(QBinTagReader &reader)
{
	M.streamin(reader);
	Minv.streamin(reader);
}


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(transf_create,dmmy)
{
	setname(SC_valname_affinetransformation);
	setclasspath_fromtype(SC_valname_affinetransformation);
	setreturntype(SC_valname_affinetransformation);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Taffinetransformation *m=G_valuecontent<Taffinetransformation>(retval);
	m->reset();
}
ENDFUNCTION(transf_create)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(transf_reset,reset)
{
	setmemberfunction(SC_valname_affinetransformation);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Taffinetransformation *m=G_valuecontent<Taffinetransformation>(owner);
	m->reset();
}
ENDFUNCTION(transf_reset)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(transf_origin,origin)
{
	setcanassign();
	setmemberfunction(SC_valname_affinetransformation);
	setreturntype(SC_valname_vertex);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Taffinetransformation *m=G_valuecontent<Taffinetransformation>(owner);
	if (assigntoval!=NULL)
	{
		Tvertex *v=G_valuecontent<Tvertex>(assigntoval);
		m->Set_origin(v);
	}
	Tvertex *rt=G_valuecontent<Tvertex>(retval);
	m->G_origin(rt);
}
ENDFUNCTION(transf_origin)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(transf_xaxis,Xaxis)
{
	setcanassign();
	setmemberfunction(SC_valname_affinetransformation);
	setreturntype(SC_valname_vector);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Taffinetransformation *m=G_valuecontent<Taffinetransformation>(owner);
	if (assigntoval!=NULL)
	{
		Tvector *v=G_valuecontent<Tvector>(assigntoval);
		m->Set_xaxis(v);
	}
	Tvector *rt=G_valuecontent<Tvector>(retval);
	m->G_xaxis(rt);
}
ENDFUNCTION(transf_xaxis)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(transf_yaxis,Yaxis)
{
	setcanassign();
	setmemberfunction(SC_valname_affinetransformation);
	setreturntype(SC_valname_vector);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Taffinetransformation *m=G_valuecontent<Taffinetransformation>(owner);
	if (assigntoval!=NULL)
	{
		Tvector *v=G_valuecontent<Tvector>(assigntoval);
		m->Set_yaxis(v);
	}
	Tvector *rt=G_valuecontent<Tvector>(retval);
	m->G_yaxis(rt);
}
ENDFUNCTION(transf_yaxis)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(transf_zaxis,Zaxis)
{
	setcanassign();
	setmemberfunction(SC_valname_affinetransformation);
	setreturntype(SC_valname_vector);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Taffinetransformation *m=G_valuecontent<Taffinetransformation>(owner);
	if (assigntoval!=NULL)
	{
		Tvector *v=G_valuecontent<Tvector>(assigntoval);
		m->Set_zaxis(v);
	}
	Tvector *rt=G_valuecontent<Tvector>(retval);
	m->G_zaxis(rt);
}
ENDFUNCTION(transf_zaxis)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(transf_translate,translate)
{
	setmemberfunction(SC_valname_affinetransformation);
	setreturntype(SC_valname_affinetransformation);
	addvar(_qstr("dir"),SC_valname_vector);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Taffinetransformation *m=G_valuecontent<Taffinetransformation>(owner);
	Taffinetransformation tf;tf=*m;
	m->translatefrom(&tf,G_valuecontent<Tvector>(arglist->get(0)));
	retval->encapsulate(m);
}
ENDFUNCTION(transf_translate)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(transf_rotate,rotate)
{
	setmemberfunction(SC_valname_affinetransformation);
	setreturntype(SC_valname_affinetransformation);
	addvar(_qstr("rotdir"),SC_valname_vector);
	addvar(_qstr("angle"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Taffinetransformation *m=G_valuecontent<Taffinetransformation>(owner);
	Taffinetransformation tf;tf=*m;
	m->rotatefrom(&tf,G_valuecontent<Tvector>(arglist->get(0)),arglist->get(1)->G_content_scalar()->G_val());
	retval->encapsulate(m);
}
ENDFUNCTION(transf_rotate)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(transf_scale,scale)
{
	setmemberfunction(SC_valname_affinetransformation);
	setreturntype(SC_valname_affinetransformation);
	addvar(_qstr("Factor"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Taffinetransformation *m=G_valuecontent<Taffinetransformation>(owner);
	m->scale(arglist->get(0)->G_content_scalar()->G_val());
	retval->encapsulate(m);
}
ENDFUNCTION(transf_scale)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(transf_invert,invert)
{
	setmemberfunction(SC_valname_affinetransformation);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Taffinetransformation *m=G_valuecontent<Taffinetransformation>(owner);
	m->invert();
}
ENDFUNCTION(transf_invert)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(transf_weight,weight)
{
	setreturntype(SC_valname_affinetransformation);
	setclasspath_fromtype(SC_valname_affinetransformation);
//	setmemberfunction(SC_valname_affinetransformation);
	addvar(_qstr("Transf1"),SC_valname_affinetransformation);
	addvar(_qstr("Transf2"),SC_valname_affinetransformation);
	addvar(_qstr("Frac2"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Taffinetransformation *m=G_valuecontent<Taffinetransformation>(retval);
	m->weight(
		G_valuecontent<Taffinetransformation>(arglist->get(0)),
		G_valuecontent<Taffinetransformation>(arglist->get(1)),
		arglist->get(2)->G_content_scalar()->G_val()
		);
}
ENDFUNCTION(transf_weight)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_mul_transfpoint,10,SC_valname_affinetransformation,SC_valname_vertex,SC_valname_vertex)
{
	setclasspath_fromtype(SC_valname_affinetransformation);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	Taffinetransformation *t=G_valuecontent<Taffinetransformation>(val1);
	Tvertex *v=G_valuecontent<Tvertex>(val2);
	t->mul(v,G_valuecontent<Tvertex>(retval));
}
ENDOPERATOR(op_mul_transfpoint)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_mul_transfvector,10,SC_valname_affinetransformation,SC_valname_vector,SC_valname_vector)
{
	setclasspath_fromtype(SC_valname_affinetransformation);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	Taffinetransformation *t=G_valuecontent<Taffinetransformation>(val1);
	Tvector *v=G_valuecontent<Tvector>(val2);
	t->mul(v,G_valuecontent<Tvector>(retval));
}
ENDOPERATOR(op_mul_transfvector)

///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_mul_transf,10,SC_valname_affinetransformation,SC_valname_affinetransformation,SC_valname_affinetransformation)
{
	setclasspath_fromtype(SC_valname_affinetransformation);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	G_valuecontent<Taffinetransformation>(retval)->multiply(
		G_valuecontent<Taffinetransformation>(val1),G_valuecontent<Taffinetransformation>(val2));
}
ENDOPERATOR(op_mul_transf)





///////////////////////////////////////////////////////////////////////////////////
TYPECAST(tc_matrix_transf,SC_valname_matrix,SC_valname_affinetransformation)
{
}
void execute_implement(TSC_value *argval, TSC_value *retval)
{
	G_valuecontent<Taffinetransformation>(retval)->copyfrom(G_valuecontent<TMatrix>(argval));
}
ENDTYPECAST(tc_matrix_transf)

///////////////////////////////////////////////////////////////////////////////////
TYPECAST(tc_transf_matrix,SC_valname_affinetransformation,SC_valname_matrix)
{
}
void execute_implement(TSC_value *argval, TSC_value *retval)
{
	G_valuecontent<Taffinetransformation>(argval)->copyto(G_valuecontent<TMatrix>(retval));
}
ENDTYPECAST(tc_transf_matrix)
