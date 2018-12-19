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
	Display	*display = XOpenDisplay ( NULL );
	if ( display == NULL )
	{
		DEBUGMESSCR("Cannot connect to xserver\n");
		return(-1);
	}
	Surface = new Controller_Surface();
	Surface->display = display;
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

		XCloseDisplay(Surface->display);
		Surface->display = NULL;

		delete Surface;
		Surface = NULL;
	}
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
	unsigned long winatrmask, backgroundpixel;
	int xres,yres;
	Atom closedownAtom;
	XSizeHints hints;

	bool first = false;
	Surface->FullScreen = fullscreen;
	if (!Surface->window)
	{
		first = true;
		Surface->screenNr = DefaultScreen ( Surface->display );
		backgroundpixel = BlackPixel ( Surface->display, Surface->screenNr );
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

		gameconf.grmode.videobuff = Surface->pixels;

		SetVideoBuffer(Surface->pixels);
		Surface->XpixelsBufferSize = x * y * Surface->ximagebpp[(Surface->SavedBpp + 1)/8];
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
										x, y, Surface->ximagebpp[(Surface->SavedBpp+1)/8], 0);
		Surface->window = XCreateSimpleWindow ( Surface->display,
												RootWindow ( Surface->display, Surface->screenNr ),
												0, 0, x, y, 0,
												backgroundpixel,
												backgroundpixel);
		Surface->gc = XCreateGC(Surface->display, Surface->window, 0, NULL);
		XSelectInput(Surface->display,Surface->window,  ButtonPressMask	|
														ButtonReleaseMask |
														KeyPressMask |
														KeyReleaseMask |
														PointerMotionMask |
														StructureNotifyMask
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

		HideCursor();
	}
	winatrmask = 0;
	if (fullscreen)
	{
		//set screen resolution
		if (!SetVideoMode(x, y))
		{
			DEBUGMESSCR("Cannot set video mode\n");
			return(0);
		}
		winatrmask |= CWOverrideRedirect;
		setwinatr.override_redirect = true;

		XMoveResizeWindow(Surface->display, Surface->window, 0, 0, x, y);
	}
	else
	{
		if (!first)
		{
			DesktopResolution(&xres,&yres);
			if (!SetVideoMode(xres, yres))
			{
				DEBUGMESSCR("Cannot set video mode\n");
				return(0);
			}
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
	int xres,yres;
	if (Surface && Surface->display)
	{
		ShowCursor();
		if (Surface->gc)
		{
			XFreeGC(Surface->display, Surface->gc);
			Surface->gc = NULL;
		}
		if (Surface->window)
		{
			XDestroyWindow(Surface->display, Surface->window);
			XFlush(Surface->display);
			Surface->window = (Window) 0;
		}
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
			SetVideoMode(xres,yres);
		}
	}
}
//===========================================
int Controller::ModifyVideoMode(int x, int y, int bpp, int fullscreen, unsigned char *palette)
{
	int retvalue = 0;
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
	int keySym;
	int keymod;
	uint8_t buttons;
	while(XPending(Surface->display))
	{
		XNextEvent(Surface->display,&Surface->event);
		//DEBUGMESSCR("%02x;\n",Surface->event.type);
		switch(Surface->event.type)
		{
		case MapNotify:
			gameconf.grmode.flags |= DISPLAYFLAGS_WINDOWACTIVE;
			break;
		case UnmapNotify:
			gameconf.grmode.flags &= ~DISPLAYFLAGS_WINDOWACTIVE;
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
			keymod = 0;
            keySym = XkbKeycodeToKeysym( Surface->display, Surface->event.xkey.keycode, 0, 0);
			SetKeyMod(keySym,true);
			if (Surface->event.xkey.state & LockMask)
				keymod ^= 1;
			if (KeyFlags & SHIFTKEYMASK)
				keymod ^= 1;
			if (keymod)
			{
				if (keySym > 0 && keySym < 128)
				{
					KeyActive = SHIFTKEYS[keySym];
					if (!KeyActive)
						KeyActive = keySym;
				}
				else
				{
					KeyActive = 0;
				}
			}
			else
			{
				KeyActive = keySym;
			}
			LastKey = KeyActive;
			if (KeyActive)
				KeysBuffer->PushElem(KeyActive);
			break;
		case KeyRelease:
            keySym = XkbKeycodeToKeysym( Surface->display, Surface->event.xkey.keycode, 0, 0);
			SetKeyMod(keySym,false);
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
			//DEBUGMESSCR("not implemented %02x;\n",Surface->event.type);
			break;
		}
	}
	return(0);
}
//===========================================
void Controller::SetKeyMod(int keysym,bool status)
{
	switch(keysym)
	{
	case XK_Shift_L:
	case XK_Shift_R:
		if (status)
			KeyFlags |= SHIFTKEYMASK;
		else
			KeyFlags &= ~SHIFTKEYMASK;
		break;
	case XK_Control_L:
	case XK_Control_R:
		if (status)
			KeyFlags |= CTRLKEYMASK;
		else
			KeyFlags &= ~CTRLKEYMASK;
		break;
	}
}
//===========================================
bool Controller::SetVideoMode(int x, int y)
{
	int modeCount;
	XF86VidModeModeInfo **modes;
    bool ok = false;
	if (XF86VidModeGetAllModeLines(Surface->display, Surface->screenNr, &modeCount, &modes))
	{
    	for (int i = 0; i < modeCount; i++)
		{
            if (x == modes[i]->hdisplay && y == modes[i]->vdisplay)
            {
				ok = XF86VidModeSwitchToMode(Surface->display, Surface->screenNr, modes[i]);
				if (ok)
				{
					XF86VidModeSetViewPort(Surface->display, Surface->screenNr, 0, 0);
				}
				break;
			}
        }
	}
	XFree(modes);
	return(ok);
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
	int deltaincr = Surface->Ximage->width - sizex;
	T *paladr = (T *)Surface->palette;
	T *Xbuf = (T *)Surface->Xpixels + y * Surface->Ximage->width + x;
	uint8_t *buf = Surface->pixels + y * Surface->Ximage->width + x;
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
	//return;
	static char noData[] = { 0,0,0,0,0,0,0,0 };
	Pixmap bitmapNoData;
	Cursor invisibleCursor;
	XColor black;

	if (!Surface->noCursor)
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
bool Controller::CanFullScreen(void)
{
	return true;
}
//===========================================
