
#include <SDL/SDL.h>
#include <grplib/grp.h>

#include "vars.h"
#include "const.h"
#include "mytime.h"
#include "version.hxx"
#include "keysdl.h"
#include "Controller.h"

Controller mainController;
//==========================
int MyEventFunc(const SDL_Event *event)
{
	switch (event->type)
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
int Controller::Init(void)
{
	mytimer.SetTickTimerFrequency(CYCLESPERSECOND);
	KeysBuffer = new mycycle<uint16_t>(16, MYCYCLE_INFINITE);
	return(0);
}
//===========================================
void Controller::DeInit(void)
{
	delete KeysBuffer;
	KeysBuffer = NULL;
}
//===========================================
int Controller::HideCursor(void)
{
	SDL_ShowCursor(SDL_DISABLE);
}
//===========================================
int Controller::ShowCursor(void)
{
	SDL_ShowCursor(SDL_Enable;
}
//===========================================
int Controller::SetWindowName(const char *winName)
{
	int ver = GetGRPLibVer();
	SDL_WM_SetCaption(winName, winName);
}
//===========================================
int Controller::QueryVideoMode(int x, int y, int bpp, int fullscreen)
{
	int flags, realbpp, emul;
	if (fullscreen)
		flags = SDL_HWSURFACE | SDL_FULLSCREEN | SDL_HWPALETTE;
	else
		flags = SDL_HWSURFACE | SDL_HWPALETTE;
	emul = 0;
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1)
		return (0);
	realbpp = SDL_VideoModeOK(x, y, bpp, flags);
	if (realbpp == 0)
		return(0);
	if (realbpp != bpp)
	{
		emul = 1;
		printf("Launching in emulated mode %dx%dx%d\n", x, y, realbpp);
	}
	Surface = SDL_SetVideoMode(x, y, bpp, flags);
	if (Surface == NULL)
		return(0);
	SDL_WarpMouse(0, 0);
//	int ver = GetGRPLibVer();
//	sprintf(cap, GAMENAME " " GAMEVERSION " with GRPlib-%d.%d.%d createtime:" GAMECOMPILE, (ver >> 16) & 0xff, ((ver >> 8) & 0xff), ver & 0xff);
//	SDL_WM_SetCaption(cap, cap);
	SDL_SetEventFilter(&MyEventFunc);
	HideCursor();
	if (SDL_MUSTLOCK(Surface))
		SDL_LockSurface(Surface);
	SetVideoBuffer(Surface->pixels);
	gameconf.grmode.videobuff = (unsigned char *)Surface->pixels;
	gameconf.grmode.flags |= DISPLAYFLAGS_WINDOWACTIVE;
	return (1 + emul);
}
//===========================================
int Controller::ModifyVideoMode(int x, int y, int bpp, int fullscreen, unsigned char *palette)
{
	int flags, realbpp, emul = 0;
	char *tempvid = (char *)wmalloc(x * y);
	memcpy(tempvid, gameconf.grmode.videobuff, x * y);
	if (fullscreen)
		flags = SDL_HWSURFACE | SDL_FULLSCREEN | SDL_HWPALETTE;
	else
		flags = SDL_HWSURFACE | SDL_HWPALETTE;
	if (SDL_MUSTLOCK(Surface))
		SDL_UnlockSurface(Surface);

	realbpp = SDL_VideoModeOK(x, y, bpp, flags);
	if (realbpp == 0)
		return(0);
	if (realbpp != bpp)
		emul = 1;

	Surface = SDL_SetVideoMode(x, y, bpp, flags);
	if (Surface == NULL)
		return(0);
	if (SDL_MUSTLOCK(Surface))
		SDL_LockSurface(Surface);
	SetVideoBuffer(Surface->pixels);
	gameconf.grmode.videobuff = (unsigned char *)Surface->pixels;
	memcpy(gameconf.grmode.videobuff, tempvid, x*y);
	if (palette)
		ApplyPalette(palette);
	if (!(palette && emul))
		UpdateScreen();
	wfree(tempvid);
	gameconf.grmode.flags |= DISPLAYFLAGS_WINDOWACTIVE;
	return(1 + emul);
}
//===========================================
void Controller::UpdateScreenRegions(int nrregions,SCREEN_REGION regions[])
{
	int nodraw = 1;
	mytimer.CallTimer(MYTIMER_SINCHRONMODE);
	if (gameconf.grmode.flags & DISPLAYFLAGS_WINDOWACTIVE)
	{
		SDL_UpdateRects(Surface, nrregions, regions);
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
	SDL_SetColors(Surface, (SDL_Color *) pal4, from, count);
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
	KeysStatus = SDL_GetKeyState(NULL);
}
//===========================================
void Controller::QuitVideoMode(void)
{
	//ModifyVideoMode(gameconf.grmode.x,gameconf.grmode.y,gameconf.grmode.s,0,NULL);
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_Quit();
}
//===========================================
int  Controller::EventsLoop(void)			//return 1 - on quit
{
	int exitevent = 0;
	int UpperKeysActive;
	SDL_Event event;
	uint8_t buttons;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_MOUSEMOTION:
			highMouse->PosX = event.motion.x;
			highMouse->PosY = event.motion.y;
			if (highMouse->MoveFunc)
				(*highMouse->MoveFunc)(event.motion.x, event.motion.y);
			break;
		case SDL_MOUSEBUTTONDOWN:
			buttons = 1 << (event.button.button - 1);
			if (highMouse->ClickFunc)
				(*highMouse->ClickFunc)(true, buttons);
			break;
		case SDL_MOUSEBUTTONUP:
			buttons = 1 << (event.button.button - 1);
			if (highMouse->ClickFunc)
				(*highMouse->ClickFunc)(false, buttons);
			RefreshAtEnd = 1;
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
					KeyActive = SHIFTKEYS[event.key.keysym.sym];
					if (!KeyActive)
						KeyActive = event.key.keysym.sym;
				}
			}
			else
				KeyActive = event.key.keysym.sym;
			if (KeyActive == ENTERKEY2)
				KeyActive = ENTERKEY;
			LastKey = KeyActive;
			KeysBuffer->PushElem(KeyActive);
			break;
		case SDL_KEYUP:
			KeyActive = 0;
			break;
		case SDL_ACTIVEEVENT:
			if (event.active.state & SDL_APPACTIVE)
			{
				if (event.active.gain)
					gameconf.grmode.flags |= DISPLAYFLAGS_WINDOWACTIVE;
				else
					gameconf.grmode.flags &= ~DISPLAYFLAGS_WINDOWACTIVE;
			}
			break;
		case SDL_QUIT:
			exitevent = 1;
			break;
		}
	}
	return (exitevent);
}
