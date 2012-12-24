#include "stdafx.h"
#include "qxwin.h"



//**************************************************************
// TQXbitmapdrawer
//**************************************************************

TQXbitmapdrawer::TQXbitmapdrawer()
{
	ID=0;
	resx=0;resy=0;
}

TQXbitmapdrawer::TQXbitmapdrawer(int iID)
{
	init(iID);
}

void TQXbitmapdrawer::init(int iID)
{
	tagBITMAP bim;
	ID=iID;
	bitmap.LoadBitmap(ID);
	bitmap.GetBitmap(&bim);
	resx=bim.bmWidth;
	resy=bim.bmHeight;
}


int TQXbitmapdrawer::G_resx() const
{
	return resx;
}

int TQXbitmapdrawer::G_resy() const
{
	return resy;
}

void TQXbitmapdrawer::draw(CDC *dc, int x0, int y0)
{
	CDC memDC;
	memDC.CreateCompatibleDC(dc);
	memDC.SelectObject(&bitmap);
	dc->BitBlt(x0,y0,resx,resy,&memDC,0,0,SRCCOPY);
}





//**************************************************************
// TQXmembitmap
//**************************************************************


TQXmembitmap::TQXmembitmap()
{
	resx=0;resy=0;
	bitmap=NULL;
	memdc=NULL;
}

TQXmembitmap::~TQXmembitmap()
{
	ASSERT(memdc==NULL);
	if (bitmap!=NULL) delete bitmap;
}

CDC* TQXmembitmap::start(CDC *dc, int iresx, int iresy)
{
	ASSERT(memdc==NULL);
	if (memdc!=NULL) return memdc;
	memdc=new CDC;
	memdc->CreateCompatibleDC(dc);
	if ((bitmap==NULL)||(iresx>resx)||(iresy>resy))
	{
		if (bitmap!=NULL) delete bitmap;
		bitmap=new CBitmap;
		resx=iresx;resy=iresy;
		bitmap->CreateCompatibleBitmap(dc,resx,resy);
	}
	memdc->SelectObject(bitmap);
	return memdc;
}


void TQXmembitmap::stop()
{
	ASSERT(memdc!=NULL);
	if (memdc==NULL) return;
	delete memdc;
	memdc=NULL;
}

void TQXmembitmap::transfer(CDC *dc, int x0, int y0)
{
	ASSERT(memdc!=NULL);
	if (memdc==NULL) return;
	dc->BitBlt(x0,y0,resx,resy,memdc,0,0,SRCCOPY);
}



//**************************************************************
// TQXgraycorrmap
//**************************************************************


TQXgraycorrmap::TQXgraycorrmap()
{
	dat=NULL;
	xres=0;yres=0;
}

TQXgraycorrmap::~TQXgraycorrmap()
{
	if (dat!=NULL) free(dat);
}


void TQXgraycorrmap::init(int ixres, int iyres)
{
	xres=ixres;
	yres=iyres;
	dat=(signed char*)realloc(dat,xres*yres);
	for (int ix=0; ix<xres; ix++)
		for (int iy=0; iy<yres; iy++)
			dat[iy*xres+ix]=0;
}

void TQXgraycorrmap::vertgrad(double val1, double val2, int tpe)
{
	double fr;
	if ((yres<=0)||(dat==NULL))
	{
		ASSERT(false);
		return;
	}
	for (int iy=0; iy<yres; iy++)
	{
		fr=iy*1.0/yres;
		signed char vl=(signed char)(val1*255+(val2-val1)*255*fr);
		for (int ix=0; ix<xres; ix++)
			dat[iy*xres+ix]=vl;
	}
}



void TQXgraycorrmap::add(TQXgraycorrmap &map, int x0, int y0)
{
	int ix0,iy0;
	for (int ix=0; ix<map.xres; ix++)
		for (int iy=0; iy<map.yres; iy++)
		{
			ix0=x0+ix;iy0=y0+iy;
			if ((ix0>=0)&&(ix0<xres)&&(iy0>=0)&&(iy0<yres))
				dat[iy0*xres+ix0]+=map.dat[iy*map.xres+ix];
		}
}

void TQXgraycorrmap::copyfrom(TQXDIBitmap &map)
{
	int rr,gg,bb;
	for (int ix=0; ix<xres; ix++)
		for (int iy=0; iy<yres; iy++)
		{
			map.G_color(ix,iy,rr,gg,bb);
			dat[iy*xres+ix]=(signed char)((rr+gg+bb)/3.0/2.0);
		}
}

void TQXgraycorrmap::clonehorline(int px, int lx, int py1, int py2)
{
	for (int iy=py1+1; iy<=py2; iy++)
		for (int ix=px; ix<px+lx; ix++)
			Set(ix,iy,Get(ix,py1));
}

void TQXgraycorrmap::clonevertline(int py, int ly, int px1, int px2)
{
	for (int ix=px1+1; ix<=px2; ix++)
		for (int iy=py; iy<py+ly; iy++)
			Set(ix,iy,Get(px1,iy));
}





//**************************************************************
// TQXbumpmap
//**************************************************************


TQXbumpmap::TQXbumpmap()
{
	dat=NULL;
	xres=0;yres=0;

	ldx=1;
	ldy=+1;
	ldz=1;
	double rr=sqrt((double)(ldx*ldx+ldy*ldy+ldz*ldz));
	ldx/=rr;ldy/=rr;ldz/=rr;

	sdx=(0+ldx)/2;
	sdy=(0+ldy)/2;
	sdz=(-1+ldz)/2;
	rr=sqrt((double)(sdx*sdx+sdy*sdy+sdz*sdz));
	sdx/=rr;sdy/=rr;sdz/=rr;

}

TQXbumpmap::~TQXbumpmap()
{
	if (dat!=NULL) free(dat);
}


void TQXbumpmap::init(int ixres, int iyres)
{
	xres=ixres;
	yres=iyres;
	dat=(double*)realloc(dat,xres*yres*sizeof(double));
	for (int ix=0; ix<xres; ix++)
		for (int iy=0; iy<yres; iy++)
			dat[iy*xres+ix]=0;
}

double TQXbumpmap::calc(double dx, double dy, double dz)
{
	double vl;
	vl=dx*ldx+dy*ldy+dz*ldz;
	vl-=0.6;
	vl=atan(5*vl);

	double svl=dx*sdx+dy*sdy+dz*sdz;
	svl=svl*4;
	svl=1/(1.0+svl*svl*svl*svl);

	vl+=svl;

	return vl;
}

void TQXbumpmap::render(TQXgraycorrmap &map)
{
	double rx,ry,rz,rr,vl;

	double vl0=calc(0,0,1);//shadow factor for horizontal surface


	if (dat==NULL) return;
	map.init(xres,yres);
	for (int ix=0; ix<xres; ix++)
		for (int iy=0; iy<yres; iy++)
		{
			rx=(Get(ix+1,iy)-Get(ix-1,iy))/2;
			ry=(Get(ix,iy+1)-Get(ix,iy-1))/2;
			rz=1;
			rr=sqrt(rx*rx+ry*ry+rz*rz);
			rx/=rr;ry/=rr;rz/=rr;
			vl=calc(rx,ry,rz);
			vl-=vl0;
			map.Set(ix,iy,(signed char)(0.5+vl*20));
		}
}



//**************************************************************
// TQXDIBitmap
//**************************************************************


TQXDIBitmap::TQXDIBitmap()
{
	present=0;
	dat=NULL;
	xres=0;yres=0;
}

TQXDIBitmap::~TQXDIBitmap()
{
	if (dat!=NULL) free(dat);
}

void TQXDIBitmap::init(int ixres, int iyres, TQXColor &col)
{
	int i,j;

	unsigned char colr=(int)(col.G_r()*255);
	unsigned char colg=(int)(col.G_g()*255);
	unsigned char colb=(int)(col.G_b()*255);

	xres=ixres;
	yres=iyres;
	present=1;
	xs=((3*xres+3)/4)*4;
	dat=(unsigned char*)realloc(dat,xs*yres);
	for (i=0; i<xres; i++)
		for (j=0; j<yres; j++)
		{
			dat[j*xs+i*3+0]=colr;
			dat[j*xs+i*3+1]=colg;
			dat[j*xs+i*3+2]=colb;
		}
	bminfo.bmiHeader.biSize=sizeof(bminfo.bmiHeader);
	bminfo.bmiHeader.biWidth=xres;
	bminfo.bmiHeader.biHeight=yres;
	bminfo.bmiHeader.biPlanes=1;
	bminfo.bmiHeader.biBitCount=24;
	bminfo.bmiHeader.biCompression=BI_RGB;
	bminfo.bmiHeader.biSizeImage=0;
	bminfo.bmiHeader.biXPelsPerMeter=10000;
	bminfo.bmiHeader.biYPelsPerMeter=10000;
	bminfo.bmiHeader.biClrUsed=0;
	bminfo.bmiHeader.biClrImportant=0;
}

void TQXDIBitmap::vertgrad(TQXColor &col1, TQXColor &col2, int type)
{
	for (int iy=0; iy<yres; iy++)
	{
		double fr=iy*1.0/yres;
		if (type==1) fr=Sshape(fr,0.6,0.4);
		TQXColor cl=(1-fr)*col1+fr*col2;
		unsigned char rr=(unsigned char)(255*cl.G_r());
		unsigned char gg=(unsigned char)(255*cl.G_g());
		unsigned char bb=(unsigned char)(255*cl.G_b());
		for (int ix=0; ix<xres; ix++)
			Set_color(ix,iy,rr,gg,bb);
	}
}
void TQXDIBitmap::horgrad(TQXColor &col1, TQXColor &col2, int type)
{
	for (int ix=0; ix<xres; ix++)
	{
		double fr=ix*1.0/xres;
		if (type==1) fr=Sshape(fr,0.6,0.4);
		TQXColor cl=(1-fr)*col1+fr*col2;
		unsigned char rr=(unsigned char)(255*cl.G_r());
		unsigned char gg=(unsigned char)(255*cl.G_g());
		unsigned char bb=(unsigned char)(255*cl.G_b());
		for (int iy=0; iy<yres; iy++)
			Set_color(ix,iy,rr,gg,bb);
	}
}


void TQXDIBitmap::copyfrom(HDC hdc, HBITMAP hbm)
{
	if (!present) return;
	GetDIBits(hdc,hbm,
		0,
		yres,
		dat,
		&bminfo,
		DIB_RGB_COLORS);
}

void TQXDIBitmap::G_color(int px, int py, unsigned char &rr, unsigned char &gg, unsigned char &bb)
{
	if ((!present)||(px<0)||(px>=xres)||(py<0)||(py>=yres)) return;
	py=yres-1-py;
	bb=dat[py*xs+px*3+0];
	gg=dat[py*xs+px*3+1];
	rr=dat[py*xs+px*3+2];
}

void TQXDIBitmap::G_color(int px, int py, int &rr, int &gg, int &bb)
{
	if ((!present)||(px<0)||(px>=xres)||(py<0)||(py>=yres)) return;
	py=yres-1-py;
	bb=dat[py*xs+px*3+0];
	gg=dat[py*xs+px*3+1];
	rr=dat[py*xs+px*3+2];
}


void TQXDIBitmap::Set_color(int px, int py, unsigned char rr, unsigned char gg, unsigned char bb)
{
	if ((!present)||(px<0)||(px>=xres)||(py<0)||(py>=yres)) return;
	py=yres-1-py;
	dat[py*xs+px*3+0]=bb;
	dat[py*xs+px*3+1]=gg;
	dat[py*xs+px*3+2]=rr;
}

void TQXDIBitmap::fliphor()
{
	unsigned char rr,gg,bb,rr0,gg0,bb0;
	for (int iy=0; iy<yres; iy++)
		for (int ix=0; ix<xres/2; ix++)
		{
			G_color(ix,iy,rr0,gg0,bb0);
			G_color(xres-1-ix,iy,rr,gg,bb);
			Set_color(ix,iy,rr,gg,bb);
			Set_color(xres-1-ix,iy,rr0,gg0,bb0);
		}
}

void TQXDIBitmap::flipvert()
{
	unsigned char rr,gg,bb,rr0,gg0,bb0;
	for (int iy=0; iy<yres/2; iy++)
		for (int ix=0; ix<xres; ix++)
		{
			G_color(ix,iy,rr0,gg0,bb0);
			G_color(ix,yres-1-iy,rr,gg,bb);
			Set_color(ix,iy,rr,gg,bb);
			Set_color(ix,yres-1-iy,rr0,gg0,bb0);
		}
}


void TQXDIBitmap::apply(TQXgraycorrmap &corrmap, int x0, int y0)
{
	int rr,gg,bb;
	signed char vl;
	if (!present) return;
	if ((xres<=0)||(yres<=0)) return;
	ASSERT(corrmap.dat!=NULL);
	for (int ix=0; ix<corrmap.xres; ix++)
		for (int iy=0; iy<corrmap.yres; iy++)
		{
			vl=corrmap.dat[(/*corrmap.yres-1-*/iy)*corrmap.xres+ix];
			G_color(x0+ix,y0+iy,rr,gg,bb);
			rr+=vl;if (rr<0) rr=0; if (rr>255) rr=255;
			gg+=vl;if (gg<0) gg=0; if (gg>255) gg=255;
			bb+=vl;if (bb<0) bb=0; if (bb>255) bb=255;
			Set_color(x0+ix,y0+iy,rr,gg,bb);
		}
}

void TQXDIBitmap::applycolormix(TQXgraycorrmap &transpmap, TQXColor &col, int x0, int y0)
{
	double fc;
	if (!present) return;
	ASSERT(transpmap.dat!=NULL);
	int ix0,iy0;
	for (int ix=0; ix<transpmap.xres; ix++)
		for (int iy=0; iy<transpmap.yres; iy++)
		{
			ix0=x0+ix;iy0=yres-1-(y0+iy);
			if ((ix0>=0)&&(ix0<xres)&&(iy0>=0)&&(iy0<yres))
			{
				fc=transpmap.dat[iy*transpmap.xres+ix]*col.G_a()/128.0;
				dat[iy0*xs+ix0*3+2]=(unsigned char)((fc*col.G_r()*255+(1-fc)*dat[iy0*xs+ix0*3+2]));
				dat[iy0*xs+ix0*3+1]=(unsigned char)((fc*col.G_g()*255+(1-fc)*dat[iy0*xs+ix0*3+1]));
				dat[iy0*xs+ix0*3+0]=(unsigned char)((fc*col.G_b()*255+(1-fc)*dat[iy0*xs+ix0*3+0]));
			}
		}
}

void TQXDIBitmap::replacecolor(TQXDIBitmap &bmp2, int bcolr, int bcolg, int bcolb)
{
	int rr,gg,bb,tcr,tcg,tcb;
	if (!present) return;
	if ((xres<=0)||(yres<=0)) return;
	ASSERT(bmp2.dat!=NULL);
	ASSERT(bmp2.xres==xres);
	ASSERT(bmp2.yres==yres);
	for (int ix=0; ix<xres; ix++)
		for (int iy=0; iy<yres; iy++)
		{
			G_color(ix,iy,rr,gg,bb);
			bmp2.G_color(ix,iy,tcr,tcg,tcb);
			rr=(int)(rr+tcr-bcolr);
			gg=(int)(gg+tcg-bcolg);
			bb=(int)(bb+tcb-bcolb);
			if (rr<0) rr=0; if (rr>255) rr=255;
			if (gg<0) gg=0; if (gg>255) gg=255;
			if (bb<0) bb=0; if (bb>255) bb=255;
			Set_color(ix,iy,(unsigned char)rr,(unsigned char)gg,(unsigned char)bb);
		}
}




void TQXDIBitmap::render(CDC *dc, int x0, int y0)
{
	if (!present) return;

	SetMapMode(*dc,MM_TEXT);

	StretchDIBits(*dc,
		x0,y0,xres,yres,
		0,0,xres,yres,
		dat,&bminfo,DIB_RGB_COLORS,SRCCOPY);
}
