#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "stdafx.h"

#include "tools.h"

#include <gdiplus.h>
using namespace Gdiplus;

//#include "b_math.h"

//#include "sxstring.h"
//#include "sxfile.h"
//#include "sxwin.h"

#include "qerror.h"
#include "qstringformat.h"

#include "qxstoredbitmap.h"


Tbmp::Tbmp()
{
	present=0;
	xres=0;yres=0;
	val8r=NULL;val8g=NULL;val8b=NULL;
	val16=NULL;val8=NULL;
	minval=0;maxval=255;
}

Tbmp::~Tbmp()
{
	if (val8r!=NULL) free(val8r);
	if (val8g!=NULL) free(val8g);
	if (val8b!=NULL) free(val8b);
	if (val16!=NULL) free(val16);
	if (val8!=NULL) free(val8);
}

int Tbmp::loadthumbnail(StrPtr ifilename)
{
//	Bitmap bm(p;
//	IStream *str;
//	Image::FromStream(str,true,false);
	//GetThumbnailImage
	throw QError(_text("Not implemented"));
	return 0;
}


void Tbmp::clear()
{
	if (val8r!=NULL) free(val8r);
	if (val8g!=NULL) free(val8g);
	if (val8b!=NULL) free(val8b);
	if (val16!=NULL) free(val16);
	if (val8!=NULL) free(val8);
	val8r=NULL;val8g=NULL;val8b=NULL;val16=NULL;val8=NULL;
	present=0;format=0;
	xres=0;yres=0;
}


int Tbmp::loadgdiplus(StrPtr ifilename)
{
	int formatok,i,j,k;

	clear();

	filename=ifilename;
	Bitmap bitmap(filename);


	if (bitmap.GetLastStatus()!=Ok)
	{
		error="Unable to read file";
		return 0;
	}
	yres=bitmap.GetHeight();
	xres=bitmap.GetWidth();

	formatok=0;
	PixelFormat pixfmt=bitmap.GetPixelFormat();

/*
	int vl;
	vl=PixelFormat1bppIndexed   ;
	vl=PixelFormat4bppIndexed   ;
	vl=PixelFormat8bppIndexed   ;
	vl=PixelFormat16bppGrayScale;
	vl=PixelFormat16bppRGB555   ;
	vl=PixelFormat16bppRGB565   ;
	vl=PixelFormat16bppARGB1555 ;
	vl=PixelFormat24bppRGB      ;
	vl=PixelFormat32bppRGB      ;
	vl=PixelFormat32bppARGB     ;
	vl=PixelFormat32bppPARGB    ;
	vl=PixelFormat48bppRGB      ;
	vl=PixelFormat64bppARGB     ;
	vl=PixelFormat64bppPARGB    ;
	vl=PixelFormatMax           ;
*/

	if (pixfmt==PixelFormat1bppIndexed) { error="1 bit per pixel format is not supported"; return 0; }



	if (pixfmt==PixelFormat24bppRGB)
	{
		format=3;
		Rect rec(0,0,xres,yres);
		BitmapData lockeddata;
		if (bitmap.LockBits(&rec,ImageLockModeRead,PixelFormat24bppRGB,&lockeddata)!=Ok)
		{
			error="Unable to get bitmap data";
			return 0;
		}
		unsigned char *vls=(unsigned char*)lockeddata.Scan0;
		int stride=lockeddata.Stride;
		val8r=(unsigned char*)malloc(xres*yres);
		val8g=(unsigned char*)malloc(xres*yres);
		val8b=(unsigned char*)malloc(xres*yres);
		int idx1=0;
		int idx2;
		for (j=0; j<yres; j++)
		{
			idx2=j*stride;
			for (i=0; i<xres; i++)
			{
				val8b[idx1]=vls[idx2];idx2++;
				val8g[idx1]=vls[idx2];idx2++;
				val8r[idx1]=vls[idx2];idx2++;
				idx1++;
			}
		}
		bitmap.UnlockBits(&lockeddata);
		formatok=1;
		minval=0;maxval=255;
	}


	if (pixfmt==PixelFormat32bppRGB)
	{
		format=3;
		Rect rec(0,0,xres,yres);
		BitmapData lockeddata;
		if (bitmap.LockBits(&rec,ImageLockModeRead,PixelFormat32bppRGB,&lockeddata)!=Ok)
		{
			error="Unable to get bitmap data";
			return 0;
		}
		unsigned char *vls=(unsigned char*)lockeddata.Scan0;
		int stride=lockeddata.Stride;
		val8r=(unsigned char*)malloc(xres*yres);
		val8g=(unsigned char*)malloc(xres*yres);
		val8b=(unsigned char*)malloc(xres*yres);
		for (i=0; i<xres; i++)
			for (j=0; j<yres; j++)
			{
				val8r[i+j*xres]=vls[j*stride+4*i+2];
				val8g[i+j*xres]=vls[j*stride+4*i+1];
				val8b[i+j*xres]=vls[j*stride+4*i+0];
			}
		bitmap.UnlockBits(&lockeddata);
		formatok=1;
		minval=0;maxval=255;
	}


	if (pixfmt==PixelFormat32bppARGB)
	{
		format=3;
		Rect rec(0,0,xres,yres);
		BitmapData lockeddata;
		if (bitmap.LockBits(&rec,ImageLockModeRead,PixelFormat32bppARGB,&lockeddata)!=Ok)
		{
			error="Unable to get bitmap data";
			return 0;
		}
		unsigned char *vls=(unsigned char*)lockeddata.Scan0;
		int stride=lockeddata.Stride;
		val8r=(unsigned char*)malloc(xres*yres);
		val8g=(unsigned char*)malloc(xres*yres);
		val8b=(unsigned char*)malloc(xres*yres);
		for (i=0; i<xres; i++)
			for (j=0; j<yres; j++)
			{
				val8r[i+j*xres]=vls[j*stride+4*i+2];
				val8g[i+j*xres]=vls[j*stride+4*i+1];
				val8b[i+j*xres]=vls[j*stride+4*i+0];
			}
		bitmap.UnlockBits(&lockeddata);
		formatok=1;
		minval=0;maxval=255;
	}

	if (pixfmt==PixelFormat8bppIndexed)
	{
		int palsize=bitmap.GetPaletteSize();
		DWORD argb;
		if (palsize<650000)
		{
			ColorPalette *pal=(ColorPalette*)malloc(palsize+1000);
			if (bitmap.GetPalette(pal,palsize)==Ok)
			{
				if ((pal->Flags&PaletteFlagsGrayScale)>0)
				{
					format=1;
					Rect rec(0,0,xres,yres);
					BitmapData lockeddata;
					if (bitmap.LockBits(&rec,ImageLockModeRead,PixelFormat8bppIndexed,&lockeddata)!=Ok)
					{
						error="Unable to get bitmap data";
						return 0;
					}
					unsigned char *vls=(unsigned char*)lockeddata.Scan0;
					int stride=lockeddata.Stride;
					val8=(unsigned char*)malloc(xres*yres);
					for (i=0; i<xres; i++)
						for (j=0; j<yres; j++)
						{
							k=vls[j*stride+i];
							argb=pal->Entries[k];
							val8[i+j*xres]=255-((unsigned char*)&argb)[0];
						}
					bitmap.UnlockBits(&lockeddata);
					formatok=1;
				}
				else
				{
					format=3;
					Rect rec(0,0,xres,yres);
					BitmapData lockeddata;
					if (bitmap.LockBits(&rec,ImageLockModeRead,PixelFormat8bppIndexed,&lockeddata)!=Ok)
					{
						error="Unable to get bitmap data";
						return 0;
					}
					unsigned char *vls=(unsigned char*)lockeddata.Scan0;
					int stride=lockeddata.Stride;
					val8r=(unsigned char*)malloc(xres*yres);
					val8g=(unsigned char*)malloc(xres*yres);
					val8b=(unsigned char*)malloc(xres*yres);
					for (i=0; i<xres; i++)
						for (j=0; j<yres; j++)
						{
							k=vls[j*stride+i];
							argb=pal->Entries[k];
							val8r[i+j*xres]=((unsigned char*)&argb)[2];
							val8g[i+j*xres]=((unsigned char*)&argb)[1];
							val8b[i+j*xres]=((unsigned char*)&argb)[0];
						}
					bitmap.UnlockBits(&lockeddata);
					formatok=1;
				}
			}
			free(pal);
		}
		minval=0;maxval=255;
	}

	if (pixfmt==PixelFormat16bppGrayScale)
	{
		format=2;
		Rect rec(0,0,xres,yres);
		BitmapData lockeddata;
		if (bitmap.LockBits(&rec,ImageLockModeRead,PixelFormat16bppGrayScale,&lockeddata)!=Ok)
		{
			error="Unable to get bitmap data";
			return 0;
		}
		unsigned __int16 *vls=(unsigned __int16*)lockeddata.Scan0;
		int stride=lockeddata.Stride;
		val16=(unsigned __int16*)malloc(xres*yres*2);
		for (i=0; i<xres; i++)
			for (j=0; j<yres; j++)
				val16[i+j*xres]=vls[j*stride+i];
		bitmap.UnlockBits(&lockeddata);
		formatok=1;
		minval=0;maxval=65535;
	}

	if (!formatok)
	{
		error="Invalid pixel format";
		return 0;
	}
	present=1;
	return 1;
}


int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes
   ImageCodecInfo* pImageCodecInfo = NULL;
   GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure
   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);
   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }
   free(pImageCodecInfo);
   return -1;  // Failure
}


void Tbmp::savejpg(StrPtr filename, int qual)
{
	if (format!=3) throw QError(_text("Invalid bitmap format"));

	CLSID pngClsid;
	GetEncoderClsid(_qstr("image/jpeg"), &pngClsid);

	Bitmap bitmap(xres,yres,PixelFormat24bppRGB);
	Rect rec(0,0,xres,yres);
	BitmapData lockeddata;
	if (bitmap.LockBits(&rec,ImageLockModeWrite,PixelFormat24bppRGB,&lockeddata)!=Ok)
		throw QError(_text("Unable to save bitmap"));
	unsigned char *vls=(unsigned char*)lockeddata.Scan0;
	int stride=lockeddata.Stride;

	for (int i=0; i<xres; i++)
		for (int j=0; j<yres; j++)
		{
			vls[j*stride+3*i+2]=(unsigned char)(val8r[i+j*xres]);
			vls[j*stride+3*i+1]=(unsigned char)(val8g[i+j*xres]);
			vls[j*stride+3*i+0]=(unsigned char)(val8b[i+j*xres]);
		}
	bitmap.UnlockBits(&lockeddata);

	EncoderParameters encoderParameters;
	encoderParameters.Count = 1;
	encoderParameters.Parameter[0].Guid = EncoderQuality;
	encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParameters.Parameter[0].NumberOfValues = 1;
	ULONG quality =qual;
	encoderParameters.Parameter[0].Value = &quality;

	bitmap.Save(filename,&pngClsid, &encoderParameters);
}



void Tbmp::init(int iresx, int iresy, int iformat, int iminval, int imaxval)
{
	int i;

	clear();
	present=1;format=iformat;
	xres=iresx;
	yres=iresy;
	minval=iminval;
	maxval=imaxval;
	int pixcount=xres*yres;
	if (format==1)
	{
		val8=(unsigned char*)malloc(xres*yres);
		for (i=0; i<pixcount; i++) val8[i]=0;
	}
	if (format==2)
	{
		val16=(unsigned __int16*)malloc(xres*yres*2);
		for (i=0; i<pixcount; i++) val16[i]=0;
	}
	if (format==3)
	{
		val8r=(unsigned char*)malloc(xres*yres);
		val8g=(unsigned char*)malloc(xres*yres);
		val8b=(unsigned char*)malloc(xres*yres);
		for (i=0; i<pixcount; i++) { val8r[i]=255;val8g[i]=255;val8b[i]=255; }
	}
}

void Tbmp::copyfrom(const Tbmp *bmp)
{
	present=0;format=0;
	clear();
	xres=bmp->xres;
	yres=bmp->yres;format=bmp->format;
	minval=bmp->minval;
	maxval=bmp->maxval;
	if (bmp->present)
	{
		if (bmp->format==1)
		{
			present=1;format=1;
			val8=(unsigned char*)malloc(xres*yres);
			memcpy(val8,bmp->val8,xres*yres);
		}
		if (bmp->format==2)
		{
			present=1;format=2;
			val16=(unsigned __int16*)malloc(xres*yres*2);
			memcpy(val16,bmp->val16,xres*yres*2);
		}
		if (bmp->format==3)
		{
			present=1;format=3;
			val8r=(unsigned char*)malloc(xres*yres);
			memcpy(val8r,bmp->val8r,xres*yres);
			val8g=(unsigned char*)malloc(xres*yres);
			memcpy(val8g,bmp->val8g,xres*yres);
			val8b=(unsigned char*)malloc(xres*yres);
			memcpy(val8b,bmp->val8b,xres*yres);
		}
	}
}


void Tbmp::reducefrom(const Tbmp *bmp, int redfac)
{
	if ((redfac<1)||(redfac>bmp->G_xres()/2)||(redfac>bmp->G_yres()/2)) throw QError(_text("Invalid bitmap reduction factor"));
	if (bmp->format!=3) throw QError(_text("Invalid bitmap format"));

	int ix,iy,jx,jy,ofs,smr,smg,smb;
	clear();
	xres=bmp->G_xres()/redfac;
	yres=bmp->G_yres()/redfac;
	int pixcount=xres*yres;
	minval=bmp->minval;maxval=bmp->maxval;

	if (bmp->format==3)
	{
		present=1;format=3;
		val8r=(unsigned char*)malloc(xres*yres);
		val8g=(unsigned char*)malloc(xres*yres);
		val8b=(unsigned char*)malloc(xres*yres);
		for (ix=0; ix<xres; ix++)
			for (iy=0; iy<yres; iy++)
			{
				smr=0;smg=0;smb=0;
				for (jx=0; jx<redfac; jx++)
					for (jy=0; jy<redfac; jy++)
					{
						ofs=(iy*redfac+jy)*bmp->xres+ix*redfac+jx;
						smr+=bmp->val8r[ofs];
						smg+=bmp->val8g[ofs];
						smb+=bmp->val8b[ofs];
					}
				ofs=iy*xres+ix;
				val8r[ofs]=smr/(redfac*redfac);
				val8g[ofs]=smg/(redfac*redfac);
				val8b[ofs]=smb/(redfac*redfac);
			}
	}

}


void Tbmp::cropfrom(const Tbmp *bmp, int x0, int y0, int lx, int ly, double rotat)
{
	int i,j,i1,i2,ofs,len;
	clear();
	xres=lx;
	yres=ly;
	int pixcount=xres*yres;
	minval=bmp->minval;maxval=bmp->maxval;
	if ((bmp->present)&&(fabs(rotat)<=0.001))
	{
		if (bmp->format==1)
		{
			present=1;format=1;
			val8=(unsigned char*)malloc(xres*yres);for (i=0; i<pixcount; i++) val8[i]=0;
			for (j=0; j<yres; j++)
			{
				if ((y0+j>=0)&&(y0+j<bmp->yres))
				{
					i1=max(0,-x0);
					i2=min(xres-1,bmp->xres-1-x0);
					ofs=(y0+j)*bmp->xres+x0+i1;
					len=i2-i1+1;
					memcpy(&(val8[j*xres+i1]),&(bmp->val8[ofs]),len);
				}
			}
		}
		if (bmp->format==2)
		{
			present=1;format=2;
			val16=(unsigned __int16*)malloc(xres*yres*2);for (i=0; i<pixcount; i++) val16[i]=0;
			for (j=0; j<yres; j++)
			{
				if ((y0+j>=0)&&(y0+j<bmp->yres))
				{
					i1=max(0,-x0);
					i2=min(xres-1,bmp->xres-1-x0);
					ofs=(y0+j)*bmp->xres+x0+i1;
					len=i2-i1+1;
					memcpy(&(val16[j*xres+i1]),&(bmp->val16[ofs]),len*2);
				}
			}
		}
		if (bmp->format==3)
		{
			present=1;format=3;
			val8r=(unsigned char*)malloc(xres*yres);
			val8g=(unsigned char*)malloc(xres*yres);
			val8b=(unsigned char*)malloc(xres*yres);
			for (i=0; i<pixcount; i++) { val8r[i]=255;val8g[i]=255;val8b[i]=255; }
			for (j=0; j<yres; j++)
			{
				if ((y0+j>=0)&&(y0+j<bmp->yres))
				{
					i1=max(0,-x0);
					i2=min(xres-1,bmp->xres-1-x0);
					ofs=(y0+j)*bmp->xres+x0+i1;
					len=i2-i1+1;
					memcpy(&(val8r[j*xres+i1]),&(bmp->val8r[ofs]),len);
					memcpy(&(val8g[j*xres+i1]),&(bmp->val8g[ofs]),len);
					memcpy(&(val8b[j*xres+i1]),&(bmp->val8b[ofs]),len);
				}
			}
		}
	}

	if ((bmp->present)&&(fabs(rotat)>0.001))
	{
		if (bmp->format==1)
		{
			int cs,sn,i2f,j2f,i2,j2,vl;
			present=1;format=1;
			val8=(unsigned char*)calloc(xres*yres,1);
			cs=(int)(10000*cos(rotat/180*3.1415927));
			sn=(int)(10000*sin(rotat/180*3.1415927));
			for (i=0; i<xres; i++)
				for (j=0; j<yres; j++)
				{
					i2f=x0*10000+i*cs+j*sn;
					j2f=y0*10000-i*sn+j*cs;
					i2=i2f/10000;j2=j2f/10000;
					i2f=(i2f%10000+50)/100;j2f=(j2f%10000+50)/100;
					if ((i2>=0)&&(i2<bmp->xres-1)&&(j2>=0)&&(j2<bmp->yres-1))
					{
						vl=   bmp->val8[(j2+0)*bmp->xres+i2+0]*(100-j2f)*(100-i2f)
							+ bmp->val8[(j2+0)*bmp->xres+i2+1]*(100-j2f)*(i2f)
							+ bmp->val8[(j2+1)*bmp->xres+i2+1]*(j2f)*(i2f)
							+ bmp->val8[(j2+1)*bmp->xres+i2+0]*(j2f)*(100-i2f);
						val8[j*xres+i]=min(255,(vl+5000)/10000);
					}
				}
		}
		if (bmp->format==2)
		{
			int cs,sn,i2f,j2f,i2,j2,vl;
			present=1;format=2;
			val16=(unsigned __int16*)calloc(xres*yres,2);
			cs=(int)(10000*cos(rotat/180*3.1415927));
			sn=(int)(10000*sin(rotat/180*3.1415927));
			for (i=0; i<xres; i++)
				for (j=0; j<yres; j++)
				{
					i2f=x0*10000+i*cs+j*sn;
					j2f=y0*10000-i*sn+j*cs;
					i2=i2f/10000;j2=j2f/10000;
					i2f=(i2f%10000+50)/100;j2f=(j2f%10000+50)/100;
					if ((i2>=0)&&(i2<bmp->xres-1)&&(j2>=0)&&(j2<bmp->yres-1))
					{
						vl=   bmp->val16[(j2+0)*bmp->xres+i2+0]*(100-j2f)*(100-i2f)
							+ bmp->val16[(j2+0)*bmp->xres+i2+1]*(100-j2f)*(i2f)
							+ bmp->val16[(j2+1)*bmp->xres+i2+1]*(j2f)*(i2f)
							+ bmp->val16[(j2+1)*bmp->xres+i2+0]*(j2f)*(100-i2f);
						val16[j*xres+i]=min(65535,(vl+5000)/10000);
					}
				}
		}
		if (bmp->format==3)
		{
			int cs,sn,i2f,j2f,i2,j2,vl;
			present=1;format=3;
			val8r=(unsigned char*)calloc(xres*yres,1);
			val8g=(unsigned char*)calloc(xres*yres,1);
			val8b=(unsigned char*)calloc(xres*yres,1);
			cs=(int)(10000*cos(rotat/180*3.1415927));
			sn=(int)(10000*sin(rotat/180*3.1415927));
			for (i=0; i<xres; i++)
				for (j=0; j<yres; j++)
				{
					i2f=x0*10000+i*cs+j*sn;
					j2f=y0*10000-i*sn+j*cs;
					i2=i2f/10000;j2=j2f/10000;
					i2f=(i2f%10000+50)/100;j2f=(j2f%10000+50)/100;
					if ((i2>=0)&&(i2<bmp->xres-1)&&(j2>=0)&&(j2<bmp->yres-1))
					{
						vl=   bmp->val8r[(j2+0)*bmp->xres+i2+0]*(100-j2f)*(100-i2f)
							+ bmp->val8r[(j2+0)*bmp->xres+i2+1]*(100-j2f)*(i2f)
							+ bmp->val8r[(j2+1)*bmp->xres+i2+1]*(j2f)*(i2f)
							+ bmp->val8r[(j2+1)*bmp->xres+i2+0]*(j2f)*(100-i2f);
						val8r[j*xres+i]=min(255,(vl+5000)/10000);
						vl=   bmp->val8g[(j2+0)*bmp->xres+i2+0]*(100-j2f)*(100-i2f)
							+ bmp->val8g[(j2+0)*bmp->xres+i2+1]*(100-j2f)*(i2f)
							+ bmp->val8g[(j2+1)*bmp->xres+i2+1]*(j2f)*(i2f)
							+ bmp->val8g[(j2+1)*bmp->xres+i2+0]*(j2f)*(100-i2f);
						val8g[j*xres+i]=min(255,(vl+5000)/10000);
						vl=   bmp->val8b[(j2+0)*bmp->xres+i2+0]*(100-j2f)*(100-i2f)
							+ bmp->val8b[(j2+0)*bmp->xres+i2+1]*(100-j2f)*(i2f)
							+ bmp->val8b[(j2+1)*bmp->xres+i2+1]*(j2f)*(i2f)
							+ bmp->val8b[(j2+1)*bmp->xres+i2+0]*(j2f)*(100-i2f);
						val8b[j*xres+i]=min(255,(vl+5000)/10000);
					}
				}
		}
	}
}

void Tbmp::insert(Tbmp *bmp, int px, int py)
{
	int j,i1,i2,ofs,len;

	if ((!present)||(!bmp->present)||(format!=bmp->format)) return;
	if (format==1)
	{
		for (j=0; j<bmp->yres; j++)
		{
			if ((py+j>=0)&&(py+j<yres))
			{
				i1=max(0,-px);
				i2=min(bmp->xres-1,xres-1-px);
				ofs=(py+j)*xres+px+i1;
				len=i2-i1;
				memcpy(&(val8[ofs]),&(bmp->val8[j*bmp->xres+i1]),len);
			}
		}
	}
	if (format==2)
	{
		for (j=0; j<bmp->yres; j++)
		{
			if ((py+j>=0)&&(py+j<yres))
			{
				i1=max(0,-px);
				i2=min(bmp->xres-1,xres-1-px);
				ofs=(py+j)*xres+px+i1;
				len=i2-i1;
				memcpy(&(val16[ofs]),&(bmp->val16[j*bmp->xres+i1]),len*2);
			}
		}
	}
	if (format==3)
	{
		for (j=0; j<bmp->yres; j++)
		{
			if ((py+j>=0)&&(py+j<yres))
			{
				i1=max(0,-px);
				i2=min(bmp->xres-1,xres-1-px);
				ofs=(py+j)*xres+px+i1;
				len=i2-i1+1;
				memcpy(&(val8r[ofs]),&(bmp->val8r[j*bmp->xres+i1]),len);
				memcpy(&(val8g[ofs]),&(bmp->val8g[j*bmp->xres+i1]),len);
				memcpy(&(val8b[ofs]),&(bmp->val8b[j*bmp->xres+i1]),len);
			}
		}
	}
}


void Tbmp::mirrordiag()
{
	int i,j;
	if (!present) return;
	if (format==1)
	{
		unsigned char *tmp=(unsigned char*)malloc(xres*yres);
		for (i=0; i<xres; i++) for (j=0; j<yres; j++)
			tmp[i*yres+j]=val8[j*xres+i];
		memcpy(val8,tmp,xres*yres);
		free(tmp);
		i=xres;xres=yres;yres=i;
	}
	if (format==2)
	{
		unsigned __int16 *tmp=(unsigned __int16*)malloc(xres*yres*2);
		for (i=0; i<xres; i++) for (j=0; j<yres; j++)
			tmp[i*yres+j]=val16[j*xres+i];
		memcpy(val16,tmp,xres*yres*2);
		free(tmp);
		i=xres;xres=yres;yres=i;
	}
	if (format==3)
	{
		unsigned char *tmp=(unsigned char*)malloc(xres*yres);
		for (i=0; i<xres; i++) for (j=0; j<yres; j++)
			tmp[i*yres+j]=val8r[j*xres+i];
		memcpy(val8r,tmp,xres*yres);
		for (i=0; i<xres; i++) for (j=0; j<yres; j++)
			tmp[i*yres+j]=val8g[j*xres+i];
		memcpy(val8g,tmp,xres*yres);
		for (i=0; i<xres; i++) for (j=0; j<yres; j++)
			tmp[i*yres+j]=val8b[j*xres+i];
		memcpy(val8b,tmp,xres*yres);
		free(tmp);
		i=xres;xres=yres;yres=i;
	}
}

void Tbmp::mirrorhoriz()
{
	int i,j;
	if (!present) return;
	if (format==1)
	{
		unsigned char tmp;
		for (i=0; i<xres/2; i++)
			for (j=0; j<yres; j++)
			{
				tmp=val8[i+j*xres];
				val8[i+j*xres]=val8[xres-1-i+j*xres];
				val8[xres-1-i+j*xres]=tmp;
			}
	}
	if (format==2)
	{
		unsigned __int16 tmp;
		for (i=0; i<xres/2; i++)
			for (j=0; j<yres; j++)
			{
				tmp=val16[i+j*xres];
				val16[i+j*xres]=val16[xres-1-i+j*xres];
				val16[xres-1-i+j*xres]=tmp;
			}
	}
	if (format==3)
	{
		unsigned char tmp;
		for (i=0; i<xres/2; i++)
			for (j=0; j<yres; j++)
			{
				tmp=val8r[i+j*xres];
				val8r[i+j*xres]=val8r[xres-1-i+j*xres];
				val8r[xres-1-i+j*xres]=tmp;
				tmp=val8g[i+j*xres];
				val8g[i+j*xres]=val8g[xres-1-i+j*xres];
				val8g[xres-1-i+j*xres]=tmp;
				tmp=val8b[i+j*xres];
				val8b[i+j*xres]=val8b[xres-1-i+j*xres];
				val8b[xres-1-i+j*xres]=tmp;
			}
	}
}


void Tbmp::mirrorvert()
{
	int i,j;
	if (!present) return;
	if (format==1)
	{
		unsigned char tmp;
		for (i=0; i<xres; i++)
			for (j=0; j<yres/2; j++)
			{
				tmp=val8[i+j*xres];
				val8[i+j*xres]=val8[i+(yres-1-j)*xres];
				val8[i+(yres-1-j)*xres]=tmp;
			}
	}
	if (format==2)
	{
		unsigned __int16 tmp;
		for (i=0; i<xres; i++)
			for (j=0; j<yres/2; j++)
			{
				tmp=val16[i+j*xres];
				val16[i+j*xres]=val16[i+(yres-1-j)*xres];
				val16[i+(yres-1-j)*xres]=tmp;
			}
	}
	if (format==3)
	{
		unsigned char tmp;
		for (i=0; i<xres; i++)
			for (j=0; j<yres/2; j++)
			{
				tmp=val8r[i+j*xres];
				val8r[i+j*xres]=val8r[i+(yres-1-j)*xres];
				val8r[i+(yres-1-j)*xres]=tmp;
				tmp=val8g[i+j*xres];
				val8g[i+j*xres]=val8g[i+(yres-1-j)*xres];
				val8g[i+(yres-1-j)*xres]=tmp;
				tmp=val8b[i+j*xres];
				val8b[i+j*xres]=val8b[i+(yres-1-j)*xres];
				val8b[i+(yres-1-j)*xres]=tmp;
			}
	}
}

void Tbmp::convertgray(int channel)
{
	int i;
	if ((!present)||(format!=3)) return;
	format=1;
	val8=(unsigned char*)malloc(xres*yres);
	int pixcount=xres*yres;
	if (channel==0)
		for (i=0; i<pixcount; i++)
			val8[i]=255-((int)val8r[i]+(int)val8g[i]+(int)val8b[i])/3;
	if (channel==1)
		for (i=0; i<pixcount; i++)
			val8[i]=255-val8r[i];
	if (channel==2)
		for (i=0; i<pixcount; i++)
			val8[i]=255-val8g[i];
	if (channel==3)
		for (i=0; i<pixcount; i++)
			val8[i]=255-val8b[i];
	free(val8r);free(val8g);free(val8b);
	val8r=NULL;val8g=NULL;val8b=NULL;
}


void Tbmp::invert()
{
	int i;
	if (!present) return;
	int pixcount=xres*yres;
	if (format==1)
		for (i=0; i<pixcount; i++) val8[i]=255-val8[i];
	if (format==2)
		for (i=0; i<pixcount; i++) val16[i]=65535-val16[i];
	if (format==3)
		for (i=0; i<pixcount; i++)
		{
			val8r[i]=255-val8r[i];
			val8g[i]=255-val8g[i];
			val8b[i]=255-val8b[i];
		}
}

void Tbmp::expandluminosityrange()
{
	int i,pixcount;
	if (!present) return;
	pixcount=xres*yres;
	if (format==1)
	{
		minval=255;maxval=0;
		for (i=0; i<pixcount; i++)
		{
			if (val8[i]<minval) minval=val8[i];
			if (val8[i]>maxval) maxval=val8[i];
		}
	}
	if (format==2)
	{
		minval=65535;maxval=0;
		for (i=0; i<pixcount; i++)
		{
			if (val16[i]<minval) minval=val8[i];
			if (val16[i]>maxval) maxval=val8[i];
		}
	}
	if (format==3)
	{
		minval=255;maxval=0;
		for (i=0; i<pixcount; i++)
		{
			if (val8r[i]<minval) minval=val8r[i];
			if (val8r[i]>maxval) maxval=val8r[i];
			if (val8g[i]<minval) minval=val8g[i];
			if (val8g[i]>maxval) maxval=val8g[i];
			if (val8b[i]<minval) minval=val8b[i];
			if (val8b[i]>maxval) maxval=val8b[i];
		}
	}
}

void Tbmp::tostring(QString &str, int tpe)
{
	str=filename;
	str+=TFormatString(_qstr(" ^1x^2"),xres,yres);
}

void Tbmp::fromstring(StrPtr str)
{
	throw QError(_text("Unable to convert bitmap from a string"));
}

void Tbmp::operator=(const Tbmp &bmp)
{
	copyfrom(&bmp);
}

void Tbmp::streamout(QBinTagWriter &writer)
{
	throw QError(_text("Streaming of bitmaps is not implemented"));
}

void Tbmp::streamin(QBinTagReader &reader)
{
	throw QError(_text("Streaming of bitmaps is not implemented"));
}


