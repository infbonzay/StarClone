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

    void GRP_Put(long x,long y,struct GRPFILE *GRP,long nrpicture,long grcolor);
    void GRP_IPut(long x,long y,struct GRPFILE *GRP,long nrpicture,long grcolor);

    void GRP_PutTransp(long x,long y,struct GRPFILE *GRP,long nrpicture,long grcolor);
    void GRP_IPutTransp(long x,long y,struct GRPFILE *GRP,long nrpicture,long grcolor);
    void GRP_PutShadow(long x,long y,struct GRPFILE *GRP,long nrpicture,char *table,long grdfactor);
    void GRP_IPutShadow(long x,long y,struct GRPFILE *GRP,long nrpicture,char *table,long grdfactor);
    void GRP_PutInv(long x,long y,struct GRPFILE *GRP,long nrpicture);
    void GRP_IPutInv(long x,long y,struct GRPFILE *GRP,long nrpicture);
    void GRP_PutGrd(long x,long y,struct GRPFILE *GRP,long nrpicture,char *table);
    void GRP_IPutGrd(long x,long y,struct GRPFILE *GRP,long nrpicture,char *table);
    void GRP_PutByTable(long x,long y,struct GRPFILE *GRP,long nrpicture,char *table,long grdfactor,long grcolor);
    void GRP_IPutByTable(long x,long y,struct GRPFILE *GRP,long nrpicture,char *table,long grdfactor,long grcolor);
    void GRP_PutWithTexture(long x,long y,struct GRPFILE *GRP,long nrpicture,char *adrtext);
    void GRP_PutRow32x32(long x,long y,char *bytes);
    void GRP_PutFog32x32(long x,long y,char *table,long lt,long rt,long lb,long rb);
    void GRP_PutNoPacked(int x,int y,struct GRPFILE *grp,int pictnr);

    extern int     GRP_wminx,GRP_wminy,GRP_wmaxx,GRP_wmaxy;
    extern int     GRP_wsizex,GRP_wsizey;
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
void GRP_recalcscanlines(void);
int  GRP_viewport(int x,int y,int sizex,int sizey);
int  GRP_Load(char *filename,struct GRPFILE **grpmem);
void GRP_Free(struct GRPFILE *a);
int  GRP_InitLib(int sizex,int sizey);
int  GRP_ResetLib(int sizex,int sizey);
void *GRP_SetVideoBuffer(void *memory);
void GRP_QuitLib(void);
void GRP_Unpack(int x,int y,struct GRPFILE *GRP,int nrpict,char *mem);
void GRP_SetTranspTable(char *table);
int  GRP_SetUserPlayerColors(int fromplayer,int nrplayers,char *table);
int  GRP_SetPlayerColors(int fromplayer,int nrplayers,
						 int firstindex,int nrofindexes,
						 char *table);

#define GRP_VERSION	0x00000414		//0.4.20

#ifdef __cplusplus
    }
#endif

#endif /*   _GRP_W   */

