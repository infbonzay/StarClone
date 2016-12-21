#if !defined(_MOUSE_W)
#define _MOUSE_W

#include "defs.h"
#include "const.h"
#include "grplib/grp.h"

#ifdef WITHSDL
       #include "sdl/mousesdl.h"
#endif
#ifdef UNDERDOS
       #define WMLEFTKEY  1
       #define WMRIGHTKEY 2
       void savelog(char *mes,int i);
       #include "dos/mousedos.h"
#endif

struct mouse_cursors
{
    GRPFILE *mousegrp;
    unsigned char curentposition;
    unsigned char maxpositions;
    unsigned char flag;
};

#define	MAXSCROLLTICKS 6
#define	SMOUTHMOUSE    1.0001

#define MOUSEON_MYUNIT			0
#define MOUSEON_ALLIANCEUNIT		1
#define MOUSEON_ENEMYUNIT		2
#define MOUSEON_NEUTRALUNIT		3
#define MOUSEON_MYBUILD			4
#define MOUSEON_ALLIANCEBUILD		5
#define MOUSEON_ENEMYBUILD		6
#define MOUSEON_NEUTRALBUILD		7
#define MOUSEON_MAXVALUE		8


#define SELECTIONMOUSE  	0
#define MOUSEILLEGAL		1
#define MOUSEONOBJGREEN 	2
#define MOUSEONOBJRED	 	3
#define MOUSEONOBJYELLOW 	4
#define MOUSEDOWNSCROLL		5
#define MOUSELEFTDOWNSCROLL	6
#define MOUSERIGHTDOWNSCROLL	7
#define MOUSELEFTSCROLL		8
#define MOUSERIGHTSCROLL	9
#define MOUSEUPSCROLL		10
#define MOUSELEFTUPSCROLL	11
#define MOUSERIGHTUPSCROLL	12
#define TARGGREENMOUSE		13
#define TARGNEUTRALMOUSE	14
#define TARGREDMOUSE		15
#define TARGYELLOWMOUSE		16
#define TIMEMOUSE		17
#define NORMALMOUSE		18

#define TYPEOFCURSORS 		19

extern int 	mousetype,grm,factorscrollkey;
extern int     	mouse_x,mouse_y,mouse_b,mousehotpos;
extern char    	curentREGIM;
extern int     	widthkart,hightkart,xmloc,ymloc,movieminikarta,mspeed;
extern struct  	XY mouser[MAXMOUSEMODE];
extern int     	patrate;
extern int      mousemaxx,mousemaxy,doublespeed,mousedoubleclick;
extern int     	decrxx,decryy,grm;
extern char    	select_aria,karta_aria,mode_aria;
extern char    	waitforleftbuton,waitfordownrightbuton,waitfordownleftbuton;
extern char    	mouse,patr,mouseclear,mloc,mousescrollflag;
extern int     	Xkart,Ykart;
extern short   	addscrx,addscry;
extern unsigned	short  mouseprop;
extern char     timemouse;
extern struct	OBJ *MouseOnOBJS[MOUSEON_MAXVALUE];
extern struct	OBJ *DestMouseOBJ;
extern char	DestMouseType;
extern char    	timemousewait;
extern int memmouseposx,memmouseposy;
extern int memmousepossizex,memmousepossizey;

int loadonecursor(char *filename,int mousetype);
int loadmousecursors(void);
void unloadmousecursors(void);
int restrictmousecoords(int REGIM);
void putmouseonscreen(void);
void desenpatr(void);
int mouseborder2(int x1,int y1,int x2,int y2);
int mouseborder(struct XY *m);
void desenlocation(void);
void  putdestination(struct OBJ *destobj,int xm,int ym,int modemove,int posibleconstr,int rightclick);
void nextmousespos(void);
void getmousetype(int x,int y);
void saveundermouse(void);
void loadundermouse(void);
float scrollmapx(int border,float factor);
float scrollmapy(int border,float factor);

void MouseOnObjClear(void);
void GetMouseOnObj(void);

//=================================
extern int mouseprevx,mouseprevy;
extern char prevmouseobjtype;
#endif /*    _MOUSE_W   */

