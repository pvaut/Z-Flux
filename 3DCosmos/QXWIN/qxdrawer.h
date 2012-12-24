#pragma once

class TQXdrawer
{
public:
	virtual ~TQXdrawer();
	virtual int G_resx() const=0;
	virtual int G_resy() const=0;
	virtual void draw(CDC *dc, int x0, int y0)=0;
public:
	void drawcent(CDC *dc, int x0, int y0)
	{
		draw(dc,x0-G_resx()/2,y0-G_resy()/2);
	}
};
