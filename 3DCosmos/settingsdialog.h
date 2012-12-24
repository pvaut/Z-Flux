
class Tsettingsdialog : public CDialog
{
private:
	Tmainwin *parent;
public:
	Tsettingsdialog(Tmainwin *iparent);
	~Tsettingsdialog();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
public:
//	DECLARE_MESSAGE_MAP();
};
