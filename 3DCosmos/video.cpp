#include "stdafx.h"
#include <stdio.h>
#include <math.h>

#include "gl/gl.h"
#include "gl/glu.h"
//#include "gl/glaux.h"

#include "qerror.h"
#include "qstringformat.h"



#include "video.h"

void mainwin_startfullscreenvideo();
void mainwin_stopfullscreenvideo();


Taviplayer::Taviplayer()
{
	opened=false;
}

Taviplayer::~Taviplayer()
{
	close();
}


void Taviplayer::load(StrPtr filename)
{
	close();
	opened=false;

	AVIFileInit();												

	// Opens The AVI Stream
	if (AVIStreamOpenFromFile(&pavi,filename,streamtypeVIDEO,0,OF_READ,NULL) !=0)
	{
		throw QError(TFormatString(_text("Unable to open AVI file ^1"),filename));
	}

	// Reads Information About The Stream Into psi
	AVIStreamInfo(pavi, &psi, sizeof(psi));						
	resx=psi.rcFrame.right-psi.rcFrame.left;					
	resy=psi.rcFrame.bottom-psi.rcFrame.top;
	framecount=AVIStreamLength(pavi);

	// Calculate Rough seconds Per Frame
	framedelay=AVIStreamSampleToTime(pavi,framecount)/1000.0/framecount;

	BITMAPINFOHEADER BIH;
    BIH.biSize = sizeof(BITMAPINFOHEADER);
    BIH.biWidth = resx;
    BIH.biHeight = resy;
    BIH.biPlanes = 1;
    BIH.biBitCount = 24;
    BIH.biCompression = BI_RGB;
    BIH.biSizeImage = 0;
    BIH.biXPelsPerMeter = 0;
    BIH.biYPelsPerMeter = 0;
    BIH.biClrUsed = 0;
    BIH.biClrImportant = 0;


	// Create The PGETFRAME	Using Our Request Mode
//	pgf=AVIStreamGetFrameOpen(pavi, /*NULL*/ &BIH);						
//	pgf=AVIStreamGetFrameOpen(pavi, NULL );						
	pgf=AVIStreamGetFrameOpen(pavi, (LPBITMAPINFOHEADER)AVIGETFRAMEF_BESTDISPLAYFMT );						
	if (pgf==NULL) return;


	opened=true;
	curframe=0;
}


void Taviplayer::close()
{
	if (opened)
	{
		AVIStreamGetFrameClose(pgf);								// Deallocates The GetFrame Resources
		AVIStreamRelease(pavi);										// Release The Stream
		AVIFileExit();												// Release The File
	}
	opened=false;
}

LPBITMAPINFOHEADER Taviplayer::getframe(int nr)
{
	if (!opened) return NULL;
	if ((nr<0)||(nr>=framecount)) return NULL;
	return (LPBITMAPINFOHEADER)AVIStreamGetFrame(pgf,nr);
}

LPBITMAPINFOHEADER Taviplayer::getcurframe()
{
	return getframe(curframe);
}


void Taviplayer::setcurframe(int framenr)
{
	curframe=max(0,min(framecount-1,framenr));
}









void flipIt1(void* buffer)										// Flips The Red And Blue Bytes (256x256)
{
	void* b = buffer;											// Pointer To The Buffer
	__asm														// Assembler Code To Follow
	{
		mov ecx, 256*256										// Counter Set To Dimensions Of Our Memory Block
		mov ebx, b												// Points ebx To Our Data (b)
		label:													// Label Used For Looping
			mov al,[ebx+0]										// Loads Value At ebx Into al
			mov ah,[ebx+2]										// Loads Value At ebx+2 Into ah
			mov [ebx+2],al										// Stores Value In al At ebx+2
			mov [ebx+0],ah										// Stores Value In ah At ebx
			
			add ebx,3											// Moves Through The Data By 3 Bytes
			dec ecx												// Decreases Our Loop Counter
			jnz label											// If Not Zero Jump Back To Label
	}
}

void flipIt2(void* buffer)										// Flips The Red And Blue Bytes (256x256)
{
	void* b = buffer;											// Pointer To The Buffer
	__asm														// Assembler Code To Follow
	{
		mov ecx, 512*512										// Counter Set To Dimensions Of Our Memory Block
		mov ebx, b												// Points ebx To Our Data (b)
		label:													// Label Used For Looping
			mov al,[ebx+0]										// Loads Value At ebx Into al
			mov ah,[ebx+2]										// Loads Value At ebx+2 Into ah
			mov [ebx+2],al										// Stores Value In al At ebx+2
			mov [ebx+0],ah										// Stores Value In ah At ebx
			
			add ebx,3											// Moves Through The Data By 3 Bytes
			dec ecx												// Decreases Our Loop Counter
			jnz label											// If Not Zero Jump Back To Label
	}
}

void flipIt3(void* buffer)										// Flips The Red And Blue Bytes (256x256)
{
	void* b = buffer;											// Pointer To The Buffer
	__asm														// Assembler Code To Follow
	{
		mov ecx, 1024*512										// Counter Set To Dimensions Of Our Memory Block
		mov ebx, b												// Points ebx To Our Data (b)
		label:													// Label Used For Looping
			mov al,[ebx+0]										// Loads Value At ebx Into al
			mov ah,[ebx+2]										// Loads Value At ebx+2 Into ah
			mov [ebx+2],al										// Stores Value In al At ebx+2
			mov [ebx+0],ah										// Stores Value In ah At ebx
			
			add ebx,3											// Moves Through The Data By 3 Bytes
			dec ecx												// Decreases Our Loop Counter
			jnz label											// If Not Zero Jump Back To Label
	}
}



Tvideotexture::Tvideotexture(Taviplayer *iavi)
{
	avi=iavi;
	resx=iavi->resx; resy=iavi->resy;

	// Get a compatible DC
	hdc=CreateCompatibleDC(0);									
	// Grab A Device Context For Our Dib
	hdd = DrawDibOpen();	

	bmih.biSize = sizeof (BITMAPINFOHEADER);					// Size Of The BitmapInfoHeader
	bmih.biPlanes = 1;											// Bitplanes	
	bmih.biBitCount = 24;										// Bits Format We Want (24 Bit, 3 Bytes)
	bmih.biWidth = resx;
	bmih.biHeight = resy;
	bmih.biCompression = BI_RGB;								// Requested Mode = RGB

	hBitmap = CreateDIBSection (hdc, (BITMAPINFO*)(&bmih), DIB_RGB_COLORS, (void**)(&data), NULL, NULL);
	SelectObject(hdc,hBitmap);								// Select hBitmap Into Our Device Context (hdc)

	// get a new id
	glGenTextures(1,&texid);
	// and bind it as the present texture
	glBindTexture(GL_TEXTURE_2D,texid);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, resx,resy,0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
    ::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    ::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
}

Tvideotexture::~Tvideotexture()
{
	DeleteObject(hBitmap);										// Delete The Device Dependant Bitmap Object
	DrawDibClose(hdd);											// Closes The DrawDib Device Context
	glDeleteTextures(1,&texid);
}


void Tvideotexture::loadcurframe()
{
	LPBITMAPINFOHEADER lpbi=avi->getcurframe();
	if (lpbi==NULL) return;
	BYTE *pdata=(BYTE *)lpbi+lpbi->biSize+lpbi->biClrUsed * sizeof(RGBQUAD);
	DrawDibDraw(hdd, hdc, 0, 0, resx, resy, lpbi, pdata, 0, 0, avi->resx, avi->resy, 0);
	// Swap The Red And Blue Bytes (GL Compatability) : only use if GL_BGR does not work!
/*	if (resscale==1) flipIt1(data);												
	if (resscale==2) flipIt2(data);
	if (resscale==3) flipIt3(data);*/
}



void Tvideotexture::select()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texid);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, resx, resy, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
}

void Tvideotexture::unselect()
{
	glDisable(GL_TEXTURE_2D);
}






//*************************************************************
// Tvideocapture
//*************************************************************

Tvideocapture videocapture;

Tvideocapture* G_videocapture()
{
	return &videocapture;
}

Tvideocapture::Tvideocapture()
{
	recording=false;
}


void Tvideocapture::start(CWnd *iwin, StrPtr ifilename)
{
	win=iwin;
	RECT rc;
	win->GetClientRect(&rc);
	resx=rc.right-2;
	resy=rc.bottom-2;

	filename=ifilename;

	AVIFileInit();
	HRESULT hr=AVIFileOpen(&pfile,filename,OF_WRITE|OF_CREATE,0);
	if (hr!=AVIERR_OK)
	{
		reporterror(_text("Unable to start video (1)"));
		AVIFileExit(); 
		return;
	}

	//create stream
    AVISTREAMINFO strhdr; 
    ::ZeroMemory(&strhdr,sizeof(strhdr));
    strhdr.fccType                = streamtypeVIDEO;
    strhdr.fccHandler             = 0; 
    strhdr.dwScale                = 1;
    strhdr.dwRate                 = 20;
    strhdr.dwSuggestedBufferSize  = 2048*768*3*5;
    SetRect(&strhdr.rcFrame, 0, 0, resx,resy);
    hr=::AVIFileCreateStream(pfile,&pavi,&strhdr);
	if (hr!=AVIERR_OK)
	{
		reporterror(_text("Unable to start video (2)"));
		AVIFileExit(); 
		return;
	}


	//set compression
	AVICOMPRESSOPTIONS avioptions;
    ::ZeroMemory(&avioptions,sizeof(avioptions));

	avioptions.fccType=streamtypeVIDEO;
	avioptions.fccHandler=strhdr.fccHandler;
//	avioptions.fccHandler=mmioFOURCC('M','S','V','C');
	avioptions.dwFlags=AVICOMPRESSF_KEYFRAMES|AVICOMPRESSF_VALID;//|AVICOMPRESSF_DATARATE;
	avioptions.dwKeyFrameEvery=15;

	AVICOMPRESSOPTIONS *ptropts;
	ptropts=&avioptions;
	AVISaveOptions(win->m_hWnd,0,1,&pavi,&ptropts);

    hr=AVIMakeCompressedStream(&cpavi,pavi,ptropts,0);
	if (hr!=AVIERR_OK)
	{
		reporterror(_text("Unable to start video (3)"));
		AVIFileExit(); 
		return;
	}


	hdd = DrawDibOpen();	
	hdc=CreateCompatibleDC(0);

	BITMAPINFO	bmpinfo;
	ZeroMemory(&bmpinfo,sizeof(BITMAPINFO));
	bmpinfo.bmiHeader.biSize = sizeof (BITMAPINFOHEADER);					// Size Of The BitmapInfoHeader
	bmpinfo.bmiHeader.biPlanes = 1;											// Bitplanes	
	bmpinfo.bmiHeader.biBitCount = 24;										// Bits Format We Want (24 Bit, 3 Bytes)
	bmpinfo.bmiHeader.biWidth = resx;										// Width We Want (256 Pixels)
	bmpinfo.bmiHeader.biHeight = resy;										// Height We Want (256 Pixels)
	bmpinfo.bmiHeader.biSizeImage=resx*resy*3;
	bmpinfo.bmiHeader.biCompression = BI_RGB;								// Requested Mode = RGB

	hBitmap = CreateDIBSection (hdc,&bmpinfo, DIB_RGB_COLORS, (void**)(&data), NULL, NULL);
	SelectObject(hdc,hBitmap);								// Select hBitmap Into Our Device Context (hdc)

	hr=AVIStreamSetFormat(cpavi,0,&bmpinfo, bmpinfo.bmiHeader.biSize);
	if (hr!=AVIERR_OK)
	{
		reporterror(_text("Unable to start video (4)"));
		AVIFileExit(); 
		return;
	}

	recording=true;
	framenr=0;
}

void Tvideocapture::stop()
{
	if (recording)
	{
		recording=false;
		AVIStreamRelease(cpavi);
		AVIStreamRelease(pavi);
		AVIFileRelease(pfile);

		DrawDibClose(hdd);
	}
}


void Tvideocapture::addframe()
{
	if (!recording) return;

	//Sleep(50);

	//HDC windc=GetDC(win->m_hWnd);//doesn't seem to work...
	HDC windc=GetDC(0);

	SelectObject(hdc,hBitmap);
	BitBlt(hdc,0,0,resx,resy,windc,0,0,SRCCOPY);


	HRESULT hr=::AVIStreamWrite(cpavi,framenr,1,
		data,resx*resy*3,
		0, NULL, NULL);
	framenr++;
	ReleaseDC(0,windc);
	if (hr!=AVIERR_OK)
	{
		reporterror(_text("Unable to add video frame"));
		AVIFileExit(); 
		return;
	}
}
