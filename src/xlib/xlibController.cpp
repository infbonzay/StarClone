
#include <grplib/grp.h>

#include "vars.h"
#include "const.h"
#include "mytime.h"
#include "version.hxx"
#include "LowMouse.h"
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
	unsigned long winatrmask;
	int borderSize = 5;
	if ( ( Surface->display = XOpenDisplay ( NULL ) ) == NULL )
	{
		DEBUGMESSCR("Cannot connect to xserver\n");
		return(0);
	}
	Surface->screenNr = DefaultScreen ( Surface->display );
	Surface->backgroundpixel = BlackPixel ( Surface->display, Surface->screenNr );
	Surface->FullScreen = fullscreen;
	Screen *defaultscreen = DefaultScreenOfDisplay(Surface->display);
	if (fullscreen)
	{
		//set screen resolution
		if (SetVideoMode<XF86VidModeModeInfo *>(x, y))
		{
			DEBUGMESSCR("Cannot set video mode\n");
			return(0);
		}
		borderSize = 0;
		//save current screen resolution
		Surface->SavedWidth  = defaultscreen->width;
		Surface->SavedHeight = defaultscreen->height;
	}
	Surface->DesiredBpp = bpp;
	Surface->SavedBpp = DefaultDepth(Surface->display, Surface->screenNr);
	Surface->window = XCreateSimpleWindow ( Surface->display,
											RootWindow ( Surface->display, Surface->screenNr ),
											0, 0, x, y, borderSize,
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
													ButtonReleaseMask |
													KeyPressMask |
													KeyReleaseMask |
													PointerMotionMask |
													VisibilityChangeMask |
													FocusChangeMask
													);
	XMapWindow ( Surface->display, Surface->window );

	XFlush(Surface->display);

	Surface->pixelsBufferSize = x * y * bpp / 8;
	Surface->pixels = wmalloc(Surface->pixelsBufferSize);
	SetVideoBuffer(Surface->pixels);
	gameconf.grmode.videobuff = (unsigned char *)Surface->pixels;
	gameconf.grmode.flags |= DISPLAYFLAGS_WINDOWACTIVE;
	Surface->palette = (unsigned char *)wmalloc(256 * 4);
	memset(Surface->palette, 0, 256 * 4);
	return(1 + (Surface->SavedBpp != bpp) );
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
	return;
	int to = from + count + 1;
	if (Surface->DesiredBpp != 8)
		return;
	memcpy(Surface->palette, pal4 + from * 4, count * 4);
	switch(Surface->SavedBpp)
	{
	case 8:
		//TODO need to update palette
		break;
	case 15:
		break;
	case 16:
		break;
	case 24:
		for (int i = 0; i < Surface->pixelsBufferSize; i++)
		{
			if (i >= from && i <= to)
			{
				Surface->Xpixels[i * 24 + 0] = Surface->palette[i * 4 + 0];
				Surface->Xpixels[i * 24 + 1] = Surface->palette[i * 4 + 1];
				Surface->Xpixels[i * 24 + 2] = Surface->palette[i * 4 + 2];
			}
		}
		break;
	case 32:
		break;
	default:
		break;
	}
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
		if (Surface->palette)
		{
			wfree(Surface->palette);
			Surface->palette = NULL;
		}
		if (Surface->FullScreen)
		{
			SetVideoMode<XF86VidModeModeInfo *>(Surface->SavedWidth,Surface->SavedHeight);
		}
		XCloseDisplay(Surface->display);
		Surface->display = NULL;
	}
}
//===========================================
int  Controller::EventsLoop(void)			//return 1 - on quit
{
	int UpperKeysActive;
	int keySym;
	uint8_t buttons;
	while(XPending(Surface->display))
	{
		XNextEvent(Surface->display,&Surface->event);
		//DEBUGMESSCR("%02x;\n",Surface->event.type);
		switch(Surface->event.type)
		{
		case FocusIn:
			gameconf.grmode.flags |= DISPLAYFLAGS_WINDOWACTIVE;
			break;
		case FocusOut:
			gameconf.grmode.flags &= DISPLAYFLAGS_WINDOWACTIVE;
			break;
		case Expose:
			break;
		case ButtonPress:
			buttons = 1 << (Surface->event.xbutton.button - 1);
			if (lowMouse.ClickEventFunc)
				(*lowMouse.ClickEventFunc)(true, buttons);
			break;
		case ButtonRelease:
			buttons = 1 << (Surface->event.xbutton.button - 1);
			if (lowMouse.ClickEventFunc)
				(*lowMouse.ClickEventFunc)(false, buttons);
			RefreshAtEnd = 1;
			break;
		case KeyPress:
			//DEBUGMESSCR("keypress state=%04x %d\n",Surface->event.xkey.state,Surface->event.xkey.keycode);
			UpperKeysActive = 0;
			if (Surface->event.xkey.state & ShiftMask)
				UpperKeysActive ^= 1;
			if (Surface->event.xkey.state & LockMask)
				UpperKeysActive ^= 1;
			keySym = XkbKeycodeToKeysym( Surface->display, Surface->event.xkey.keycode, 0, 0);
            if (UpperKeysActive)
			{
				if (keySym > 0 && keySym < 128)
				{
					KeyActive = SHIFTKEYS[keySym];
					if (!KeyActive)
						KeyActive = keySym;
				}
			}
			else
			{
				KeyActive = keySym;
			}
			if (KeyActive == ENTERKEY2)
			{
				KeyActive = ENTERKEY;
			}
			LastKey = KeyActive;
			KeysBuffer->PushElem(KeyActive);
			break;
		case KeyRelease:
			KeyActive = 0;
			break;
		case MotionNotify:
			if (lowMouse.MoveEventFunc)
				(*lowMouse.MoveEventFunc)(Surface->event.xmotion.x,Surface->event.xmotion.y);
			break;
		default:
			break;
		}
	}
	return(0);
}
//===========================================
template <typename T>
bool Controller::SetVideoMode(int x, int y)
{
    int modeCount;
    T *modes;
    T findMode = NULL;
	bool ok = false;
    if (XF86VidModeGetAllModeLines(Surface->display, Surface->screenNr, &modeCount, &modes))
    {
        for (int i = 0; i < modeCount; i++)
        {
            if (x == modes[i]->hdisplay && y == modes[i]->vdisplay)
				findMode = modes[i];
        }
		if (findMode == modes[0])
		{
			//current mode already active
			findMode = NULL;
			ok = true;
		}
    }
    if (findMode)
    {
		ok = XF86VidModeSwitchToMode(Surface->display, Surface->screenNr, findMode);
		if (ok)
		{
			XF86VidModeSetViewPort(Surface->display, Surface->screenNr, 0, 0);
			XFlush(Surface->display);
		}
    }
	XFree(modes);
	return(ok);
}
//===========================================
