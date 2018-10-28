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

#define	MAXSCROLLTICKS 6

extern int 		factorscrollkey;
extern int     	mouse_b,mousehotpos;
extern char    	curentREGIM;
extern int     	xmloc,ymloc,movieminikarta,mspeed;
extern struct  	XY mouser[MAXMOUSEMODE];
extern int      mousemaxx,mousemaxy,doublespeed,mousedoubleclick;
extern int     	decrxx,decryy,grm;
extern char    	waitfordownrightbuton,waitfordownleftbuton;
extern char    	mouse,patr,mouseclear,mloc,mousescrollflag;
extern unsigned	short  mouseprop;
extern char     timemouse;
extern char    	timemousewait;
extern int 		memmouseposx,memmouseposy;
extern int 		memmousepossizex,memmousepossizey;

int  restrictmousecoords(int REGIM);
int  mouseborder2(int x1,int y1,int x2,int y2);
int  mouseborder(struct XY *m);
void desenlocation(void);
void putdestination(struct OBJ *destobj,int xm,int ym,int modemove,int posibleconstr,int rightclick);
void getmousetype(int x,int y);
void saveunderpatr(void);
void loadunderpatr(void);

float scrollmapx(int border,float factor);
float scrollmapy(int border,float factor);

//=================================
#endif /*    _MOUSE_W   */

