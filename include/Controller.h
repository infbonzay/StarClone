#ifndef _CONTROLLER_W

#include <stdint.h>
#include "mycycle.h"

#define _CONTROLLER_W

#ifdef WITHSDL
	#include <SDL/SDL.h>
	#define Controller_Surface 	SDL_Surface
	#define SCREEN_REGION		SDL_Rect
#endif

#define KEYPRESS(ikey) (mainController.KeysStatus[ikey] == KEYPRESSED)


class Controller
{
	const uint8_t SHIFTKEYS[128] =
	{
		'\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
		'\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
		'\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
		'\x00','\x00',' '	,'\x00','\x00','\x00','\x00','\x00','\x00','\x22',
		'\x00','\x00','\x00','\x00','<'	  ,'_'	 ,'>'	,'?'   ,')'	  ,'!'	 ,
		'@'	  ,'\x23','$'	,'%'   ,'^'	  ,'&'	 ,'*'	,'('   ,'\x00',':'	 ,
		'\x00','+'	 ,'\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
		'\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
		'\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00',
		'\x00','{'	 ,'|'	,'}'   ,'\x00','\x00','~'	,'A'   ,'B'	  ,'C'	 ,
		'D'	  ,'E'	 ,'F'	,'G'   ,'H'	  ,'I'	 ,'J'	,'K'   ,'L'	  ,'M'	 ,
		'N'	  ,'O'	 ,'P'	,'Q'   ,'R'	  ,'S'	 ,'T'	,'U'   ,'V'	  ,'W'	 ,
		'X'	  ,'Y'	 ,'Z'	,'\x00','\x00','\x00','\x00','\x00'
	};

	int 				Fullscreen;
	Controller_Surface	*Surface;

public:
	short int 			KeyActive,LastKey;
	int 				RefreshAtEnd;
	uint8_t				*KeysStatus;
	mycycle<uint16_t> 	*KeysBuffer;

	int Init();
	void DeInit();
	int  QueryVideoMode(int x, int y, int bpp, int fullscreen);
	int  ModifyVideoMode(int x, int y, int bpp, int fullscreen,unsigned char *palette);

	void UpdateScreenRegions(int nrregions,SCREEN_REGION regions[]);
	void UpdateScreenRegion(int x,int y,int xsize,int ysize);
	void UpdateScreen(void);
	void ApplyPalette(unsigned char *pal4,int from,int count);
	void ApplyPalette(unsigned char *pal4);
	void ApplyPalette256x3(unsigned char *pal3);
	void KeyPressRefresh(void);
	void QuitVideoMode(void);
	int  EventsLoop(void);


};

extern Controller mainController;
#endif

