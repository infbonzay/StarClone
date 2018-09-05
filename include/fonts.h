
#if !defined(_FONTS_W)
#define _FONTS_W

#include <grplib/grp.h>
#include "pcx.h"

//colors font for title
#define	TWHITECOLORFONT		4

//colors font for loadedmap,game in

//#define	GBLUECOLORFONT		0
//#define	GYELLOWCOLORFONT	1
//#define	GWHITECOLORFONT		2
//#define	GGREYCOLORFONT		3
//#define	GREDCOLORFONT		4
//#define	GGREENCOLORFONT		5
//#define	DEFAULTCOLORFONT	6
//#define	MAXCOLORFONT		6

#define STATICCOLOR1		5
#define	DEFAULTCOLORFONT	8
#define	GDEFAULTCOLORFONTCHAR	"\x8"
#define	MAXCOLORFONT		DEFAULTCOLORFONT

#define	GBLUECOLORFONT		2
#define	GYELLOWCOLORFONT	3
#define	GWHITECOLORFONT		4
#define	GGREYCOLORFONT		5
#define	GREDCOLORFONT		6
#define	GGREENCOLORFONT		7

#define	GBLUECOLORFONTCHAR		"\x2"
#define	GYELLOWCOLORFONTCHAR		"\x3"
#define	GWHITECOLORFONTCHAR		"\x4"
#define	GGREYCOLORFONTCHAR		"\x5"
#define	GREDCOLORFONTCHAR		"\x6"
#define	GGREENCOLORFONTCHAR		"\x7"



#define FONTCOLOR(tfont,genercolor,gradation) (tfont[(genercolor-2)*8+gradation])

#define GREENHPBAR		0
#define YELLOWHPBAR		1
#define REDHPBAR		2
#define BLUEHPBAR		3
#define VIOLETHPBAR		4
#define GREYHPBAR		5
#define MAXCOLORHPBAR		6

#define IDFONT8			0
#define IDFONT10		1
#define IDFONT12		2
#define IDFONT14		3
#define IDFONT16		4
#define IDFONT16X		5
#define IDFONT32		6
#define IDFONT50		7

#define SIMPLE(colors)		((colors) & 0xff)
#define SELECTEDBYCLICK(colors)	(((colors) >> 16) & 0xff)
#define SELECTEDMOUSE(colors)	((colors) >> 24)


#define RIGHTALIGNSYMB		18
#define CENTERALIGNSYMB		19
#define COMMANDSYMB		20
#define COMMANDSYMBCHAR		"\x14"

#define TEXT_LEFTALIGN		0
#define TEXT_CENTERALIGN	1
#define TEXT_RIGHTALIGN		2

#define MINERALSYMB		1
#define MINERALSYMBCHAR		"\x1"
#define GASSYMB			2
#define GASSYMBCHAR		"\x2"
#define MANASYMB		3
#define PSISYMBZ		4
#define PSISYMBT		5
#define PSISYMBP		6

//				7-10 removed for compatibility with '\n' symbol
#define ICONFOROPENDIR		11
#define ICONFORCLOSENOSELDIR	12
#define ICONFORCLOSESELDIR	13
#define ICONFORSELFILE		14
#define ICONFORNOSELFILE	15
#define CUBESYMB		16
#define	CHANGEFONTSYMB		17
#define	DLGGRPSYMB		18
#define EMPTYCUBE14X14		19
#define EMPTYCUBE14X14CHAR	"\x13"
#define TOTALCOMMANDSYMBS	20

SC_FontHeader *loadfont(char *filename);

void unloadfont(SC_FontHeader *font);
int  putfntsymbol2(int x,int y,int fontnr,int symbolnr,char *offsettable,int skupup,int skipdown);
int  putglowtext(int x,int y,int fontnr,char *str,int fromcolor,char *table,int glowfactor);
int  putmessage(int x,int y,int fontnr,char *str,int fromcolor,char *table,GRPFILE *dlggrp);
int  getlinesintext(int fontnr,char *mes,int meslen,int sizexrect);
int  putmessage(int x,int y,int fontnr,char *str,int fromcolor,char *table,GRPFILE *dlggrp,int skipup,int skipdown);
int  getspacebetweensymbols(int fontnr);
int  getlettersizexy(int fontnr,int symbolnr,int *sizex,int *sizey);
int  getmessagesizex(int fontnr,char *str,int sizestr,int *lenx,int *leny);
void getmaxsymbolsize(int fontnr,int *sizex,int *sizey);
int  getmessagelen(int fontnr,char *str,int *lenx,int *leny);
void putboxmessage( int cborder,int cbox,
                    int x1,int y1,int bordersize,
                    int fontnr,char *message,int messagecolor,char *tfont,GRPFILE *dlggrp);

#define MESINRECT_COLORS	0x1
#define MES_SPLITTONEXTLINE	0x2

void putmessageinrectangleL(int x,int y,int sizexrect,int sizeyrect,int rowsize,char *mes,
			    int fontnr,int color,char *fonttable,GRPFILE *dlggrp,int flags,int droppixels);
void putmessageinrectangleR(int x,int y,int sizexrect,int sizeyrect,int rowsize,char *mes,
			    int fontnr,int color,char *fonttable,GRPFILE *dlggrp,int flags);
void putmessageinrectangleM(int x,int y,int sizexrect,int sizeyrect,int rowsize,char *mes,
			    int fontnr,int color,char *fonttable,GRPFILE *dlggrp,int flags);
int  GetFontNrFromYSize(int ysize);
void putsmkbuttonmes(int x,int y,int rowsize,char *mes,int fontnr,int color,char *fonttable,int glowfactor);
void putsmkbuttonmesL(int x,int y,int rowsize,char *mes,int fontnr,int color,char *fonttable,int glowfactor);
void putsmkbuttonmesM(int x,int y,int sizexrect,int rowsize,char *mes,int fontnr,int color,char *fonttable,int glowfactor);
void putsmkbuttonmesR(int x,int y,int sizexrect,int rowsize,char *mes,int fontnr,int color,char *fonttable,int glowfactor);
int  visibletext(int fontnr,int sizex,char *text);
int  visiblerighttext(int fontnr,int sizex,char *text);
int  getrowmessagesize(int fontnr,char *str,int sizestr);
void putrowtext(int x,int y,int sizewinx,int fontnr,int flags,char *str,int fromcolor,char *table);

extern PCX dlgfilediriconspcx[5];

#endif /*_FONTS_W*/


