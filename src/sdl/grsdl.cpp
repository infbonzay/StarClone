#include <string.h>
//#include <ctype.h>

#include "version.hxx"
#include "mytime.h"
#include "mouse.h"
#include "maps.h"
#include "vars.h"
#include "debug.h"
#include <grplib/gr8.h>
#include <grplib/grp.h>
#include "mycycle.h"

#include "sdl/keysdl.h"
#include "sdl/grsdl.h"

#define MOUSEDBLCLICKTIME 8
mycycle<uint16_t> keybuffer = mycycle<uint16_t>(16,MYCYCLE_INFINITE);
const uint8_t SHIFTKEYS[128]={
		    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
		    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
		    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
		    '\x00','\x00',' '   ,'\x00','\x00','\x00','\x00','\x00','\x00','\x22',
		    '\x00','\x00','\x00','\x00','<'   ,'_'   ,'>'   ,'?'   ,')'   ,'!'   ,
		    '@'   ,'\x23','$'   ,'%'   ,'^'   ,'&'   ,'*'   ,'('   ,'\x00',':'   ,
		    '\x00','+'   ,'\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
		    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
		    '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
		    '\x00','{'   ,'|'   ,'}'   ,'\x00','\x00','~'   ,'A'   ,'B'   ,'C'   ,
		    'D'   ,'E'   ,'F'   ,'G'   ,'H'   ,'I'   ,'J'   ,'K'   ,'L'   ,'M'   ,
		    'N'   ,'O'   ,'P'   ,'Q'   ,'R'   ,'S'   ,'T'   ,'U'   ,'V'   ,'W'   ,
		    'X'   ,'Y'   ,'Z'   ,'\x00','\x00','\x00','\x00','\x00'
		    };

volatile short int keyactive,lastkey;
int 			ro;
int 			fullscreen = 0;
SDL_Surface 	*sdlsurface;
Uint8       	*keystatus;
int 			needrefreshatend;
int				dblclick;
//==========================
int setpalette(SDL_Color rememberpal[])
{
//    int err = SDL_SetColors(sdlsurface,rememberpal,0,256);
//    the 255 color is always RGB(255,255,255)

/*
    rememberpal[0].r=255;
    rememberpal[0].g=255;
    rememberpal[0].b=255;
    rememberpal[0].unused=0;
    rememberpal[255].r=255;
    rememberpal[255].g=255;
    rememberpal[255].b=255;
    rememberpal[255].unused=0;
*/
    return(setpalettecolors(rememberpal,0,256));
}
//==========================
int setpalettecolors(SDL_Color rememberpal[],int fromcolors,int nrofcolors)
{
    return(SDL_SetColors(sdlsurface,rememberpal,fromcolors,nrofcolors));
}
//==========================
int MyEventFunc(const SDL_Event *event)
{
    switch(event->type)
    {
	    case SDL_MOUSEMOTION:
	    case SDL_MOUSEBUTTONDOWN:
	    case SDL_MOUSEBUTTONUP:
	    case SDL_VIDEOEXPOSE:
	    case SDL_KEYUP:
	    case SDL_KEYDOWN:
	    case SDL_ACTIVEEVENT:
	    case SDL_QUIT:
	    case SDL_USEREVENT:
		return 1;
	    default:
		return(0);
    }
}
//===========================================
int installvectors(void)
{
    mytimer.SetTickTimerFrequency(CYCLESPERSECOND);
    return(0);
}
//==========================
void uninstallvectors(void)
{
//    SDL_FreeSurface(sdlsurface);
    SDL_Quit();
}
//==========================
void keyrefresh(void)
{
//    static int toggletofullscr=0;
    keystatus = SDL_GetKeyState(NULL);
/*    if (keystatus[SDLK_F4] == SDL_PRESSED && !toggletofullscr)
    {
        SDL_WM_ToggleFullScreen(sdlsurface);
        setpalette((SDL_Color *)map.palette);
	toggletofullscr = 1;
	fullscreen = fullscreen^1;
    }
    if (keystatus[SDLK_F4] != SDL_PRESSED)
	toggletofullscr = 0;
*/
}
//==========================
//==========================
int changemode(int x,int y,int bpp,int fullscreen,char *pal)
{
    int flags,realbpp,emul;
    char *tempvid = (char *)wmalloc(x*y);
    memcpy(tempvid,gameconf.grmode.videobuff,x*y);
    if (fullscreen)
        flags = SDL_HWSURFACE|SDL_FULLSCREEN|SDL_HWPALETTE;
    else
        flags = SDL_HWSURFACE|SDL_HWPALETTE;
    if (SDL_MUSTLOCK(sdlsurface))
        SDL_UnlockSurface(sdlsurface);

    realbpp = SDL_VideoModeOK(x,y, bpp, flags);
    if (realbpp == 0)
		return(0);
    if (realbpp != bpp)
    {
		emul = 1;
    }

    sdlsurface = SDL_SetVideoMode(x,y,bpp,flags);
    if ( sdlsurface == NULL )
		return(0);
    if (SDL_MUSTLOCK(sdlsurface))
        SDL_LockSurface(sdlsurface);
    SetVideoBuffer(sdlsurface->pixels);
    gameconf.grmode.videobuff = (unsigned char *)sdlsurface->pixels;
    memcpy(gameconf.grmode.videobuff,tempvid,x*y);
    if (pal)
		activatepalette(pal);
    if (!(pal && emul))
		wscreenon();
    wfree(tempvid);
    gameconf.grmode.flags |= DISPLAYFLAGS_WINDOWACTIVE;
    return(1+emul);
}
//==========================
int detectmode(int x,int y,int bpp,int fullscreen)
{
    char cap[200];
    int flags,realbpp,emul;
    if (fullscreen)
        flags = SDL_HWSURFACE|SDL_FULLSCREEN|SDL_HWPALETTE;
    else
        flags = SDL_HWSURFACE|SDL_HWPALETTE;
    emul = 0;
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)==-1)
        return (0);
    realbpp=SDL_VideoModeOK(x,y, bpp, flags);
    if (realbpp == 0)
		return(0);
    if (realbpp != bpp)
    {
		printf("Launching in emulated mode %dx%dx%d\n",x,y,realbpp);
		emul=1;
    }
    sdlsurface = SDL_SetVideoMode(x,y,bpp,flags);
    if ( sdlsurface == NULL )
		return(0);
    SDL_WarpMouse(0,0);
    int ver = GetGRPLibVer();
    sprintf(cap,GAMENAME " " GAMEVERSION " with GRPlib-%d.%d.%d createtime:" GAMECOMPILE,(ver>>16)&0xff,((ver>>8)&0xff),ver&0xff);
    SDL_WM_SetCaption(cap,cap);
    SDL_SetEventFilter(&MyEventFunc);
    SDL_ShowCursor(SDL_DISABLE);
    if (SDL_MUSTLOCK(sdlsurface))
        SDL_LockSurface(sdlsurface);
    SetVideoBuffer(sdlsurface->pixels);
    gameconf.grmode.videobuff = (unsigned char *)sdlsurface->pixels;
    gameconf.grmode.flags |= DISPLAYFLAGS_WINDOWACTIVE;
    return (1+emul);
}
//==========================
void SetPitchAndChunk(void)
{
        //pitch set in wscreenon
}
//==========================
void wscreenonregions(int nrregions,SCREEN_REGION regions[])
{
    int nodraw=1;
    mytimer.CallTimer(MYTIMER_SINCHRONMODE);
    if (gameconf.grmode.flags & DISPLAYFLAGS_WINDOWACTIVE)
    {
		SDL_UpdateRects(sdlsurface,nrregions,regions);
		nodraw = 0;
    }
    if (nodraw || gameconf.speedconf.cputhrottling)
        usleep(4000);
}
//==========================
void wscreenonregion(int x,int y,int xsize,int ysize)
{
    SDL_Rect scrpart;
    scrpart.x = x;
    scrpart.y = y;
    scrpart.w = xsize;
    scrpart.h = ysize;
    wscreenonregions(1,&scrpart);
}
//==========================
void wscreenon(void)
{
    wscreenonregion(0,0,gameconf.grmode.x,gameconf.grmode.y);
}
//==========================
void activatepalette(char *pal4)	//4*256 bytes palette
{
    setpalette((SDL_Color *)pal4);
}
//==========================
void activatepalette256x3(unsigned char *pal3)	//4*256 bytes palette
{
    int i;
    unsigned char pal4[4*256];
    unsigned char *pal4p = pal4;
    *pal4p++ = 0;
    *pal4p++ = 0;
    *pal4p++ = 0;
    *pal4p++ = 0;
    pal3+=3;
    for (i=1;i<255;i++)
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

    setpalette((SDL_Color *)&pal4);
}
//==========================
void settextmode(void)
{
    changemode(gameconf.grmode.x,gameconf.grmode.y,gameconf.grmode.s,0,NULL);
}
//==========================
int eventwindowloop(void) //return 1 - on quit
{
    static long long prevtimer = 0;
    int exitevent = 0;
    int UpperKeysActive;
    int doubleClick;
    SDL_Event event;
    uint8_t buttons;
    while ( SDL_PollEvent(&event))
    {
        switch (event.type)
        {
			case SDL_MOUSEMOTION:
				if (LowMouse.MoveEventFunc)
					(*LowMouse.MoveEventFunc)(event.motion.x,event.motion.y);
				break;
			case SDL_MOUSEBUTTONDOWN:
				doubleClick = 0;
				buttons = 1 << (event.button.button - 1);
				if (buttons == 1)
				{
					if (tick_timer - prevtimer <= MOUSEDBLCLICKTIME)
					{
						doubleClick = 1;
					}
					else
						doubleClick = 0;
					prevtimer = tick_timer;
					if (LowMouse.DblClickEventFunc)
						(*LowMouse.DblClickEventFunc)(doubleClick);
				}
				if (doubleClick)
					if (LowMouse.ClickEventFunc)
						(*LowMouse.ClickEventFunc)(buttons);
				//mouse_b |= buttons;
				break;
			case SDL_MOUSEBUTTONUP:
				if (LowMouse.DblClickEventFunc)
					(*LowMouse.DblClickEventFunc)(0);
				needrefreshatend = 1;
				break;
			case SDL_KEYDOWN:
				UpperKeysActive = 0;
				if (event.key.keysym.mod & KMOD_SHIFT)
					UpperKeysActive ^= 1;
				if (event.key.keysym.mod & KMOD_CAPS)
					UpperKeysActive ^= 1;
				//printf("SHIFT=%x CAPS=%x result=%x\n",event.key.keysym.mod & KMOD_SHIFT,event.key.keysym.mod & KMOD_CAPS,UpperKeysActive);
				if (UpperKeysActive)
				{
					if (event.key.keysym.sym > 0 && event.key.keysym.sym < 128)
					{
						keyactive = SHIFTKEYS[event.key.keysym.sym];
						if (!keyactive)
							keyactive = event.key.keysym.sym;
					}
				}
				else
					keyactive = event.key.keysym.sym;
				if (keyactive == ENTERKEY2)
					keyactive = ENTERKEY;
				lastkey = keyactive;
				keybuffer.PushElem(keyactive);
				break;
			case SDL_KEYUP:
				keyactive = 0;
				break;
			case SDL_ACTIVEEVENT:
				if ( event.active.state & SDL_APPACTIVE )
				{
					if ( event.active.gain )
						gameconf.grmode.flags |= DISPLAYFLAGS_WINDOWACTIVE;
					else
						gameconf.grmode.flags &= DISPLAYFLAGS_WINDOWACTIVE;
				}
				break;
			case SDL_QUIT:
				exitevent = 1;
				break;
		}
    }
    return (exitevent);
}
//===========================================
