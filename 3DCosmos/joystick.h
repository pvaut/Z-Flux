
#define JOYSTICKBUTTONCOUNT 32

class Tjoystickinfo {
private:
	int lastrockerpos,rockerpos;//-1= neutral 0=top 90=right 180=down 270=right
	bool buttondown[JOYSTICKBUTTONCOUNT],prevbuttondown[JOYSTICKBUTTONCOUNT],buttondoubleclick[JOYSTICKBUTTONCOUNT],buttonshortclick[JOYSTICKBUTTONCOUNT],buttonlongclick[JOYSTICKBUTTONCOUNT];
	long lastbuttondowntick[JOYSTICKBUTTONCOUNT],lastbuttonuptick[JOYSTICKBUTTONCOUNT];
public:
	long lastrockertesttick,lastrockerchanged;
	double xp,yp,zp,rp,up,vp;
	bool read(int nr);
public:
	Tjoystickinfo();
	double G_xpcorr();
	double G_ypcorr();
	double G_zpcorr();
	double G_rpcorr();
	double G_upcorr();
	double G_vpcorr();
	double G_axis(int axisnr, bool corrected);
	bool G_buttondown(int nr);
	bool G_buttonclicked(int nr);
	bool G_buttondoubleclicked(int nr);
	bool G_buttonshortclicked(int nr);
	bool G_buttonlongclicked(int nr);
	int G_rockerpos() { return rockerpos; }
	bool G_rockerposchanged() { return rockerpos!=lastrockerpos; }

	int G_rockerclicked();//0=none 1=up 2=right 3=down 4=left

public:
	int G_button_shift() { return 8-1; }
	int G_button_control() { return 6-1; }
	int G_button_return() { return 5-1; }
	int G_button_esc() { return 7-1; }
	int G_button_F1() { return 1-1; }
	int G_button_F2() { return 2-1; }
	int G_button_F3() { return 3-1; }
	int G_button_F4() { return 4-1; }
//	int G_button_timespeed() { return 8-1; }
};