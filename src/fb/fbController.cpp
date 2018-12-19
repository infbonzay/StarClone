#include <grplib/grp.h>

#include "vars.h"
#include "const.h"
#include "mytime.h"
#include "rand.h"
#include "Controller.h"


Controller mainController;

//==========================
int Controller::Init(void)
{
    int fbfd;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	char *fbp;
	
	fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) 
	{
        DEBUGMESSCR("Error: cannot open framebuffer device");
        return(-1);
	}

	if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) 
	{
        DEBUGMESSCR("Error reading fixed information");
		close(fbfd);
        return(-1);
	}
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) 
	{
		DEBUGMESSCR("Error reading variable information");
		close(fbfd);
        return(-1);
	}
 
	Surface = new Controller_Surface();
	Surface->fbfd = fbfd;
	Surface->vinfo = vinfo;
	Surface->finfo = finfo;

	mytimer.SetTickTimerFrequency(CYCLESPERSECOND);
	KeysBuffer = new mycycle<uint16_t>(16, MYCYCLE_INFINITE);
	return(0);
}
//===========================================
void Controller::DeInit(void)
{
	if (KeysBuffer)
	{
		delete KeysBuffer;
		KeysBuffer = NULL;
	}
	if (Surface)
	{
		QuitVideoMode();
		delete Surface;
		Surface = NULL;
	}
}
//===========================================
void Controller::SetWindowName(const char *winName)
{
}
//===========================================
int Controller::QueryVideoMode(int x, int y, int bpp, int fullscreen)
{
	if (!Surface->pixels)
	{

		Surface->SavedWidth = vinfo.xres;
		Surface->SavedHeight = vinfo.yres;
		Surface->SavedBpp = vinfo.bits_per_pixel;

		Surface->FBpixelsBufferSize = Surface->SavedWidth * Surface->SavedHeight * Surface->SavedBpp / 8;

		fbp = (char *)mmap(0, Surface->FBpixelsBufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, Surface->fbfd, 0);
		if ((int)fbp == -1) 
		{
			DEBUGMESSCR("Error: failed to map framebuffer device to memory");
			return(0);
		}
		Surface->FBpixels = fbp;

		Surface->pixelsBufferSize = x * y * bpp / 8;
		Surface->pixels = (uint8_t *) wmalloc(Surface->pixelsBufferSize);

		Surface->palette = (uint8_t *)wmalloc(256 * 4);
		memset(Surface->palette, 0, 256 * 4);

		gameconf.grmode.videobuff = Surface->pixels;

		SetVideoBuffer(Surface->pixels);
		
		Surface->XpixelsBufferSize = x * y * Surface->ximagebpp[(Surface->SavedBpp + 1)/8];
		
	}	
	gameconf.grmode.flags |= DISPLAYFLAGS_WINDOWACTIVE;
	return(1 + ((Surface->flags & CFLAG_EXACTBPP) != CFLAG_EXACTBPP));
}
//===========================================
void Controller::QuitVideoMode(void)
{
	if (Surface)
	{
		if (Surface->palette)
		{
			wfree(Surface->palette);
			Surface->palette = NULL;
		}
		if (Surface->pixels)
		{
			wfree(Surface->pixels);
			Surface->pixels = NULL;
		}
		if (Surface->FBpixels)
		{
			munmap(fbp, screensize);
		}	
		if (fbfd)
		{
			close(fbfd);
			fbfd = 0;
		}
	}
}
//===========================================
int Controller::ModifyVideoMode(int x, int y, int bpp, int fullscreen, unsigned char *palette)
{
	return 0;
}
//===========================================
void Controller::UpdateScreenRegions(int nrregions,SCREEN_REGION regions[])
{
	mytimer.CallTimer(MYTIMER_SINCHRONMODE);
	if (gameconf.grmode.flags & DISPLAYFLAGS_WINDOWACTIVE)
	{
		//TODO
	}
	if (nodraw || gameconf.speedconf.cputhrottling)
		usleep(4000);
}
//==========================
void Controller::UpdateScreenRegion(int x, int y, int xsize, int ysize)
{
	SCREEN_REGION scrpart;
	scrpart.x = x;
	scrpart.y = y;
	scrpart.w = xsize;
	scrpart.h = ysize;
	UpdateScreenRegions(1, &scrpart);
}
//===========================================
void Controller::UpdateScreen(void)
{
	UpdateScreenRegion(0, 0, gameconf.grmode.x, gameconf.grmode.y);
}
//===========================================
void Controller::ApplyPalette(unsigned char *pal4,int from,int count)
{
	//TODO
	return;
}
//==========================
void Controller::ApplyPalette(unsigned char *pal4)
{
	ApplyPalette(pal4, 0, 256);
}
//==========================
void Controller::ApplyPalette256x3(unsigned char *pal3)
{
	int i;
	unsigned char pal4[4 * 256];
	unsigned char *pal4p = pal4;
	*pal4p++ = 0;
	*pal4p++ = 0;
	*pal4p++ = 0;
	*pal4p++ = 0;
	pal3 += 3;
	for (i = 1;i < 255;i++)
	{
		*pal4p++ = *pal3++;
		*pal4p++ = *pal3++;
		*pal4p++ = *pal3++;
		*pal4p++ = 0;
	}
	*pal4p++ = 255;
	*pal4p++ = 255;
	*pal4p++ = 255;
	*pal4p++ = 0;

	ApplyPalette(pal4);
}
//===========================================
int  Controller::EventsLoop(void)			//return 1 - on quit
{
	//TODO
	return(0);
}
//===========================================
void Controller::SetKeyMod(int keysym,bool status)
{
}
//===========================================
bool Controller::SetVideoMode(int x, int y)
{
}
//===========================================
void Controller::TransformPixels(int x, int y, int sizex, int sizey)
{
}
//===========================================
void Controller::HideCursor(void)
{
}
//===========================================
void Controller::ShowCursor(void)
{
}
//===========================================
bool Controller::CanFullScreen(void)
{
	return false;
}
//===========================================

