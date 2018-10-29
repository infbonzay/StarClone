#if !defined(_MOUSE_W)
#define _MOUSE_W

#include <grplib/grp.h>
#include "defs.h"
#include "mousenew.h"

/*#ifdef WITHSDL
	#include "sdl/mousesdl.h"
#endif

#ifdef UNDERDOS
	   #define WMLEFTKEY  1
	   #define WMRIGHTKEY 2
	   void savelog(char *mes,int i);
	   //#include "dos/mousedos.h"
#endif
*/

#define MAXSCROLLTICKS 6

extern int				factorscrollkey;
extern int		mousehotpos;
extern int		xmloc,ymloc,movieminikarta,mspeed;
extern int		mousemaxx,mousemaxy,doublespeed,mousedoubleclick;
extern int		decrxx,decryy;
extern char		waitfordownrightbuton,waitfordownleftbuton;
extern char		patr,mouseclear,mloc,mousescrollflag;
extern unsigned short  mouseprop;
extern char		timemouse;
extern char		timemousewait;


void desenlocation(void);
void putdestination(struct OBJ *destobj,int xm,int ym,int modemove,int posibleconstr,int rightclick);
void getmousetype(int x,int y);
void saveunderpatr(void);
void loadunderpatr(void);

float scrollmapx(int border,float factor);
float scrollmapy(int border,float factor);

//=================================
#endif /*	 _MOUSE_W	*/

