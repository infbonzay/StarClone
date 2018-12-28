#if !defined(_GRP_W)
#define _GRP_W

#include <stdio.h>

#pragma pack(push,1)

struct GRPPICTURE
{
       unsigned char SkipLeft;
       unsigned char SkipUp;
       unsigned char PixelPerLine;
       unsigned char LinesPerPicture;
       unsigned int  OffsetForLine;
};
struct GRPFILE
{
       unsigned short int CountPictures;
       unsigned short int SizeX;
       unsigned short int SizeY;
       struct GRPPICTURE Picture[];
};

#pragma pack(pop)
//=======================================
#ifdef __cplusplus
    extern "C" {
#endif

    void putgrp(long x,long y,struct GRPFILE *GRP,long nrpicture,long grcolor);
    void iputgrp(long x,long y,struct GRPFILE *GRP,long nrpicture,long grcolor);

    void putgrptransp(long x,long y,struct GRPFILE *GRP,long nrpicture,long grcolor);
    void iputgrptransp(long x,long y,struct GRPFILE *GRP,long nrpicture,long grcolor);
    void putgrpshadow(long x,long y,struct GRPFILE *GRP,long nrpicture,char *table,long grdfactor);
    void iputgrpshadow(long x,long y,struct GRPFILE *GRP,long nrpicture,char *table,long grdfactor);
    void putgrpnonvis(long x,long y,struct GRPFILE *GRP,long nrpicture);
    void iputgrpnonvis(long x,long y,struct GRPFILE *GRP,long nrpicture);
    void putgrpgrd(long x,long y,struct GRPFILE *GRP,long nrpicture,char *table);
    void iputgrpgrd(long x,long y,struct GRPFILE *GRP,long nrpicture,char *table);
    void putgrptotable(long x,long y,struct GRPFILE *GRP,long nrpicture,char *table,long grdfactor,long grcolor);
    void iputgrptotable(long x,long y,struct GRPFILE *GRP,long nrpicture,char *table,long grdfactor,long grcolor);
    void putgrpwarpfromtext(long x,long y,struct GRPFILE *GRP,long nrpicture,char *adrtext);
    void putspr32x32(long x,long y,char *bytes);
    void putfog32x32(long x,long y,char *table,long lt,long rt,long lb,long rb);
    void putgrp_nopacked(int x,int y,struct GRPFILE *grp,int pictnr);

    extern int     GRP_wminx,GRP_wminy,GRP_wmaxx,GRP_wmaxy;
    extern int     GRP_screensizex,GRP_screensizey;
    extern int     GRP_sizexwarppict,GRP_sizeywarppict;
    extern char    *GRP_vidmem;
    extern char    *GRP_transpcolors;
    extern int     *GRP_scanlineoffsets;
    extern char    *GRP_tableforunitcolor;
    extern short   int GRP_wcomandcolor,GRP_wcomcolor;
    extern unsigned char GRP_gr_color,GRP_gr_maxcolordesen;
    extern int     GRP_wmaxdwordwritel;

//===========================================
void recalcmulyy(void);
int  wviewport(int x,int y,int sizex,int sizey);
int  loadgrp(char *filename,struct GRPFILE **grpmem);
void freegrp(struct GRPFILE *a);
int  InitGrpLib(int sizex,int sizey);
int  ResetGrpLib(int sizex,int sizey);
void *SetVideoBuffer(void *memory);
void QuitGrpLib(void);
void unpackgrp(int x,int y,struct GRPFILE *GRP,int nrpict,char *mem);
void SetTranspTable(char *table);
int  SetUserPlayerColors(int fromplayer,int nrplayers,char *table);
int  SetPlayerColors(int fromplayer,int nrplayers,
		     int firstindex,int nrofindexes,
		     char *table);

//return 0x00aabbcc
//aa - Major Version
//bb - Minor Version
//cc - Patch Version

#define GRPLIBVER
int GetGRPLibVer(void);

#ifdef __cplusplus
    }
#endif

#endif /*   _GRP_W   */

