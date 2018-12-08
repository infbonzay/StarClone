#ifndef _CONTROLLER_W

#include <stdint.h>
#include "mycycle.h"

#define _CONTROLLER_W

#ifdef WITHSDL
	#include "sdl/keysdl.h"

	typedef SDL_Surface Controller_Surface;
	typedef SDL_Rect SCREEN_REGION;

#endif
#ifdef WITHX

	#include "xlib/xlibkey.h"

	#define	CFLAG_EXACTBPP	0x01

	typedef struct _Controller_Surface
	{
		Display 	*display;
		int			screenNr;
		Window		window;
		bool		FullScreen;
		uint8_t		*pixels;
		uint8_t		*palette;
		int			pixelsBufferSize;

		XImage		*Ximage;
		GC			gc;
		uint8_t		*Xpixels;
		int			XpixelsBufferSize;

		uint8_t		flags;

		XEvent		event;

		long		backgroundpixel;
		int			DesiredBpp;
		int			SavedWidth;
		int			SavedHeight;
		int			SavedBpp;
	} Controller_Surface;

	typedef struct _SCREEN_REGION
	{
		int16_t  x, y;
		uint16_t w, h;
	} SCREEN_REGION;
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

	int 				FullScreen;
	Controller_Surface	*Surface;

public:
	long 				KeyActive,LastKey;
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
private:
	template <typename T>
	bool  SetVideoMode(int x, int y);


};

extern Controller mainController;
#endif

