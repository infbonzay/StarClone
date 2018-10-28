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
extern int     	mouse_x,mouse_y,mouse_b,mousehotpos;
extern char    	curentREGIM;
extern int     	xmloc,ymloc,movieminikarta,mspeed;
extern struct  	XY mouser[MAXMOUSEMODE];
extern int     	patrate;
extern int      mousemaxx,mousemaxy,doublespeed,mousedoubleclick;
extern int     	decrxx,decryy,grm;
extern char    	select_aria,karta_aria,mode_aria;
extern char    	waitforleftbuton,waitfordownrightbuton,waitfordownleftbuton;
extern char    	mouse,patr,mouseclear,mloc,mousescrollflag;
extern unsigned	short  mouseprop;
extern char     timemouse;
extern struct	OBJ *MouseOnOBJS[MOUSEON_MAXVALUE];
extern struct	OBJ *DestMouseOBJ;
extern char		DestMouseType;
extern char    	timemousewait;
extern int 		memmouseposx,memmouseposy;
extern int 		memmousepossizex,memmousepossizey;

int  restrictmousecoords(int REGIM);
int  mouseborder2(int x1,int y1,int x2,int y2);
int  mouseborder(struct XY *m);
void desenlocation(void);
void putdestination(struct OBJ *destobj,int xm,int ym,int modemove,int posibleconstr,int rightclick);
void nextmousespos(void);
void getmousetype(int x,int y);
void saveundermouse(void);
void loadundermouse(void);
void saveunderpatr(void);
void loadunderpatr(void);

float scrollmapx(int border,float factor);
float scrollmapy(int border,float factor);

void MouseOnObjClear(void);
void GetMouseOnObj(void);

//=================================
extern int mouseprevx,mouseprevy;
extern char prevmouseobjtype;
#endif /*    _MOUSE_W   */

