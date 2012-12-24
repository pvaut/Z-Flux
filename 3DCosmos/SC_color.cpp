#include "stdafx.h"
#include "SC_value.h"
#include "SC_color.h"
#include "SC_env.h"

void TSC_color::tostring(QString &str, int tpe)
{
	QString st1;
	str.clear();
	st1.formatscalar(G_R());str+=st1;str+=_qstr(", ");
	st1.formatscalar(G_G());str+=st1;str+=_qstr(", ");
	st1.formatscalar(G_B());str+=st1;str+=_qstr(", ");
	st1.formatscalar(G_A());str+=st1;//str+=_qstr(")");
}

void TSC_color::fromstring(StrPtr icontent)
{
	QString st=icontent,st1;
	st.splitstring(_qstr(","),st1);R=qstr2double(st1);
	st.splitstring(_qstr(","),st1);G=qstr2double(st1);
	st.splitstring(_qstr(","),st1);B=qstr2double(st1);
	st.splitstring(_qstr(","),st1);A=qstr2double(st1);
}

void TSC_color::mulwith(const TSC_color *col)
{
	R=R*col->R;
	G=G*col->G;
	B=B*col->B;
	A=A*col->A;
	F=F*col->F;
}


void TSC_color::addfrom(TSC_color *col1, TSC_color *col2)
{
	R=(col1->R+col2->R);
	G=(col1->G+col2->G);
	B=(col1->B+col2->B);
	A=(col1->A+col2->A);
	F=(col1->F+col2->F);
}

void TSC_color::mulfrom(double vl, TSC_color *col)
{
	R=vl*col->R;
	G=vl*col->G;
	B=vl*col->B;
	A=vl*col->A;
	F=vl*col->F;
}

void TSC_color::streamout(QBinTagWriter &writer)
{
	writer.write_int32((int)(R*100000.0));
	writer.write_int32((int)(G*100000.0));
	writer.write_int32((int)(B*100000.0));
	writer.write_int32((int)(A*100000.0));
}

void TSC_color::streamin(QBinTagReader &reader)
{
	R=reader.read_int32()/100000.0;
	G=reader.read_int32()/100000.0;
	B=reader.read_int32()/100000.0;
	A=reader.read_int32()/100000.0;
	F=1;
}





///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_color,color)
{
	setclasspath_fromtype(SC_valname_color);
	setreturntype(SC_valname_color);
	addvar(_qstr("r"),SC_valname_scalar);
	addvar(_qstr("g"),SC_valname_scalar);
	addvar(_qstr("b"),SC_valname_scalar);
	addvar(_qstr("a"),SC_valname_scalar,false);
}

void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_color col;
	double A=1;
	if (arglist->G_ispresent(3)) A=arglist->get(3)->G_content_scalar()->G_val();
	col.copyfrom(arglist->get(0)->G_content_scalar()->G_val(),
	             arglist->get(1)->G_content_scalar()->G_val(),
	             arglist->get(2)->G_content_scalar()->G_val(),
	             A
				 );

	*G_valuecontent<TSC_color>(retval)=col;
}

ENDFUNCTION(func_color)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_colorr,r)
{
	setcanassign();
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_color);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_color *cl=G_valuecontent<TSC_color>(owner);
	if (assigntoval!=NULL) cl->Set_R(assigntoval->G_content_scalar()->G_val());
	retval->G_content_scalar()->copyfrom(cl->G_R());
}
ENDFUNCTION(func_colorr)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_colorg,g)
{
	setcanassign();
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_color);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_color *cl=G_valuecontent<TSC_color>(owner);
	if (assigntoval!=NULL) cl->Set_G(assigntoval->G_content_scalar()->G_val());
	retval->G_content_scalar()->copyfrom(cl->G_G());
}
ENDFUNCTION(func_colorg)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_colorb,b)
{
	setcanassign();
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_color);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_color *cl=G_valuecontent<TSC_color>(owner);
	if (assigntoval!=NULL) cl->Set_B(assigntoval->G_content_scalar()->G_val());
	retval->G_content_scalar()->copyfrom(cl->G_B());
}
ENDFUNCTION(func_colorb)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_colora,a)
{
	setcanassign();
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_color);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_color *cl=G_valuecontent<TSC_color>(owner);
	if (assigntoval!=NULL) cl->Set_A(assigntoval->G_content_scalar()->G_val());
	retval->G_content_scalar()->copyfrom(cl->G_A());
}
ENDFUNCTION(func_colora)



///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_color_add,8,SC_valname_color,SC_valname_color,SC_valname_color)
{
	setclasspath_fromtype(SC_valname_color);
	Set_description(_text("Returns the (weighted) average of two colors"));
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	TSC_color *col1=G_valuecontent<TSC_color>(val1);
	TSC_color *col2=G_valuecontent<TSC_color>(val2);
	G_valuecontent<TSC_color>(retval)->addfrom(col1,col2);
}
ENDOPERATOR(op_color_add)

///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_color_mul,10,SC_valname_scalar,SC_valname_color,SC_valname_color)
{
	setclasspath_fromtype(SC_valname_color);
	Set_description(_text("Multiplies the weight of a color (used in combination with addition of two colors)"));
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	double vl=val1->G_content_scalar()->G_val();
	TSC_color *col=G_valuecontent<TSC_color>(val2);
	G_valuecontent<TSC_color>(retval)->mulfrom(vl,col);
}
ENDOPERATOR(op_color_mul)
