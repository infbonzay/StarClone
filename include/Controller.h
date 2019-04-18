#ifndef _SC_CONTROLLER_H

#define _SC_CONTROLLER_H

#include <stdint.h>
#include "mycycle.h"
#include "mouse.h"

#define KEYPRESSEDTABLE(table,scancode) (toupper((scancode)&0xff)+1)

#define	CFLAG_EXACTBPP	0x01
#define	CFLAG_QUIT		0x02

#ifdef UNDERWINDOWS

#include "win/winkey.h"

	typedef struct
	{
		const uint8_t ximagebpp[5] = {0,8,16,32,32}; //for 0,8,16,24,32 color bits
		bool		FullScreen;
		uint8_t		*pixels;
		uint8_t		*palette;
		int			pixelsBufferSize;
		int			DesiredBpp;
		int			SavedWidth;
		int			SavedHeight;
		int			SavedBpp;

		//win32 vars
		HWND		window;
		HINSTANCE	hInstance;
		MSG			msg;
		HDC			hdc;
		HDC			backDC;
		HBITMAP		backBitmap;
		int			iCmdShow;
		int			width;
		int			height;
		uint8_t		*Xpixels;
		int			XpixelsBufferSize;
		uint8_t		flags;
		DEVMODE     *DevMode;
	} Controller_Surface;

	typedef struct
	{
		int16_t  x, y;
		uint16_t w, h;
	} SCREEN_REGION;

#endif
#ifdef WITHSDL
	#include "sdl/sdlkey.h"

	typedef SDL_Surface Controller_Surface;
	typedef SDL_Rect SCREEN_REGION;

#endif
#ifdef WITHX

	#include "xlib/xlibkey.h"

	typedef struct
	{
		const uint8_t ximagebpp[5] = {0,8,16,32,32}; //for 0,8,16,24,32 color bits
		bool		FullScreen;
		uint8_t		*pixels;
		uint8_t		*palette;
		int			pixelsBufferSize;
		int			DesiredBpp;
		int			SavedWidth;
		int			SavedHeight;
		int			SavedBpp;

		//xlib vars
		Display 	*display;
		int			screenNr;
		Window		window;
		Visual		*XVisual;
		XImage		*Ximage;
		Cursor		noCursor;
		GC			gc;
		XEvent		event;
		uint8_t		*Xpixels;
		int			XpixelsBufferSize;
		uint8_t		flags;

	} Controller_Surface;

	typedef struct
	{
		int16_t  x, y;
		uint16_t w, h;
	} SCREEN_REGION;
#endif
#ifdef WITHFB
	#include "fb/fbkey.h"

	typedef struct
	{
		const uint8_t ximagebpp[5] = {0,8,16,24,32}; //for 0,8,16,24,32 color bits
		uint8_t		*pixels;
		uint8_t		*palette;
		int			pixelsBufferSize;
		int			width;
		int			heigth;
		int			DesiredBpp;

		//frame buffer vars
		int			SavedWidth;
		int			SavedHeight;
		int			SavedBpp;
		struct fb_var_screeninfo vinfo;
		struct fb_fix_screeninfo finfo;
		int			fbfd;
		void		*fbp;

		uint8_t		*FBpixels;
		int			FBpixelsBufferSize;
		uint8_t		flags;

	} Controller_Surface;
	typedef struct
	{
		int16_t  x, y;
		uint16_t w, h;
	} SCREEN_REGION;

#endif

class Controller
{

public:
	int 				FullScreen;
	Controller_Surface	*Surface;

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

	long 				KeyActive,LastKey;
	int 				RefreshAtEnd;
	uint32_t			KeyFlags;				//shift,ctrl,alt
	mycycle<uint16_t> 	*KeysBuffer;

	int Init();
	void DeInit();
	int  QueryVideoMode(int x, int y, int bpp, int fullscreen);
	int  ModifyVideoMode(int x, int y, int bpp, int fullscreen,unsigned char *palette);
	bool CanFullScreen(void);

	void UpdateScreenRegions(int nrregions,SCREEN_REGION regions[]);
	void UpdateScreenRegion(int x,int y,int xsize,int ysize);
	void UpdateScreen(void);
	void ApplyPalette(unsigned char *pal4,int from,int count);
	void ApplyPalette(unsigned char *pal4);
	void ApplyPalette256x3(unsigned char *pal3);
	void QuitVideoMode(void);
	void HideCursor(void);
	void ShowCursor(void);
	int  EventsLoop(void);
	void SetWindowName(const char *winName);
	void SetKeyMod(int keysym, bool status);
private:
	bool SetVideoMode(int x, int y);
	void QuitVideoMode(bool Disconnect);
	void TransformPixels(int x, int y, int sizex, int sizey);
	template <typename T>
			void Transform(int x, int y, int sizex, int sizey);
	void SaveDesktopResolution(int x,int y);
	void DesktopResolution(int *x,int *y);
};

extern Controller mainController;
#endif

