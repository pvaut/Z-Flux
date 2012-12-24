
#include "tools.h"

class TQXwinheadercompdef
{
public:
	QString name;
	PanelSideDef side;
};

class TQXwinpaneldef
{
	friend class TQXpanel;
public:
	enum SubTypeDef { SUB_HOR, SUB_VERT, SUB_TAB };
private:
	Tarray<TQXwinpaneldef> subpanels;
	Tarray<TQXwinheadercompdef> headercomponents;
	SubTypeDef subtype;
	QString name;
	double sizefrac;
public:
	bool hastitlebar,hascustomtabs;
public:
	TQXwinpaneldef()
	{
		subtype=SUB_HOR;
		hastitlebar=false;
		hascustomtabs=false;
	}
	void Set_name(StrPtr iname)
	{
		name=iname;
	}
	TQXwinpaneldef* addpanel(StrPtr iname, double isizefrac=1, SubTypeDef isubtype=SUB_HOR)
	{
		TQXwinpaneldef *sub=new TQXwinpaneldef;
		subpanels.add(sub);
		sub->name=iname;
		sub->sizefrac=isizefrac;
		sub->subtype=isubtype;
		return sub;
	}
	TQXwinheadercompdef* addheadercomponent(StrPtr iname, PanelSideDef iside)
	{
		TQXwinheadercompdef *headercompdef=new TQXwinheadercompdef;
		headercompdef->name=iname;
		headercompdef->side=iside;
		headercomponents.add(headercompdef);
		return headercompdef;
	}
};

class TQXwindef
{
public:
	TQXwinpaneldef rootpanel;
public:
};
