
#include <time.h>
#include <SDL/SDL.h>

#include "mycycle.h"

#if !defined(_GRSDL_W)
#define _GRSDL_W 1

#define SCREEN_REGION	SDL_Rect

	int  installvectors(void);
	void uninstallvectors(void);
	int  changemode(int x,int y,int bpp,int fullscreen,char *pal);
	int  detectmode(int x,int y,int bpp,int fullscreen);
	void SetPitchAndChunk(void);
	void wscreenonregions(int nrregions,SCREEN_REGION regions[]);
	void wscreenonregion(int x,int y,int xsize,int ysize);
	void wscreenon(void);
	void activatepalette(char *pal4);
	void activatepalette256x3(unsigned char *pal3);
	void keyrefresh(void);
	void settextmode(void);
	int  eventwindowloop(void);
	int  setpalette(SDL_Color *rememberpal);
	int  setpalettecolors(SDL_Color rememberpal[],int fromcolor,int nrofcolors);
//==========================


extern	volatile short int keyactive,lastkey;
extern	int fullscreen;
extern	SDL_Surface *sdlsurface;
extern	int needrefreshatend;
extern	mycycle<uint16_t> keybuffer;
#endif

