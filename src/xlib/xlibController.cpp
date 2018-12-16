#include <grplib/grp.h>

#include "vars.h"
#include "const.h"
#include "mytime.h"
#include "version.hxx"
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
void Controller::SetWindowName(const char *winName)
{
    XStoreName(Surface->display, Surface->window, winName);
}
//===========================================
int Controller::QueryVideoMode(int x, int y, int bpp, int fullscreen)
{
    XWindowAttributes winatr;
	XSetWindowAttributes setwinatr;
	unsigned long winatrmask;
	int xres,yres;
	Atom closedownAtom;
	XSizeHints hints;

	bool first = false;
	Surface->FullScreen = fullscreen;
	if (!Surface->display)
	{
		first = true;
		if ( ( Surface->display = XOpenDisplay ( NULL ) ) == NULL )
		{
			DEBUGMESSCR("Cannot connect to xserver\n");
			return(0);
		}
		Surface->screenNr = DefaultScreen ( Surface->display );
		Surface->backgroundpixel = BlackPixel ( Surface->display, Surface->screenNr );
		Screen *defaultscreen = DefaultScreenOfDisplay(Surface->display);
		Surface->XVisual = DefaultVisual(Surface->display, 0);

		//save current screen resolution
		SaveDesktopResolution(defaultscreen->width,defaultscreen->height);

		Surface->DesiredBpp = bpp;
		Surface->SavedBpp = DefaultDepth(Surface->display, Surface->screenNr);

		Surface->pixelsBufferSize = x * y * bpp / 8;
		Surface->pixels = (uint8_t *) wmalloc(Surface->pixelsBufferSize);

		Surface->palette = (uint8_t *)wmalloc(256 * 4);
		memset(Surface->palette, 0, 256 * 4);

		gameconf.grmode.videobuff = (unsigned char *)Surface->pixels;

		SetVideoBuffer(Surface->pixels);
		Surface->XpixelsBufferSize = x * y * 4;
		if (Surface->flags & CFLAG_EXACTBPP)
		{
			Surface->Xpixels = Surface->pixels;
		}
		else
		{
			Surface->Xpixels = (uint8_t *) wmalloc(Surface->XpixelsBufferSize);
			printf("Launching in emulated mode %dx%dx%d\n", x, y, Surface->SavedBpp);
		}
		Surface->Ximage = XCreateImage( Surface->display, Surface->XVisual, Surface->SavedBpp,
										ZPixmap, 0, (char *)Surface->Xpixels,
										x, y, 32, 0);
		Surface->window = XCreateSimpleWindow ( Surface->display,
												RootWindow ( Surface->display, Surface->screenNr ),
												0, 0, x, y, 0,
												Surface->backgroundpixel,
												Surface->backgroundpixel);
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

		if ( (closedownAtom = XInternAtom(Surface->display, "WM_DELETE_WINDOW", False)) )
			XSetWMProtocols(Surface->display, Surface->window, &closedownAtom, 1);

		hints.flags = USSize | PMinSize | PMaxSize;
		hints.min_width = x;
		hints.min_height = y;
		hints.max_width = x;
		hints.max_height = y;
		hints.base_width = x;
		hints.base_height = y;

	    XSetWMNormalHints(Surface->display, Surface->window, &hints);

		//HideCursor();
	}
	winatrmask = 0;
	if (fullscreen)
	{
		//set screen resolution
		if (!SetVideoMode<XF86VidModeModeInfo *>(x, y))
		{
			DEBUGMESSCR("Cannot set video mode\n");
			return(0);
		}
		//create unmanagement video mode
		winatrmask |= CWOverrideRedirect;
		setwinatr.override_redirect = true;
		
		XMoveResizeWindow(Surface->display, Surface->window, 0, 0, x, y);
	}
	else
	{
		if (!first)
		{
			DesktopResolution(&xres,&yres);
			if (!SetVideoMode<XF86VidModeModeInfo *>(xres, yres))
			{
				DEBUGMESSCR("Cannot set video mode\n");
				return(0);
			}
			//create unmanagement video mode
			winatrmask |= CWOverrideRedirect;
			setwinatr.override_redirect = false;
			
		}
	}
	winatrmask |= CWBackingStore;
	setwinatr.backing_store = NotUseful;
	if (winatrmask)
	{
		XChangeWindowAttributes(Surface->display, Surface->window, winatrmask, &setwinatr);
	}

	XMapWindow ( Surface->display, Surface->window );
	XFlush(Surface->display);

	XGetWindowAttributes(Surface->display,Surface->window,&winatr);
    if (winatr.map_state == IsViewable )
    { 
		XSetInputFocus(Surface->display, Surface->window, RevertToParent, CurrentTime);
    }
	gameconf.grmode.flags |= DISPLAYFLAGS_WINDOWACTIVE;
	return(1 + ((Surface->flags & CFLAG_EXACTBPP) != CFLAG_EXACTBPP));
}
//===========================================
void Controller::QuitVideoMode(void)
{
	QuitVideoMode(true);
}
//===========================================
void Controller::QuitVideoMode(bool disconnectFromServer)
{
	int xres,yres;
	if (Surface->display)
	{
		if (disconnectFromServer)
		{
			if (Surface->gc)
			{
				XFreeGC(Surface->display, Surface->gc);
				Surface->gc = NULL;
			}
			XDestroyWindow(Surface->display, Surface->window);
			XFlush(Surface->display);
			Surface->window = (Window) 0;
			ShowCursor();
			if (Surface->palette)
			{
				wfree(Surface->palette);
				Surface->palette = NULL;
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
			if (Surface->FullScreen)
			{
				DesktopResolution(&xres,&yres);
				SetVideoMode<XF86VidModeModeInfo *>(xres,yres);
			}
			XCloseDisplay(Surface->display);
			Surface->display = NULL;
		}
	}
}
//===========================================
int Controller::ModifyVideoMode(int x, int y, int bpp, int fullscreen, unsigned char *palette)
{
	int retvalue = 0;
	//if (fullscreen != Surface->FullScreen)
	{
		XUnmapWindow ( Surface->display, Surface->window );
		retvalue = QueryVideoMode(x,y,bpp,fullscreen);
		if (retvalue)
		{
			if (palette)
				ApplyPalette(palette);
			if (palette && (Surface->flags & CFLAG_EXACTBPP))
				UpdateScreen();
			gameconf.grmode.flags |= DISPLAYFLAGS_WINDOWACTIVE;
		}
	}
	return retvalue;
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
			gameconf.grmode.flags &= ~DISPLAYFLAGS_WINDOWACTIVE;
			break;
		case Expose:
			break;
		case MotionNotify:
			highMouse->PosX = Surface->event.xmotion.x;
			highMouse->PosY = Surface->event.xmotion.y;
			if (highMouse->MoveFunc)
				(*highMouse->MoveFunc)(Surface->event.xmotion.x,Surface->event.xmotion.y);
			break;
		case ButtonPress:
			buttons = 1 << (Surface->event.xbutton.button - 1);
			if (highMouse->ClickFunc)
				(*highMouse->ClickFunc)(true, buttons);
			break;
		case ButtonRelease:
			buttons = 1 << (Surface->event.xbutton.button - 1);
			if (highMouse->ClickFunc)
				(*highMouse->ClickFunc)(false, buttons);
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
		case ClientMessage:
/*			if (((XClientMessageEvent *) & event)->data.l[0] == closedownAtom) 
			{
		    	//close window
				return(1);
	    	}
*/
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
            {
				findMode = modes[i];
				break;
			}
        }
	}
	if (findMode)
	{
		ok = XF86VidModeSwitchToMode(Surface->display, Surface->screenNr, findMode);
		if (ok)
		{
			XF86VidModeSetViewPort(Surface->display, Surface->screenNr, 0, 0);
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
	//return;
	static char noData[] = { 0,0,0,0,0,0,0,0 };
	Pixmap bitmapNoData;
	Cursor invisibleCursor;
	XColor black;

	if (Surface->noCursor)
	{
		bitmapNoData = XCreateBitmapFromData(Surface->display, Surface->window, noData, 8, 8);
		Surface->noCursor = XCreatePixmapCursor(Surface->display, bitmapNoData, bitmapNoData,
                                     	  		&black, &black, 0, 0);
		XDefineCursor(Surface->display, Surface->window, Surface->noCursor);
		XFreePixmap(Surface->display, bitmapNoData);
	}
}
//===========================================
void Controller::ShowCursor(void)
{
	if (Surface->noCursor)
	{
		XFreeCursor(Surface->display, Surface->noCursor);
		Surface->noCursor = (Cursor) 0;
		XFlush(Surface->display);
	}
}
//===========================================
void Controller::SaveDesktopResolution(int x,int y)
{
	Surface->SavedWidth  = x;
	Surface->SavedHeight = y;
}
//===========================================
void Controller::DesktopResolution(int *x,int *y)
{
	*x = Surface->SavedWidth;
	*y = Surface->SavedHeight;
}
//===========================================
