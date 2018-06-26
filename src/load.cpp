//=================================================================
//=================================================================
#include <stdlib.h>
#include <grplib/usegrp.h>
#include <grplib/palette.h>

#include "creep.h"
#include "flingy.h"

#include "const.h"
#include "vars.h"
#include "man.h"
#include "gr.h"
#include "pcx.h"
#include "mouse.h"
#include "wmem.h"
#include "scripts.h"
#include "mpq.h"
#include "gener.h"
#include "fonts.h"
#include "maps.h"
#include "portrait.h"
#include "objinfo.h"
#include "overlay.h"
#include "lists.h"
#include "weapons.h"
#include "stringfiles.h"
#include "load.h"

char    zerglife[LIFEMAXPOS][4]={
                            {14, 6, 7, 8},
                            {19,11,13, 6},
                            {19,11,10,16},
                            { 1,18,20,16},
                            { 1,18,20,11},
                            { 0, 1,18,10}
                        };
char       ptlife[LIFEMAXPOS][4]={
			    {10,10,13,13},
			    { 0,10,13,13},
			    { 0,10,10,13},
			    { 1, 0, 0,10},
			    { 1, 1, 0, 0},
			    { 1, 1, 1, 1}
			};
char	shield_pos[SHIELDMAXPOS+1]={2,2,9,7,7,5,5,3,3,3}; 
char	mineral_pos[4]={8,9,11,12}; 
char	gas_pos[4]={15,16,17,18}; 
//================================================
//first bytes is count of empty pixel repeated
char *CreatePackedConsoleMenu(char *mem,int xpictsize,int ypictsize,
					int xwinsize,int ywinsize,int skipx)
{
    int i,j,membyte,countzero=skipx,countnozero=0,bytes=0,writepos;
    char *tempmem,*tempmem2;
    int *tempmemint;
    tempmem = (char *) wmalloc(xwinsize*ywinsize);
    memset(tempmem,0,xwinsize*ywinsize);
    for (i=0;i<ywinsize;i++)
    for (j=0;j<xwinsize;j++)
    {
	if (i<ywinsize-ypictsize||j>=xpictsize)
	    membyte = 0;
	else
	    membyte = mem[(i-(ywinsize-ypictsize))*xpictsize+j];
	if (!membyte)
	{
	    if (countnozero)
	    {
		tempmemint = (int *)&tempmem[writepos];
		*tempmemint = countnozero;
    		countnozero=0;
	    }
	    countzero++;
	}
	else
	{
	    countnozero++;
	    if (countzero||(!i))
	    {
		tempmemint = (int *)&tempmem[bytes];
		*tempmemint = countzero;
		bytes+=sizeof(int);
		countzero=0;
	    }
	    if (countnozero==1)
	    {
		writepos = bytes;
		bytes+=sizeof(int);
	    }
	    tempmem[bytes++] = membyte;
	}
    }
    if (countnozero)
    {
	tempmemint = (int *)&tempmem[writepos];
	*tempmemint = countnozero;
    }	
    if (countzero)
    {
	tempmemint = (int *)&tempmem[bytes];
	*tempmemint = countzero;
	bytes+=sizeof(int);
    }	

    tempmemint = (int *)&tempmem[bytes];
    *tempmemint = 0;
    bytes+=sizeof(int);
    tempmem2 = (char *) wmalloc(bytes);
    memcpy(tempmem2,tempmem,bytes);
    wfree(tempmem);
    return (tempmem2);
}
//================================================
void unloadpcxicons(void)
{
    //no to unload pointers the, iresp & manap is static array
}
//================================================
int loadpcxicons(void)
{
    int err;
    PCX pcx;
    for (int race=0;race<3;race++)
    {
	for (int type=0;type<3;type++)
	{
	    err=pcx.openMpqPcx(ires[race][type]);
	    if ((err!=OKPCX)||(pcx.sizePcx()!=14*14))
	    {
    		printf("error in %s\n",ires[race][type]);
    		return -1;
	    }
	    pcx.readBytesFromPcx((char *)iresp[race][type]);
	    pcx.closePcx();
	}
    }

    err=pcx.openMpqPcx(imana);
    if ((err!=OKPCX)||(pcx.sizePcx()!=14*14))
    {
	printf("error in %s\n",imana);
	return -1;
    }
    pcx.readBytesFromPcx((char *)imanap);
    pcx.closePcx();
    err=pcx.openMpqPcx(iemptycube);
    if ((err!=OKPCX)||(pcx.sizePcx()!=14*14))
    {
	printf("error in %s\n",iemptycube);
	return -1;
    }
    pcx.readBytesFromPcx((char *)iemptycubep);
    pcx.closePcx();
    return(0);
}
//================================================
void unloadznak(void)
{
    if (blinkgrp)
    {
	unloadfilefrommpq((char *)blinkgrp);
	blinkgrp=NULL;
    }
    if (znakgrp)
    {
	unloadfilefrommpq((char *)znakgrp);
	znakgrp=NULL;
    }
    if (grpwire1)
    {
	unloadfilefrommpq((char *)grpwire1);
	grpwire1=NULL;
    }
    if (grpwire2)
    {
	unloadfilefrommpq((char *)grpwire2);
	grpwire2=NULL;
    }
    if (grpwire3)
    {
	unloadfilefrommpq((char *)grpwire3);
	grpwire3=NULL;
    }
}
//================================================
int loadznak(void)
{
    if (mpqloadfile(iconsgrp,(char **)&znakgrp))
    {
	printf("error in %s\n",iconsgrp);
	return -1;
    }
    if (mpqloadfile(wire1,(char **)&grpwire1))
    {
	printf("error in %s\n",wire1);
	return -2;
    }
    if (mpqloadfile(wire2,(char **)&grpwire2))
    {
	printf("error in %s\n",wire2);
	return -2;
    }
    if (mpqloadfile(wire3,(char **)&grpwire3))
    {
	printf("error in %s\n",wire3);
	return -2;
    }
    if (mpqloadfile(blinkgrpfn,(char **)&blinkgrp))
    {
	printf("error in %s\n",blinkgrpfn);
	return -1;
    }
    return 0;
}
//=============================================
void unloadcircles(void)
{
    for (int i=0;i<10;i++)
    {
	if (fullc[i])
	{
	    fullc[i]=NULL;
	}
	if (puncc[i])
	{
	    puncc[i]=NULL;
	}
    }
}
//=============================================
int saveallstruct(char *file,char *string1)
{
  return(1);
}
//==========================
int loadstage(char *file)
{
    return(1);
}
//==============================
void restart(void)
{
}
//==============================
int loadlang(void)
{
    switch(gameconf.lang)
    {
    case 0:
	return(_loadlang("data/rus.lng"));
    case 1:
	return(_loadlang("data/eng.lng"));
    }
    return(-1);
}
//==============================
int _loadlang(char *lang)
{
  int m;
  FILE *h;
  char buf[512];
  h = fopen(lang,"rb");
  if (!h)
     return(-1);
  do{
    m=fread(&buf,512,1,h);   
  }while(m==512);
  fclose(h);
return(0);
}
//==============================
struct OBJstruct *loadobj(int numberobj)
{
    if (!allobj[numberobj])
    {
	int  k;
	FILE *f;
        char sss[100];
        f=fopen(filemans[pos_in_file_man[numberobj].filenr],"r");
        if (!f)
	{
    	    sprintf(sss,"file %s not opened!!!",filemans[0]);
    	    savelog(sss,0);
            gameend(forexit);
        }
	if (pos_in_file_man[numberobj].pos==0)
	{
    	    sprintf(sss,"object nr=%d not found in %s!!!",
        	      numberobj,filemans[0]);
    	    savelog(sss,0);
	    char *adr=NULL;
	    *adr=0x0;
//	    return NULL;
            gameend(forexit);
	}
	fseek(f,pos_in_file_man[numberobj].pos,SEEK_SET);
	k=readrecordmans(f,numberobj);
        if (k!=0)
	{
    	    sprintf(sss,"object nr=%d not loaded!!!",
        	      numberobj);
    	    savelog(sss,0);
            gameend(forexit);
	}
        fclose(f);
//        sprintf(sss,"object nr=%d loaded ok!",numberobj);
//        savelog(sss,0);
    }
    return(allobj[numberobj]);
}
//==============================
void unloadpcxtables(void)
{
//    adrtabls[TFONTGAMTABLE] = NULL;
}
//==============================
int loadpcxtables(void)
{
    static char alreadyset=0;
    char *tempcol;
    int i,j,j2;
    PCX pcx;
    char tablforunit[256];
    int err=pcx.openMpqPcx(tunit);
    if ((err!=OKPCX)||(pcx.sizePcx()!=128))
    {
	printf("error in %s \n",tunit);
	return -1;
    }
    pcx.readBytesFromPcx(tunitp,128);
    pcx.closePcx();

    err=pcx.openMpqPcx(thpbar);

    if (err!=OKPCX)
    {
	printf("error in %s \n",thpbar);
	return -1;
    }
    pcx.readBytesFromPcx(thpbarp);
    pcx.closePcx();

    SetUserPlayerColors(0,16,tunitp);

    err=pcx.openMpqPcx(icons);
    if ((err!=OKPCX)||(pcx.sizePcx()!=96))
    {
	printf("error in %s \n",icons);
	return -1;
    }
    pcx.readBytesFromPcx(tablforunit,96);
    pcx.closePcx();

    SetPlayerColors(FORYELLOW,1,0,16,&tablforunit[0]);
    SetPlayerColors(FORGRAY,1,0,16,&tablforunit[16]);
    SetPlayerColors(FORWHITE,1,0,16,&tablforunit[32]);
    SetPlayerColors(FORBLUE,1,0,16,&tablforunit[64]);
    
    err=pcx.openMpqPcx(tselect);
    if ((err!=OKPCX)||(pcx.sizePcx()>256))
    {
	printf("error in %s \n",tselect);
	return -1;
    }
    pcx.readBytesFromPcx(tablforunit);
    SetPlayerColors(GREENCIRCLE,1,1,8,&tablforunit[0]);
    SetPlayerColors(YELLOWCIRCLE,1,1,8,&tablforunit[8]);
    SetPlayerColors(REDCIRCLE,1,1,8,&tablforunit[16]);
    pcx.closePcx();

    err=pcx.openMpqPcx(tminimap);
    if ((err!=OKPCX)||pcx.sizePcx()>16)
    {
	printf("error in %s \n",tminimap);
	return -1;
    }
    pcx.readBytesFromPcx(tableforminimap);
    pcx.closePcx();

    err=pcx.openMpqPcx(twire);
    if (err!=OKPCX||pcx.sizePcx()>24)
    {
	printf("error in %s \n",twire);
	return -1;
    }
    pcx.readBytesFromPcx(twirep,24);
    for (i=0;i<LIFEMAXPOS;i++)
	for (j=0;j<4;j++)
	{
	    tempcol = &twirep[zerglife[i][j]];
	    SetPlayerColors(LIFE1+i,1,0xd8+j,1,tempcol);
	    tempcol = &twirep[ptlife[i][j]];
	    SetPlayerColors(LIFE1+i,1,0xd0+j,1,tempcol);
	}
    if (!alreadyset)
    {
	alreadyset=1;
	for (i=0;i<SHIELDMAXPOS+1;i++)
	{
	    shield_pos[i] = twirep[shield_pos[i]];
	}

	for (i=0;i<4;i++)
	{
	    mineral_pos[i] = tunitp[mineral_pos[i]];
	    gas_pos[i] = thpbarp[gas_pos[i]];
	}
    }
    //set color for minerals


    SetPlayerColors(LIFEMINERAL,1,0xd8,4,mineral_pos);
    SetPlayerColors(LIFEGAS,1,0xd8,4,gas_pos);
    
    pcx.closePcx();

    err=pcx.openMpqPcx(tfontgam);

    if (err!=OKPCX)
    {
	printf("error in %s \n",tfontgam);
	return -1;
    }
    pcx.readBytesFromPcx(tfontgamp,48);
    pcx.closePcx();
    
    err=pcx.openMpqPcx(tblink);

    if (err!=OKPCX)
    {
	printf("error in %s \n",tblink);
	return -1;
    }
    pcx.readBytesFromPcx(tblinkp,152);
    pcx.closePcx();

    memset(&KUBIK,FONTCOLOR(tfontgamp,GBLUECOLORFONT,0),sizeof(KUBIK));
    for (i=0;i<MAXCOLORHPBAR;i++)
    {
	for (j=1;j<SIZEYKUBIK;j++)
	    for (j2=1;j2<SIZEXKUBIK;j2++)
		KUBIK[i][j*SIZEXKUBIK+j2] = thpbarp[i*3+j-1];
    }
    memset(&PKUBIK,FONTCOLOR(tfontgamp,GBLUECOLORFONT,0),sizeof(PKUBIK));
    for (i=0;i<MAXCOLORHPBAR;i++)
    {
	for (j=1;j<SIZEYKUBIK-1;j++)
	    for (j2=1;j2<SIZEXKUBIK;j2++)
		PKUBIK[i][j*SIZEXKUBIK+j2] = thpbarp[i*3+j];
    }
    return 0;
}
//==============================
char *dlgtilegrpname[4]={
		"dlgs\\zerg.grp",
		"dlgs\\terran.grp",
		"dlgs\\protoss.grp",
		"dlgs\\tile.grp"
};
//==============================
int loadtilegrp(int race)
{
    int err;
    if (race==ZERGRACE||race==TERRANRACE||race==PROTOSSRACE)
	err=mpqloadfile(dlgtilegrpname[race],(char **)&gamedlggrp);
    if (err)
	return(-1);
    err=mpqloadfile(dlgtilegrpname[3],(char **)&gametilegrp);
    if (err)
    {
	unloadtilegrp();
	return(-1);
    }
    return(0);
}
//==============================
void unloadtilegrp(void)
{
    if (gamedlggrp)
    {
    	unloadfilefrommpq((char *)gamedlggrp);
	gamedlggrp=NULL;
    }
    if (gametilegrp)
    {
	unloadfilefrommpq((char *)gametilegrp);
	gametilegrp=NULL;
    }
}
//==============================
void loadallfonts(void)
{
    unloadallfonts();
    for (int i=0;i<MAXFONTS;i++)
    {
//	printf("loading %s",fontname[i]);
	fonts[i] = loadfont(fontname[i]);
//	if (!fonts[i])
//	    printf(" ... with error");
//	printf("\n");	    
    }
}
//==============================
void unloadallfonts(void)
{
    for (int i=0;i<MAXFONTS;i++)
    {
	if (fonts[i])
	{
	    unloadfont(fonts[i]);
	    fonts[i]=NULL;
	}
    }
}
//==============================
void unloadpal(void)
{
    if (_tored_)
    {
        wfree(_tored_);
	_tored_=NULL;
    }
    if (_togreen_)
    {
        wfree(_togreen_);
	_togreen_=NULL;
    }
    if (_toblue_)
    {
        wfree(_toblue_);
	_toblue_=NULL;
    }
    if (_towhite_)
    {
        wfree(_towhite_);
	_towhite_=NULL;
    }
    if (_toblack_)
    {
        wfree(_toblack_);
	_toblack_=NULL;
    }
    if (transpadr)
    {
        wfree(transpadr);
	transpadr=NULL;
    }
    if (tomono)
    {
        wfree(tomono);
	tomono=NULL;
    }
}
//==============================
int loadpal(int tileset,char *palette4)
{
    int s,k;
    char tablforunit[256];
    char palette3[256*3];
    char temppath[MAX_PATH];
    PCX *pcx=(PCX *)wmalloc(sizeof(PCX));
    strcpy(temppath,palettedir[tileset]);
    strcat(temppath,blackgrd);
    k=pcx->openMpqPcx(temppath);
    if (k!=OKPCX)
	return 1;
    s=pcx->sizePcx();
    _toblack_ = (char *)wmalloc(s+256);
    memset(_toblack_,0,256);
    pcx->readBytesFromPcx(_toblack_+256);
    pcx->closePcx();

    strcpy(temppath,palettedir[tileset]);
    strcat(temppath,whitegrd);
    k=pcx->openMpqPcx(temppath);
    if (k!=OKPCX)
	return 2;
    s=pcx->sizePcx();
    _towhite_ = (char *)wmalloc(s+256);
    memset(_towhite_,0,256);
    pcx->readBytesFromPcx(_towhite_+256);
    pcx->closePcx();

    strcpy(temppath,palettedir[tileset]);
    strcat(temppath,redgrd);
    k=pcx->openMpqPcx(temppath);
    if (k!=OKPCX)
	return 3;
    s=pcx->sizePcx();
    _tored_=(char *)wmalloc(s+256);
    memset(_tored_,0,256);
    pcx->readBytesFromPcx(_tored_+256);
    pcx->closePcx();

    strcpy(temppath,palettedir[tileset]);
    strcat(temppath,greengrd);
    k=pcx->openMpqPcx(temppath);
    if (k!=OKPCX)
	return 4;
    s=pcx->sizePcx();
    _togreen_=(char *)wmalloc(s+256);
    memset(_togreen_,0,256);
    pcx->readBytesFromPcx(_togreen_+256);
    pcx->closePcx();

    strcpy(temppath,palettedir[tileset]);
    strcat(temppath,bluegrd);
    k=pcx->openMpqPcx(temppath);
    if (k!=OKPCX)
	return 5;
    s=pcx->sizePcx();
    _toblue_=(char *)wmalloc(s+256);
    memset(_toblue_,0,256);
    pcx->readBytesFromPcx(_toblue_+256);
    pcx->closePcx();

    pal4to3(palette3,palette4);    
    tomono = (char *)wmalloc(256);
    CreateMono(palette3,tomono);
    SetPlayerColors(TOMONO,1,0,256,tomono);

    strcpy(temppath,palettedir[tileset]);
    strcat(temppath,greentr);
    k=pcx->openMpqPcx(temppath);
    if (k!=OKPCX)
	return 6;
    pcx->readBytesFromPcx(tablforunit);
    SetPlayerColors(TOGREEN,1,0,256,tablforunit);
    pcx->closePcx();

    strcpy(temppath,palettedir[tileset]);
    strcat(temppath,bluetr);
    k=pcx->openMpqPcx(temppath);
    if (k!=OKPCX)
	return 7;
    pcx->readBytesFromPcx(tablforunit);
    SetPlayerColors(TOBLUE,1,0,256,tablforunit);
    pcx->closePcx();

    strcpy(temppath,palettedir[tileset]);
    strcat(temppath,redtr);
    k=pcx->openMpqPcx(temppath);
    if (k!=OKPCX)
	return 8;
    pcx->readBytesFromPcx(tablforunit);
    SetPlayerColors(TORED,1,0,256,tablforunit);
    pcx->closePcx();


    strcpy(temppath,palettedir[tileset]);
    strcat(temppath,transp);
    k=pcx->openMpqPcx(temppath);
    if ((k!=OKPCX)||(pcx->sizePcx()!=256*255))
	return 9;
    transpadr = (char *)wmalloc(256*256);
    pcx->readBytesFromPcx(transpadr+256);
    memset(transpadr,0,256);
//  memcpy(transpadr,transpadr+256,256);
    pcx->closePcx();
    wfree(pcx);

    SetTranspTable(transpadr);
    initadrtables();
    return 0;
}
//=======================================
void unloadramka(void)
{
    int i;
    consoleover.closePcx();
    consoleraw.closePcx();
    iconraw.closePcx();
    buildemptyraw.closePcx();
    buildfullraw.closePcx();
    if (packedconsole)
    {
	wfree(packedconsole);
	packedconsole=NULL;
    }
    if (packedconsoleover)
    {
	wfree(packedconsoleover);
	packedconsoleover=NULL;
    }
    for (i=0;i<3;i++)
    {
//	wfree(grpminmineral[i]);	//not needed if loaded with GetLoadImage
//	wfree(grpmingas[i]);
	grpminmineral[i]=NULL;
	grpmingas[i]=NULL;
    }
}
//=======================================
int loadramka(int needrace)	//0-z,1-t,2-p
{
    int err,i;
    char race[2],filename[200];
    race[0] = racename[needrace][0];
    race[1] = 0;
    //read console menu of needed race

    strcpy(filename,"game\\");
    strcat(filename,race);
    strcat(filename,CONSOLEPCX);
    err=consoleraw.openMpqPcx(filename);
    if (err!=OKPCX)
	return -1;

    packedconsole = CreatePackedConsoleMenu(consoleraw.GetPcxRawBytes(),
		    consoleraw.xsizePcx(),consoleraw.ysizePcx(),GRP_screensizex,GRP_screensizey,0);
    //read upgrade-empty positions of needed race

    strcpy(filename,"game\\");
    strcat(filename,race);
    strcat(filename,CONSOLEOVERPCX);
    err=consoleover.openMpqPcx(filename);
    if (err!=OKPCX)
	return -2;

    packedconsoleover = CreatePackedConsoleMenu(consoleover.GetPcxRawBytes(),
		    consoleover.xsizePcx(),consoleover.ysizePcx(),GRP_screensizex,SMKICONPOSY+consoleover.ysizePcx(),SMKICONPOSX);

    //read upgrade-full positions of needed race

    strcpy(filename,"game\\");
    strcat(filename,race);
    strcat(filename,BUILDEMPTYPCX);
    err=buildemptyraw.openMpqPcx(filename);
    if (err!=OKPCX)
	return -4;

    strcpy(filename,"game\\");
    strcat(filename,race);
    strcat(filename,BUILDFULLPCX);
    err=buildfullraw.openMpqPcx(filename);
    if (err!=OKPCX)
	return -5;

    return 0;
}
//==========================
int loadbuttons(int race)
{
    sprintf(PREFIX_UNIT+PREFIX_UNIT_OFFSET,"cmdbtns\\%ccmdbtns.grp",racename[race][0]);
    return(mpqloadfile(PREFIX_UNIT,(char **)&grpicons));
}
//==========================
void unloadbuttons(void)
{
    if (grpicons)
    {
	wfree(grpicons);
	grpicons=NULL;
    }
}
//==========================
int loadcreep(int tileset)
{
    char temppath[MAX_PATH];

    strcpy(temppath,palettedir[tileset]);
    strcat(temppath,".grp");
    
    int ret = mpqloadfile(temppath,(char **)&creepgrp);
    if (!ret)
    {
	if (map.creeptiles==NULL)
	{
	    map.creeptilecnt=1;
	    map.creeptiles=(char *)wmalloc(32*32);
    	    unpackgrp(0,0,creepgrp,36,map.creeptiles);//unpack last full grp sprite
	}
    }
    return ret;
}
//==============================
void unloadcreep(void)
{
    if (creepgrp)
    {
	wfree(creepgrp);
	creepgrp=NULL;
    }
}
//==============================
void savelog(char *mes,int i)
{
#ifdef UNDERLINUX
    if (i)
	printf("%s%d\n",mes,i);
    else
	printf("%s\n",mes);
#endif
#ifdef DEBUGOUTPUT
    char sss[500];
    if (i)
	sprintf(sss,"%s %d\n",mes,i);
    else
	sprintf(sss,"%s\n",mes);
    FILE *f=fopen("1.log","a+");
    if (!f)
       return;
    fwrite(sss,1,strlen(sss),f);
    fclose(f);
#endif
}
//=======================================
#define SC_IMAGES_DAT		"arr\\images.dat"
#define SC_SPRITES_DAT		"arr\\sprites.dat"
#define SC_UNITS_DAT		"arr\\units.dat"
#define SC_FLINGY_DAT		"arr\\flingy.dat"
#define SC_PORTDATA_DAT		"arr\\portdata.dat"
#define SC_WEAPONS_DAT		"arr\\weapons.dat"
#define SC_UPGRADES_DAT		"arr\\upgrades.dat"
#define SC_TECHDATA_DAT		"arr\\techdata.dat"
#define SC_ORDERS_DAT		"arr\\orders.dat"
#define SC_SFXDATA_DAT		"arr\\sfxdata.dat"
#define SC_MAPDATA_DAT		"arr\\mapdata.dat"

#define SC_IMAGES_TBL		"arr\\images.tbl"
#define SC_PORTDATA_TBL		"arr\\portdata.tbl"
#define SC_SFXDATA_TBL		"arr\\sfxdata.tbl"
#define SC_STATTXT_TBL		"rez\\stat_txt.tbl"
#define SC_GLUALL_TBL		"rez\\gluall.tbl"
#define SC_NETWORK_TBL		"rez\\network.tbl"
#define SC_MYINFO_TBL		"rez\\myinfo.tbl"
#define SC_CAMPAIGN_TBL		"rez\\campaign.tbl"
#define SC_MAPDATA_TBL		"arr\\mapdata.tbl"
//============================================
int LoadDatTblFiles(DATTBLSTRUCT *dattbl)
{
    char tblstring;
    int i,portdata_id,nrofstr;
    char *str;
    if (mpqloadfile(SC_IMAGES_DAT,(char **)&dattbl->images_dat))
	;//return(-1);
    if (mpqloadfile(SC_SPRITES_DAT,(char **)&dattbl->sprites_dat))
	;//return(-2);
    if (mpqloadfile(SC_UNITS_DAT,(char **)&dattbl->units_dat))
	;//return(-3);
    if (mpqloadfile(SC_FLINGY_DAT,(char **)&dattbl->flingy_dat))
	;//return(-3);
    if (mpqloadfile(SC_PORTDATA_DAT,(char **)&dattbl->portdata_dat))
	;//return(-4);
    if (mpqloadfile(SC_WEAPONS_DAT,(char **)&dattbl->weapons_dat))
	;//return(-5);
    if (mpqloadfile(SC_UPGRADES_DAT,(char **)&dattbl->upgrades_dat))
	;//return(-6);
    if (mpqloadfile(SC_TECHDATA_DAT,(char **)&dattbl->techdata_dat))
	;//return(-7);
    if (mpqloadfile(SC_ORDERS_DAT,(char **)&dattbl->orders_dat))
	;//return(-8);
    if (mpqloadfile(SC_SFXDATA_DAT,(char **)&dattbl->sfxdata_dat))
	;//return(-9);
    if (mpqloadfile(SC_MAPDATA_DAT,(char **)&dattbl->mapdata_dat))
	;//return(-10);
    dattbl->images_tbl = new TBL;
    dattbl->images_tbl->loadTBL(SC_IMAGES_TBL);

    dattbl->portdata_tbl = new TBL;
    dattbl->portdata_tbl->loadTBL(SC_PORTDATA_TBL);

    dattbl->sfxdata_tbl = new TBL;
    dattbl->sfxdata_tbl->loadTBL(SC_SFXDATA_TBL);

    dattbl->stattxt_tbl = new TBL;
    dattbl->stattxt_tbl->loadTBL(SC_STATTXT_TBL);

    dattbl->gluall_tbl = new TBL;
    dattbl->gluall_tbl->loadTBL(SC_GLUALL_TBL);
    
    dattbl->network_tbl = new TBL;
    dattbl->network_tbl->loadTBL(SC_NETWORK_TBL);

    dattbl->mapdata_tbl = new TBL;
    dattbl->mapdata_tbl->loadTBL(SC_MAPDATA_TBL);

    dattbl->myinfo_tbl = new TBL;
    dattbl->myinfo_tbl->loadTBL(SC_MYINFO_TBL);
    
    dattbl->campaign_tbl = new TBL;
    dattbl->campaign_tbl->loadTBL(SC_CAMPAIGN_TBL);
    
    return(0);
}
//============================================
void UnloadDatTblFiles(DATTBLSTRUCT *dattbl)
{
    if (dattbl->images_dat)
    {
	wfree(dattbl->images_dat);
	dattbl->images_dat=NULL;
    }
    if (dattbl->sprites_dat)
    {
	wfree(dattbl->sprites_dat);
	dattbl->sprites_dat=NULL;
    }
    if (dattbl->units_dat)
    {
	wfree(dattbl->units_dat);
	dattbl->units_dat=NULL;
    }
    if (dattbl->flingy_dat)
    {
	wfree(dattbl->flingy_dat);
	dattbl->flingy_dat=NULL;
    }
    if (dattbl->portdata_dat)
    {
	wfree(dattbl->portdata_dat);
	dattbl->portdata_dat=NULL;
    }
    if (dattbl->weapons_dat)
    {
	wfree(dattbl->weapons_dat);
	dattbl->weapons_dat=NULL;
    }
    if (dattbl->upgrades_dat)
    {
	wfree(dattbl->upgrades_dat);
	dattbl->upgrades_dat=NULL;
    }
    if (dattbl->techdata_dat)
    {
	wfree(dattbl->techdata_dat);
	dattbl->techdata_dat=NULL;
    }
    if (dattbl->orders_dat)
    {
	wfree(dattbl->orders_dat);
	dattbl->orders_dat=NULL;
    }
    if (dattbl->sfxdata_dat)
    {
	wfree(dattbl->sfxdata_dat);
	dattbl->sfxdata_dat=NULL;
    }
    if (dattbl->mapdata_dat)
    {
	wfree(dattbl->mapdata_dat);
	dattbl->mapdata_dat=NULL;
    }
    if (dattbl->images_tbl)
    {
	dattbl->images_tbl->~TBL();
	delete dattbl->images_tbl;
	dattbl->images_tbl=NULL;
    }
    if (dattbl->portdata_tbl)
    {
	dattbl->portdata_tbl->~TBL();
	delete dattbl->portdata_tbl;
	dattbl->portdata_tbl=NULL;
    }
    if (dattbl->sfxdata_tbl)
    {
	dattbl->sfxdata_tbl->~TBL();
	delete dattbl->sfxdata_tbl;
	dattbl->sfxdata_tbl=NULL;
    }
    if (dattbl->stattxt_tbl)
    {
	dattbl->stattxt_tbl->~TBL();
	delete dattbl->stattxt_tbl;
	dattbl->stattxt_tbl=NULL;
    }
    if (dattbl->gluall_tbl)
    {
	dattbl->gluall_tbl->~TBL();
	delete dattbl->gluall_tbl;
	dattbl->gluall_tbl=NULL;
    }
    if (dattbl->network_tbl)
    {
	dattbl->network_tbl->~TBL();
	delete dattbl->network_tbl;
	dattbl->network_tbl=NULL;
    }
    if (dattbl->mapdata_tbl)
    {
	dattbl->mapdata_tbl->~TBL();
	delete dattbl->mapdata_tbl;
	dattbl->network_tbl=NULL;
    }
    if (dattbl->myinfo_tbl)
    {
	dattbl->myinfo_tbl->~TBL();
	delete dattbl->myinfo_tbl;
	dattbl->myinfo_tbl=NULL;
    }
    if (dattbl->campaign_tbl)
    {
	dattbl->campaign_tbl->~TBL();
	delete dattbl->campaign_tbl;
	dattbl->campaign_tbl=NULL;
    }
}
//================================================
void LoadPatchTbl(void)
{
    alldattbl.weapons_dat->MaximumRange[WEAPONID_SCARAB] = 15;		//scarab need to be melee atack
    alldattbl.weapons_dat->WeaponCooldown[WEAPONID_SCARAB] = 60;	//reaver cooldown
    alldattbl.weapons_dat->WeaponCooldown[WEAPONID_PULSECANNON] = 10;	//interceptor cooldown
    alldattbl.units_dat->ElevationLevel[SC_REAVEROBJ] += 1;	//reaver elevation
    alldattbl.units_dat->ElevationLevel[SC_HERO_WARBRINGEROBJ] += 1;	//warbringer elevation
    alldattbl.flingy_dat->MoveControl[FLINGYID_GOLIATHTURRET] = FLINGYMOVECONTROL_ISCRIPT;	//goliath turret has flingy move control (why?)
    alldattbl.flingy_dat->TurnRadius[FLINGYID_HALOROCKETS] *= 2;	//doubel rotation speed to prevent freeze around
    alldattbl.units_dat->SpecialAbilityFlags[SC_SCANNERSWEEP] |= SPECIAL_INVINCIBLE;	//prevent attack sweep
    alldattbl.units_dat->SpecialAbilityFlags[SC_BUNKEROBJ] |= SPECIAL_FULLAUTOATTACK;	//units inside bunker can atack
    alldattbl.units_dat->SpecialAbilityFlags[SC_MAPREVEALEROBJ] |= SPECIAL_INVINCIBLE;	//prevent attack sweep
    
}
//============================================
void *imagesreftogrpdata[MAX_IMAGES_ELEM];
//===========================================
void Unload_SC_Images_List(void)
{
//    SC_Images_List.DeallocList(&FreeImagesListData);
    for (int i=0;i<MAX_IMAGES_ELEM;i++)
    {
	if (imagesreftogrpdata[i])
	    wfree(imagesreftogrpdata[i]);
	imagesreftogrpdata[i]=NULL;
    }
}
//===========================================
int GetLoadedImage(int mpqfilenr,int images_tbl,void **loadeddata)
{
    int err=0;
    void *tempdata = imagesreftogrpdata[images_tbl];
    if (tempdata==NULL)
    {
	if (images_tbl>0)
	{
	    strcpy(PREFIX_UNIT+PREFIX_UNIT_OFFSET,alldattbl.images_tbl->get_TBL_STR(images_tbl-1));
	    if (loadfilefrommpq(allmpq.GetElemNr(mpqfilenr),PREFIX_UNIT,(char **)&tempdata,NULL))
	    {
		err=-1;
	    }
	    else
	    {
		imagesreftogrpdata[images_tbl]=tempdata;
	    }
	}
	else
	    err=-1;
    }
    *loadeddata=tempdata;
    return(err);
}
//===========================================
//-1 error
//>=0 nr in list
//===========================================
int GetLoadedImage(int images_tbl,void **loadeddata)
{
    int err=0;
    void *tempdata = imagesreftogrpdata[images_tbl];
    if (tempdata==NULL)
    {
	if (images_tbl>0)
	{
	    strcpy(PREFIX_UNIT+PREFIX_UNIT_OFFSET,alldattbl.images_tbl->get_TBL_STR(images_tbl-1));
	    if (mpqloadfile(PREFIX_UNIT,(char **)&tempdata))
	    {
		err=-1;
	    }
	    else
	    {
		imagesreftogrpdata[images_tbl]=tempdata;
	    }
	}
	else
	    err=-1;
    }
    *loadeddata=tempdata;
    return(err);
}
//=======================================
void unloadtexturegrp(void)
{
    if (grptexture)
    {
	wfree(grptexture);
	grptexture=NULL;
	ctextures=0;
    }
}
//=======================================
void loadtexturegrp(void)
{
    GRPFILE *grpmem2;
    int images_tbl,muls,listnr,i;
    if (!grptexture)
    {
	images_tbl = alldattbl.images_dat->images_tbl[IMAGEID_TEXTURE];
	listnr = GetLoadedImage(images_tbl,(void **)&grpmem2);
	GRP_sizexwarppict=grpmem2->SizeX;
	GRP_sizeywarppict=grpmem2->SizeY;
	muls=GRP_sizexwarppict*GRP_sizeywarppict;
	ctextures=grpmem2->CountPictures;
	muls*=ctextures;
	grptexture=(char *)wmalloc(muls);
	for (i=0;i<ctextures;i++)
	{
	    unpackgrp(0,0,grpmem2,i,grptexture+GRP_sizexwarppict*GRP_sizeywarppict*i);
        }
//	SC_Images_List.FreeAndDelList(listnr,&FreeImagesListData);//no needed packed texture.grp from now
    }
}
//===========================================
char statbtn[]="rez\\statbtn?.bin";
#define STATBTN_RACEOFFSET 11
//===========================================
int loadbeforestarmap(int race)
{
    int images_id,images_tbl;
    int i;
    for (i=0;i<3;i++)
    {
	images_id=alldattbl.sprites_dat->images_id[resource_sprites_id[i][0]];
	images_tbl=alldattbl.images_dat->images_tbl[images_id];
	GetLoadedImage(images_tbl,(void **)&grpminmineral[i]);

	images_id=alldattbl.sprites_dat->images_id[resource_sprites_id[i][1]];
	images_tbl=alldattbl.images_dat->images_tbl[images_id];
	GetLoadedImage(images_tbl,(void **)&grpmingas[i]);
    }
    int err=readmageproperties();
    if (err<0)
    {
        printf("mage:Error in file nr:%d\n",err);
    }
    minimapmenu=DialogBin_GetItemCoords("rez\\minimap.bin");
    f10menu=DialogBin_GetItemCoords("rez\\stat_f10.bin");
    statbtn[STATBTN_RACEOFFSET]=RACE_CHAR[race];
    statbtnmenu=DialogBin_GetItemCoords(statbtn);
    if (!NETWORKGAME)
    {
	minimapmenu->iteminfo[MINIMAPDIALOG_TERRAINBUTTON].Flags|=DIALOGBIN_FLAGS_ITEMVISIBLED;
	minimapmenu->iteminfo[MINIMAPDIALOG_MESSAGEBUTTON].Flags&=~DIALOGBIN_FLAGS_ITEMVISIBLED;
	minimapmenu->iteminfo[MINIMAPDIALOG_DIPLOMACYBUTTON].Flags&=~DIALOGBIN_FLAGS_ITEMVISIBLED;
    }
    else
    {
	minimapmenu->iteminfo[MINIMAPDIALOG_TERRAINBUTTON].Flags&=~DIALOGBIN_FLAGS_ITEMVISIBLED;
	minimapmenu->iteminfo[MINIMAPDIALOG_MESSAGEBUTTON].Flags|=DIALOGBIN_FLAGS_ITEMVISIBLED;
	minimapmenu->iteminfo[MINIMAPDIALOG_DIPLOMACYBUTTON].Flags|=DIALOGBIN_FLAGS_ITEMVISIBLED;
    }
    if (loadpcxtables())
    {
        printf("error loading pcx tables\n");
    }
    printf("loading pcx (transparent and palette tables)...\n");
    if (loadpcxicons())
    {
        printf("error loading pcx icons\n");
    }
    loadtexturegrp();
}
//==========================
void unloadafterstarmap(void)
{
    unloadpcxicons();
    unloadpcxtables();
    DialogBin_FreeItemCoords(statbtnmenu);
    DialogBin_FreeItemCoords(f10menu);
    DialogBin_FreeItemCoords(minimapmenu);
    unloadcircles();
    unloadtexturegrp();
    Unload_SC_Images_List();
    freeOBJstructs(1);
    FreeQueues();
    freeMAGEstructs();
    DestroyCreepLists();
}
//==========================
#define MAXCAMPAIGNS	7
unsigned char mapdatatbl_raceoffset[MAXCAMPAIGNS]={1,12,22,32,40,49,255};
//==========================
int missionid2missiontbl(int campaignnr,int selected_id)
{
    int i,j,maxmissions;
    maxmissions = curplayer[0].missions[campaignnr].nrlinesinhisttbl;
    for (i=0,j=0;i<maxmissions;i++)
    {
	if (i == selected_id)
	    break;
	if ((curplayer[0].missions[campaignnr].seq_missions[i] & 0x7f) <= 0x3f)
	{
	    j++;
	}
    }
    return(j);
}
//==========================
int getcampaignname(int campaignnr,int selected_id)
{
    int fromtblnr,i,lines,missiontype,mission_tbl;
    char *campname;
    missiontype = curplayer[0].missions[campaignnr].seq_missions[selected_id];
    SELECTMAP[0]=0;
    if (missiontype ==0x00)
    {
	//this is endcampaign
	return(ENDCAMPAIGN);
    }
    if ((missiontype & 0x3f) == 0x3f)
    {
	//this is preview text
	return(PREVIEWTEXT);
    }
    if (missiontype & 0x40)
    {
	//this is video 
	return(SHOWVIDEO);
    }
    selected_id &= 0x3f;
    fromtblnr = mapdatatbl_raceoffset[campaignnr];
    mission_tbl = missionid2missiontbl(campaignnr,selected_id);
    strcpy(SELECTMAP,alldattbl.mapdata_tbl->get_TBL_STR(fromtblnr + mission_tbl));
    return(STARTGAME);
}
//==========================
void getmissionidformmissionname(char *mname,int *mission_id)
{
    int i,j,fromtblname,maxtblnr,fromtbl;
    char *tbltxt;
    char missionname[20];
    strcpy(missionname,mname);
    j=strlen(missionname);
    for (i=0;i<j;i++)
	if (missionname[i]=='.')
	{
	    missionname[i]=0;
	    break;
	}
    maxtblnr=alldattbl.mapdata_tbl->get_STRS()+1;
    fromtbl=mapdatatbl_raceoffset[campaign_id];
    for (i=fromtbl;i<maxtblnr;i++)
    {
	tbltxt=alldattbl.mapdata_tbl->get_TBL_STR(i);
	if (strcasestr(tbltxt,missionname))
	{
	    //find substring
	    for (j=0;j<MAXCAMPAIGNS;j++)
	    {
		if (i<mapdatatbl_raceoffset[j])
		{
		    if (mission_id)
			*mission_id=i-mapdatatbl_raceoffset[j-1];
		    return;
		}
	    }
	}
    }
}
//==========================
int openmission(int campaignnr,int nextmission_id)
{
    int retvalue;
    if (curplayer[0].missions[campaignnr].campaigndone)
	return(0x80);
    retvalue = curplayer[0].missions[campaignnr].seq_missions[nextmission_id] & 0x80;
    if (curplayer[0].missions[campaignnr].seq_missions[nextmission_id]==0x00)//no more mission
    {
	curplayer[0].missions[campaignnr].campaigndone = 1;
    }
    else
    {
	curplayer[0].missions[campaignnr].seq_missions[nextmission_id] |= 0x80;
    }
    saveplayerinfo();
    return(retvalue);
}
//==========================



