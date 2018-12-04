
#include <grplib/grp.h>

#include "vars.h"
#include "const.h"
#include "mytime.h"
#include "version.hxx"
#include "Controller.h"


Controller mainController;
//==========================
int Controller::Init(void)
{
	Surface = new Controller_Surface();
	Surface->display = NULL;
	Surface->screenNr = 0;
	Surface->window = 0;
	Surface->pixels = NULL;

	mytimer.SetTickTimerFrequency(CYCLESPERSECOND);
	KeysBuffer = new mycycle<uint16_t>(16, MYCYCLE_INFINITE);
	return(0);
}
//===========================================
void Controller::DeInit(void)
{
	delete KeysBuffer;
	KeysBuffer = NULL;

	delete Surface;
	Surface = NULL;
}
//===========================================
int Controller::QueryVideoMode(int x, int y, int bpp, int fullscreen)
{
	XSetWindowAttributes setwinatr;
	XWindowAttributes winatr;
	int *depths,totaldepths;
	unsigned long winatrmask;
	if ( ( Surface->display = XOpenDisplay ( NULL ) ) == NULL )
	{
		DEBUGMESSCR("Cannot connect to xserver\n");
		return(0);
	}
	Surface->screenNr = DefaultScreen ( Surface->display );
	Surface->backgroundpixel = BlackPixel ( Surface->display, Surface->screenNr );
    //save current screen resolution
	Screen *defaultscreen = DefaultScreenOfDisplay(Surface->display);
    Surface->SavedWidth  = defaultscreen->width;
    Surface->SavedHeight = defaultscreen->height;
	Surface->SavedBpp = DefaultDepth(Surface->display, Surface->screenNr);
	//set screen resolution
	if (SetVideoMode(x, y) == -1)
	{
		DEBUGMESSCR("Cannot set video mode\n");
		return(0);
	}
	Surface->window = XCreateSimpleWindow ( Surface->display,
											RootWindow ( Surface->display, Surface->screenNr ),
											0, 0, x, y, 5,
											WhitePixel ( Surface->display, Surface->screenNr ),
											Surface->backgroundpixel);
	winatrmask = CWOverrideRedirect;
	if (fullscreen)
	{
		//create unmanagement video mode
		setwinatr.override_redirect = true;
		XChangeWindowAttributes(Surface->display, Surface->window, winatrmask, &setwinatr);
	}
	else
	{
		//call WM for parameters
	}
	XSelectInput(Surface->display,Surface->window,  ExposureMask | 
													ButtonPressMask	|
													ButtonReleaseMask
													);
	XMapWindow ( Surface->display, Surface->window );

	XFlush(Surface->display);

	Surface->pixels = wmalloc(x * y * bpp / 8);
	SetVideoBuffer(Surface->pixels);
	gameconf.grmode.videobuff = (unsigned char *)Surface->pixels;
	gameconf.grmode.flags |= DISPLAYFLAGS_WINDOWACTIVE;
	return(1 + (Surface.SavedBpp != bpp) );
}
//===========================================
int Controller::ModifyVideoMode(int x, int y, int bpp, int fullscreen, unsigned char *palette)
{
	int emul = 0;
	return(0);
}
//===========================================
void Controller::UpdateScreenRegions(int nrregions,SCREEN_REGION regions[])
{
	int nodraw = 1;
	//...
	//...
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
void Controller::KeyPressRefresh(void)
{
}
//===========================================
void Controller::QuitVideoMode(void)
{
	if (Surface->display)
	{
		if (Surface->pixels)
		{
			wfree(Surface->pixels);
			Surface->pixels = NULL;
		}
		SetVideoMode(Surface->SavedWidth,Surface->SavedHeight);
		XCloseDisplay(Surface->display);
		Surface->display = NULL;
	}
}
//===========================================
int  Controller::EventsLoop(void)			//return 1 - on quit
{
	While(XPending(Surface->display))
	{
		XNextEvent(Surface->display,&Surface->event);
		switch(Surface->event.type)
		{
		case ExposureMask:
			printf("exposed\n");
			break;
		case ButtonPressMask:
			printf("buttonpress\n");
			break;
		case ButtonReleaseMask: 
			printf("buttonrelease\n");
			break;
		case KeyPressMask:
			printf("keypress\n");
			break;
		case KeyReleaseMask:
			printf("keyrelease\n");
			break;
		case EnterWindowMask:
			printf("enterwindow\n");
			break;
		case LeaveWindowMask:
			printf("leavewindow\n");
			break;
		case PointerMotionMask:
			printf("pointermotion\n");
			break;
		case VisibilityChangeMask:
			printf("visibility\n");
			break;
		}
	}
	return(0);
}
//===========================================
int Controller::SetVideoMode(int x, int y)
{
    videoMode = FindVideoMode(x, y);
    if (videoMode != -1)
    {
        XF86VidModeSwitchToMode(display, screen, videoMode);
        XF86VidModeSetViewPort(display, screen, 0, 0);
        XFlush(display);
    }
	return(videoMode);
}
//===========================================
int Controller::FindVideoMode(int x, int y)
{
    int modeCount;
    XF86VidModeModeInfo **modes;

    if (XF86VidModeGetAllModeLines(Surface->display, Surface->screen, &modeCount, &modes))
    {
        for(int i = 0; i < modeCount; i++)
        {
            if (x == modes[i]->hdisplay && y == modes[i]->vdisplay)
				return i;
        }
        XFree(modes);
    }
    return -1;
}
//===========================================
