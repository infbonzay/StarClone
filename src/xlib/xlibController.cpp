#include <grplib/grp.h>

#include "vars.h"
#include "const.h"
#include "mytime.h"
#include "version.hxx"
#include "LowMouse.h"
#include "rand.h"
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
	Surface->Xpixels = NULL;
	Surface->pixelsBufferSize = 0;
	Surface->palette = NULL;
	Surface->flags = 0x00;
	Surface->Ximage = NULL;

	mytimer.SetTickTimerFrequency(CYCLESPERSECOND);
	KeysBuffer = new mycycle<uint16_t>(16, MYCYCLE_INFINITE);
	KeysStatus = (uint8_t *) wmalloc(256);
	memset(KeysStatus,0,256);
	return(0);
}
//===========================================
void Controller::DeInit(void)
{
	if (KeysStatus)
	{
		wfree(KeysStatus);
		KeysStatus = 0;
	}
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
	Visual *xVisual = DefaultVisual(Surface->display, 0);
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
	if (bpp == Surface->SavedBpp)
	{
		Surface->flags |= CFLAG_EXACTBPP;
	}
/*
	int items,a;
	XVisualInfo *vi;
	XVisualInfo vtemplate;
	if ((vi = XGetVisualInfo(Surface->display,VisualNoMask,&vtemplate,&items)))
	{
		for (int i=0;i<items;i++)
		{
			a = items + 1;
		}
	}
*/
	Surface->window = XCreateSimpleWindow ( Surface->display,
											RootWindow ( Surface->display, Surface->screenNr ),
											0, 0, x, y, borderSize,
											WhitePixel ( Surface->display, Surface->screenNr ),
											Surface->backgroundpixel);
	winatrmask = 0;
	if (fullscreen)
	{
		//create unmanagement video mode
		winatrmask |= CWOverrideRedirect;
		setwinatr.override_redirect = true;
	}
	winatrmask |= CWBackingStore;
	setwinatr.backing_store = NotUseful;
	if (winatrmask)
		XChangeWindowAttributes(Surface->display, Surface->window, winatrmask, &setwinatr);
	//TODO call WM for parameters
	
	Surface->gc = XCreateGC(Surface->display, Surface->window, 0, NULL);
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
	HideCursor();
	XFlush(Surface->display);

	Surface->pixelsBufferSize = x * y * Surface->DesiredBpp / 8;
	Surface->pixels = (uint8_t *) wmalloc(Surface->pixelsBufferSize);
	SetVideoBuffer(Surface->pixels);
	gameconf.grmode.videobuff = (unsigned char *)Surface->pixels;
	gameconf.grmode.flags |= DISPLAYFLAGS_WINDOWACTIVE;
	Surface->palette = (uint8_t *)wmalloc(256 * 4);
	memset(Surface->palette, 0, 256 * 4);

	Surface->XpixelsBufferSize = x * y * 4;
	if (Surface->flags & CFLAG_EXACTBPP)
		Surface->Xpixels = Surface->pixels;
	else
	{
		Surface->Xpixels = (uint8_t *) wmalloc(Surface->XpixelsBufferSize);
		printf("Launching in emulated mode %dx%dx%d\n", x, y, Surface->SavedBpp);
	}
	Surface->Ximage = XCreateImage( Surface->display, xVisual, Surface->SavedBpp,
									ZPixmap, 0, (char *)Surface->Xpixels,
									x, y, 32, 0);
	return(1 + ((Surface->flags & CFLAG_EXACTBPP) != CFLAG_EXACTBPP));
}
//===========================================
void Controller::QuitVideoMode(void)
{
	if (Surface->display)
	{
		if (Surface->palette)
		{
			wfree(Surface->palette);
			Surface->palette = NULL;
		}
		if (Surface->FullScreen)
		{
			SetVideoMode<XF86VidModeModeInfo *>(Surface->SavedWidth,Surface->SavedHeight);
		}
		Surface->Xpixels = NULL;
		if (Surface->flags & CFLAG_EXACTBPP)
		{
			Surface->pixels = NULL;
		}
		if (Surface->Ximage)
		{
			XDestroyImage(Surface->Ximage);		//also this function deallocate XImage->data aka Surface->Xpixels
												// on 8 bpp this equals with Surface->pixels
			Surface->Ximage = NULL;
		}
		if (Surface->pixels)
		{
			wfree(Surface->pixels);
			Surface->pixels = NULL;
		}
		if (Surface->gc)
		{
			XFreeGC(Surface->display, Surface->gc);
			Surface->gc = NULL;
		}
		XCloseDisplay(Surface->display);
		Surface->display = NULL;
	}
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
	int i, nodraw = 1;
	mytimer.CallTimer(MYTIMER_SINCHRONMODE);
	if (gameconf.grmode.flags & DISPLAYFLAGS_WINDOWACTIVE)
	{
		for (i = 0; i < nrregions; i++)
		{
			TransformPixels(regions[i].x, regions[i].y, regions[i].w, regions[i].h);
			XPutImage(Surface->display, Surface->window, Surface->gc, Surface->Ximage,
                      regions[i].x, regions[i].y, regions[i].x, regions[i].y,
                      regions[i].w, regions[i].h);
		}
		nodraw = 0;
		XFlush(Surface->display);
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
	int to = (from + count) * 4;
	if (Surface->DesiredBpp != 8)
		return;
	for (int i = from ; i < to; i+=4)
	{
		Surface->palette[i+2] = pal4[i+0];
		Surface->palette[i+1] = pal4[i+1];
		Surface->palette[i+0] = pal4[i+2];
		//Surface->palette[i+3] = pal4[i+3];
	}
	UpdateScreen();
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
void Controller::KeyPressRefresh(void)
{
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
		XF86VidModeLockModeSwitch( Surface->display, Surface->window, 0 );
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
void Controller::TransformPixels(int x, int y, int sizex, int sizey)
{
	switch (Surface->SavedBpp)
	{
	case 8:
		return;
	case 24:
		Transform32(x, y, sizex, sizey);
		break;
	default:
		DEBUGMESSCR("Transformation from %d bpp to %d bpp not implemented\n", Surface->DesiredBpp, Surface->SavedBpp);
		return;
	}
}
//===========================================
void Controller::Transform32(int x, int y, int sizex, int sizey)
{
	int i, j, Xpixel;
	int xlast = x + sizex;
	int ylast = y + sizey;
	int deltaincr = Surface->Ximage->width - sizex;
	uint8_t pixel;
	uint32_t *palette4bytes = (uint32_t *)Surface->palette;
	uint32_t *Xbuf = (uint32_t *)Surface->Xpixels;
	uint8_t *buf = Surface->pixels;
	Xbuf += y * Surface->Ximage->width + x;
	buf +=  y * Surface->Ximage->width + x;
	for (i = y;i < ylast; i++)
	{
		for (j = x;j < xlast; j++)
		{
			*Xbuf = palette4bytes[*buf];
			Xbuf++;
			buf++;
		}
		Xbuf += deltaincr;
		buf += deltaincr;
	}
}
//===========================================
void Controller::HideCursor(void)
{
	return;
	static char noData[] = { 0,0,0,0,0,0,0,0 };
	Pixmap bitmapNoData;
	Cursor invisibleCursor;
	XColor black {0, 0, 0};

	bitmapNoData = XCreateBitmapFromData(Surface->display, Surface->window, noData, 8, 8);
	invisibleCursor = XCreatePixmapCursor(Surface->display, bitmapNoData, bitmapNoData, 
                                     	  &black, &black, 0, 0);
	XDefineCursor(Surface->display, Surface->window, invisibleCursor);	
	XFreeCursor(Surface->display, invisibleCursor);
	XFreePixmap(Surface->display, bitmapNoData);
}
//===========================================
void Controller::ShowCursor(void)
{
	return;
	Cursor cursor;
	cursor = XCreateFontCursor(Surface->display, XC_left_ptr);
	XDefineCursor(Surface->display, Surface->window, cursor);
	XFreeCursor(Surface->display, cursor);
	XUndefineCursor(Surface->display, Surface->window);
	sleep(5);
}
//===========================================
