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

		Surface->SavedWidth = Surface->vinfo.xres;
		Surface->SavedHeight = Surface->vinfo.yres;
		Surface->SavedBpp = Surface->vinfo.bits_per_pixel;
		Surface->DesiredBpp = bpp;
		Surface->width = x;
		Surface->heigth = y;

		Surface->FBpixelsBufferSize = Surface->SavedWidth * Surface->SavedHeight * Surface->SavedBpp / 8;

		Surface->fbp = mmap(0, Surface->FBpixelsBufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, Surface->fbfd, 0);
		if ((long)Surface->fbp == -1)
		{
			DEBUGMESSCR("Error: failed to map framebuffer device to memory");
			return(0);
		}
		Surface->FBpixels = (uint8_t *) Surface->fbp;
		Surface->FBpixelsBufferSize = x * y * Surface->ximagebpp[(Surface->SavedBpp + 1)/8];

		
		Surface->pixelsBufferSize = x * y * bpp / 8;
		Surface->pixels = (uint8_t *) wmalloc(Surface->pixelsBufferSize);

		Surface->palette = (uint8_t *)wmalloc(256 * 4);
		memset(Surface->palette, 0, 256 * 4);

		gameconf.grmode.videobuff = Surface->pixels;

		GRP_SetVideoBuffer(Surface->pixels);


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
			munmap(Surface->fbp, Surface->FBpixelsBufferSize);
			Surface->FBpixels = NULL;
		}
		if (Surface->fbfd)
		{
			close(Surface->fbfd);
			Surface->fbfd = 0;
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
	int i, nodraw = 1;
	mytimer.CallTimer(MYTIMER_SINCHRONMODE);
	if (gameconf.grmode.flags & DISPLAYFLAGS_WINDOWACTIVE)
	{
		for (i = 0; i < nrregions; i++)
		{
			TransformPixels(regions[i].x, regions[i].y, regions[i].w, regions[i].h);
		}
		nodraw = 0;
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
	if (Surface->DesiredBpp != 8)
		return;
	uint16_t *palette16 = ( (uint16_t *) Surface->palette) + from;
	uint16_t palcol16;
	uint32_t *palette32 = ( (uint32_t *) Surface->palette) + from;
	uint32_t palcol32;
	switch(Surface->SavedBpp)
	{
	case 8:
		return;
	case 15:
		do{
			palcol16 = (( (*pal4++) >> 3) << 10) | (( (*pal4++) >> 3) << 5) | (( (*pal4++) >> 3) );
			pal4++;
			*palette16++ = palcol16;
		}while(--count);
		break;
	case 16:
		do{
			palcol16 = (( (*pal4++) >> 3) << 11) | (( (*pal4++) >> 2) << 5) | (( (*pal4++) >> 3) );
			pal4++;
			*palette16++ = palcol16;
		}while(--count);
		break;
	case 24:
		do{
			palcol32 = ( (*pal4++) << 16) | ( (*pal4++) << 8) | ( (*pal4++) );
			pal4++;
			*palette32++ = palcol32;
		}while(--count);
		break;
	}
	UpdateScreen();
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
	if (Surface->DesiredBpp != 8)
		return;
	switch (Surface->SavedBpp)
	{
	case 8:
		return;
	case 15:
	case 16:
		Transform<uint16_t>(x, y, sizex, sizey);
		break;
	case 24:
	case 32:
		Transform<uint32_t>(x, y, sizex, sizey);
		break;
	default:
		DEBUGMESSCR("Transformation from %d bpp to %d bpp not implemented\n", Surface->DesiredBpp, Surface->SavedBpp);
		return;
	}
}
//===========================================
template <typename T>
	void Controller::Transform(int x, int y, int sizex, int sizey)
{
	int i, j;
	int deltaincr = Surface->width - sizex;
	T *paladr = (T *)Surface->palette;
	T *Xbuf = (T *)Surface->FBpixels + y * Surface->width + x;
	uint8_t *buf = Surface->pixels + y * Surface->width + x;
	i = sizey;
	do{
		j = sizex;
		do{
			*Xbuf++ = paladr[*buf++];
		}while(--j);
		Xbuf += deltaincr;
		buf += deltaincr;
	}while(--i);
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

