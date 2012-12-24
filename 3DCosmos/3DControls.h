class T3DControl :  public T3DObject
{
private:
	TSC_scalar *sizeunit;
	Tvertex *position;
	TSC_boolean *wasmodified,*isdefault;
	TSC_color *color;
protected:
	TSC_color rendercolor;


public:
	T3DControl(T3DScene *iscene, bool addtocontrols);
	~T3DControl();
public:
	virtual void precalcrender(Tprecalcrendercontext *pre);
	virtual void render(Trendercontext *rc, const TObjectRenderStatus *status, bool isrecording);

	double G_sizeunit() { return sizeunit->G_val(); }
	double G_controlposx() { return position->G3_x(); }
	double G_controlposy() { return position->G3_y(); }
	double G_textsize();
	double G_offset();//small offset used to render in front
	bool G_isactive();//returns true if this control is currenty highlighted

	void RenderGenericBackGround(Trendercontext *rc);
	void render_rect(Trendercontext *rc, double x0, double y0, double lx, double ly, double z0);
	void render_rect2(Trendercontext *rc, double x0, double y0, double lx, double ly, double z0, TQXColor &cl1, TQXColor &cl2);
	void render_frame(Trendercontext *rc, double x0, double y0, double lx, double ly, double z0);
	void Set_wasmodified() { wasmodified->copyfrom(true); }
	void Unset_hasmodified() { wasmodified->copyfrom(false); }

	bool IsDefault() { return isdefault->G_val(); }

public:

	virtual bool G_canselect() { return false; }
	virtual double G_controlsizex()=0;
	virtual double G_controlsizey()=0;
	virtual void render_impl(Trendercontext *rc, const TObjectRenderStatus *status) {};

public:
	virtual bool UI_OnKeyDown(UINT ch, bool shiftpressed, bool controlpressed) { return false; }
	virtual bool UI_OnChar(UINT ch, bool shiftpressed, bool controlpressed) { return false; }
	virtual bool UI_OnPressed_Up() { return false; }
	virtual bool UI_OnPressed_Down() { return false; }

	virtual void MouseArrowClick(TMouseClickInfo info);
	virtual void MouseArrowClickImpl(TMouseClickInfo info) {}

};


class T3DControl_Frame : public T3DControl
{
private:
	TSC_scalar *sizex,*sizey;
public:
	static StrPtr GetClassName() { return SC_valname_control_frame; }
	virtual StrPtr G_classname() { return SC_valname_control_frame; }
	T3DControl_Frame(T3DScene *iscene=NULL, bool addtocontrols=true);
	~T3DControl_Frame();
public:
	virtual T3DObject* CreateInstance() { return new T3DControl_Frame(NULL); }
	virtual bool G_canselect() { return false; }
	virtual double G_controlsizex();
	virtual double G_controlsizey();
	virtual void render_impl(Trendercontext *rc, const TObjectRenderStatus *status);
};


class T3DControl_Text : public T3DControl
{
private:
	TSC_string *content;
public:
	static StrPtr GetClassName() { return SC_valname_control_text; }
	virtual StrPtr G_classname() { return SC_valname_control_text; }
	T3DControl_Text(T3DScene *iscene=NULL, bool addtocontrols=true);
	~T3DControl_Text();
public:
	virtual T3DObject* CreateInstance() { return new T3DControl_Text(NULL); }
	virtual bool G_canselect() { return false; }
	virtual double G_controlsizex();
	virtual double G_controlsizey();
	virtual void render_impl(Trendercontext *rc, const TObjectRenderStatus *status);
};


class T3DControl_Scalar : public T3DControl
{
private:
	TSC_scalar *sizex,*rangesizex,*decimalcount;
	TSC_scalar *value,*vmin,*vmax,*vstep;
private:
	void checkrange();
	void addstep(double stepcount);
public:
	static StrPtr GetClassName() { return SC_valname_control_scalar; }
	virtual StrPtr G_classname() { return SC_valname_control_scalar; }
	T3DControl_Scalar(T3DScene *iscene=NULL, bool addtocontrols=true);
	~T3DControl_Scalar();
public:
	virtual T3DObject* CreateInstance() { return new T3DControl_Scalar(NULL); }
	virtual bool G_canselect() { return true; }
	virtual double G_controlsizex();
	virtual double G_controlsizey();
	virtual void render_impl(Trendercontext *rc, const TObjectRenderStatus *status);
	bool UI_OnPressed_Up();
	bool UI_OnPressed_Down();
	bool UI_OnKeyDown(UINT ch, bool shiftpressed, bool controlpressed);
	bool UI_OnChar(UINT ch, bool shiftpressed, bool controlpressed);
	virtual void paramchanged(StrPtr iname);
};


class T3DControl_Edit : public T3DControl
{
private:
	TSC_scalar *sizex;
	TSC_string *content;
public:
	static StrPtr GetClassName() { return SC_valname_control_edit; }
	virtual StrPtr G_classname() { return SC_valname_control_edit; }
	T3DControl_Edit(T3DScene *iscene=NULL, bool addtocontrols=true);
	~T3DControl_Edit();
public:
	virtual T3DObject* CreateInstance() { return new T3DControl_Edit(NULL); }
	virtual bool G_canselect() { return true; }
	virtual double G_controlsizex();
	virtual double G_controlsizey();
	virtual void render_impl(Trendercontext *rc, const TObjectRenderStatus *status);
	bool UI_OnKeyDown(UINT ch, bool shiftpressed, bool controlpressed);
	bool UI_OnChar(UINT ch, bool shiftpressed, bool controlpressed);
	virtual void paramchanged(StrPtr iname);
};

class T3DControl_Check : public T3DControl
{
private:
	TSC_boolean *checked;
public:
	static StrPtr GetClassName() { return SC_valname_control_check; }
	virtual StrPtr G_classname() { return SC_valname_control_check; }
	T3DControl_Check(T3DScene *iscene=NULL, bool addtocontrols=true);
	~T3DControl_Check();
public:
	virtual T3DObject* CreateInstance() { return new T3DControl_Check(NULL); }
	virtual bool G_canselect() { return true; }
	virtual double G_controlsizex();
	virtual double G_controlsizey();
	virtual void render_impl(Trendercontext *rc, const TObjectRenderStatus *status);
	bool UI_OnKeyDown(UINT ch, bool shiftpressed, bool controlpressed);
	bool UI_OnChar(UINT ch, bool shiftpressed, bool controlpressed);
	virtual void MouseArrowClickImpl(TMouseClickInfo info);
};


class T3DControl_Button : public T3DControl
{
private:
	TSC_scalar *sizex;
	TSC_string *content;
public:
	static StrPtr GetClassName() { return SC_valname_control_button; }
	virtual StrPtr G_classname() { return SC_valname_control_button; }
	T3DControl_Button(T3DScene *iscene=NULL, bool addtocontrols=true);
	~T3DControl_Button();
public:
	virtual T3DObject* CreateInstance() { return new T3DControl_Button(NULL); }
	virtual bool G_canselect() { return true; }
	virtual double G_controlsizex();
	virtual double G_controlsizey();
	virtual void render_impl(Trendercontext *rc, const TObjectRenderStatus *status);
	bool UI_OnKeyDown(UINT ch, bool shiftpressed, bool controlpressed);
	virtual void MouseArrowClickImpl(TMouseClickInfo info);
};



class T3DControl_List : public T3DControl
{
private:
	TSC_scalar *sizex,*county,*selectidx;
	TSC_list *list;
	int offset;
public:
	static StrPtr GetClassName() { return SC_valname_control_list; }
	virtual StrPtr G_classname() { return SC_valname_control_list; }

	T3DControl_List(T3DScene *iscene=NULL, bool addtocontrols=true);
	~T3DControl_List();

	TSC_value* G_selecteditem();

public:
	virtual T3DObject* CreateInstance() { return new T3DControl_List(NULL); }

	virtual bool G_canselect() { return true; }
	virtual double G_controlsizex();
	virtual double G_controlsizey();
	virtual void render_impl(Trendercontext *rc, const TObjectRenderStatus *status);

	virtual bool UI_OnPressed_Up();
	virtual bool UI_OnPressed_Down();
	bool UI_OnChar(UINT ch, bool shiftpressed, bool controlpressed);

	virtual void MouseArrowClickImpl(TMouseClickInfo info);

	virtual void paramchanged(StrPtr iname);
};


class T3DControl_Menu_Item
{
public:
	T3DControl_Menu_Item *parent;
	QString ID,name;
	Tarray<T3DControl_Menu_Item> subitems;
	int cursubsel;
	bool cancheck,checked;
	void additem(StrPtr iID, StrPtr iname, bool cancheck);
	T3DControl_Menu_Item *finditem(StrPtr iID);
public:
	T3DControl_Menu_Item(T3DControl_Menu_Item *iparent)
	{
		parent=iparent;
		cursubsel=-1;
		cancheck=false;
		checked=false;
	}
	bool IsSeparator()
	{
		return issame(ID,_qstr("-"));
	}
};

class T3DControl_Menu : public T3DControl
{
private:
	TSC_scalar *sizex;
	TSC_string *selectid;
public:
	T3DControl_Menu_Item root;
public:
	static StrPtr GetClassName() { return SC_valname_control_menu; }
	virtual StrPtr G_classname() { return SC_valname_control_menu; }

	T3DControl_Menu(T3DScene *iscene=NULL, bool addtocontrols=true);
	~T3DControl_Menu();

	void additem(StrPtr iID, StrPtr iname, StrPtr parentid, bool cancheck);

	bool SelectItem(T3DControl_Menu_Item *current, StrPtr id);

//	TSC_value* G_selecteditem();

public:
	virtual T3DObject* CreateInstance() { return new T3DControl_Menu(NULL); }

	virtual bool G_canselect() { return true; }
	virtual double G_controlsizex() { return 0; }
	virtual double G_controlsizey() { return 0; }
	StrPtr G_texturename() { return G_paramstring(_qstr("Texture")); }

	void setselectid();

	void rendermenu(T3DControl_Menu_Item *parentitem, Trendercontext *rc, double x0, double y0);
	virtual void render(Trendercontext *rc, const TObjectRenderStatus *status, bool isrecording);

	T3DControl_Menu_Item* G_selitem();

	double GetItemH(T3DControl_Menu_Item &item);

	virtual bool UI_OnPressed_Up();
	virtual bool UI_OnPressed_Down();
	bool UI_OnKeyDown(UINT ch, bool shiftpressed, bool controlpressed);
	bool UI_OnChar(UINT ch, bool shiftpressed, bool controlpressed);

	virtual void paramchanged(StrPtr iname);

	void TestClickMenu(T3DControl_Menu_Item *parentitem,double x0, double y0, double clickx, double clicky);

	virtual void MouseArrowClickImpl(TMouseClickInfo info);

};