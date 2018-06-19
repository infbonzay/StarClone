#if !defined(_GR_W)
#define _GR_W

#include <grplib/grp.h>
#include "defs.h"
//==========================

int  setmode(int x,int y,int bpp,int fullscreen);
void horizline( int color,int x1,int x2,int y);
void vertline(  int color,int x,int y1,int y2);
void wrectangle(int color,int x1,int y1,int x2,int y2);
void wrectangle2(int color,int x1,int y1,int x2,int y2);
int  imagesize(struct XY *x,int dd);

//========================================
#define setimage	setimage8
#define putimage	putimage8
#define getimage	getimage8

  void circlef(int xc,int yc,int a,int b,int n);
  void circlenf(int xc,int yc,int a,int b,int n);
//========================================
extern  char 	*_tored_,*_togreen_,*_toblue_,*_towhite_,*_toblack_,*tomono;
extern  char  *adrtabls[];

#define NODRAW 		-1
#define NONETABLE 	0
#define REDTABLE  	1
#define GREENTABLE 	2
#define BLUETABLE	3
#define WHITETABLE 	4
#define BLACKTABLE	5
#define TFONTGAMTABLE	6
#define DISTORTIONTABLE 0

#define TORED		16
#define TOGREEN		17
#define TOBLUE		18
#define TOMONO		19

#define	FORYELLOW	20		//for text,properties
#define	FORGRAY		21
#define	FORWHITE	22
#define	FORBLUE		23

#define GREENCIRCLE	24
#define YELLOWCIRCLE	25
#define REDCIRCLE	26

#define SHIELDMAXPOS	9
#define LIFEMAXPOS	6

#define LIFE1		26
#define LIFEEND		(LIFE1+LIFEMAXPOS-1)
#define LIFEMINERAL	(LIFEEND+1)
#define LIFEGAS		(LIFEMINERAL+1)
#define TOMENUCOLOR	(LIFEGAS+1)

#define MAXGAMMA	100
#define MAXSATURATE	100

void initadrtables(void);
void putcube(int x,int y,int sizex,int sizey,char color);
void puttranslucencyrow(int x,int y,int sizex,int sizey,unsigned char *buff,int tablenr);
void putrow(int x,int y,int sizex,int sizey,unsigned char *buff);
void putrowwithtable(int x,int y,int sizex,int sizey,unsigned char *buff,char *table);
void putrow2x2(int x,int y,int sizex,int sizey,unsigned char *buff);
void putrow2x1(int x,int y,int sizex,int sizey,unsigned char *buff);
void palettegamma(char *palette,const char *origpalette,int factor);//factor -50 +50
void palettemono(char *palette,const char *origpalette,int factor);//factor 0-100
void pal3to4(char *pal4,const char *pal3);
void pal4to3(char *pal3,const char *pal4);
void ConvertColorsToNewPalette(unsigned char *pictpal,unsigned char *newpal,unsigned char *conversionarray,int palettesize);
void putgrp_nopacked2(int x,int y,int onlyx,int onlyy,GRPFILE *grp,int pictnr);
void drawmonoifpaused(void);

char *savescreen(void);
void restorescreen(char *adr);

#endif

