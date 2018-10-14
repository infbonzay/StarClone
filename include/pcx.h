#if !defined(_PCX_W)
#define _PCX_W

#include "mpq.h"

#define PCX_EMPTYCOLOR1		0
#define PCX_EMPTYCOLOR2		255

#define OKPCX       	0
#define NOFILE      	1
#define NOPCX       	2
#define NO8BITPCX   	3
#define NOMEMORY    	4
#define NOMEMORY2   	5
#define BADFILE     	6

#define PCXSHOW_25	0
#define PCXSHOW_50	1
#define PCXSHOW_75	2
#define PCXSHOW_100	3

#pragma pack(push,1)

struct RGB
{
    unsigned char  Red;
    unsigned char  Green;
    unsigned char  Blue;
};
struct PCXH
{
    unsigned char  Flag;
    unsigned char  Version;
    unsigned char  Encoding;
    unsigned char  BitsPerPixel;
    unsigned short xmin;
    unsigned short ymin;
    unsigned short xmax;
    unsigned short ymax;
    unsigned short HDPI;
    unsigned short VDPI;
    RGB            Colormap[16];
    unsigned char  Reserved;
    unsigned char  NPlanes;
    unsigned short BytesPerLine;
    unsigned short PaletteInfo;
    unsigned short HscreenSize;
    unsigned short VscreenSize;
    unsigned char  Filler[54];
};
#pragma pack(pop)
class PCX
{
private:
    PCXH header;
    long xsize;
    long ysize;
    long lsize;
    char *adrpict;
    char *adrpal;

    void encgetMPQ(int *pbyt, int *pcnt, void *f);
public:
    PCX(void);
    ~PCX(void);
    int  openMpqPcx(const char *filepcx,HANDLE hmpq);
    int  openMpqPcx(const char *filepcx);
    void readBytesFromPcx(char *dest);
    void readBytesFromPcx(char *dest,int lsize);
    void readPalFromPcx(char *dest);
    void readPalFromPcx(char *dest,int fourbytepalette);
    void CreatePcx(int sizex,int sizey);
    void closePcx(void);
    char *GetPcxRawBytes(void);
    char *GetRawPal768(void);

    long int  sizePcx(void);
    long int  xsizePcx(void);
    long int  ysizePcx(void);
    void PutPcx(int x,int y,unsigned char emptycolor);
    void PutPcx(int x,int y,unsigned char emptycolor,unsigned char transparentcolor,int grdnr);
    void PutPcx(int x,int y,int showx,unsigned char emptycolor,unsigned char transparentcolor,int grdnr);
    void PutPcx(int x,int y,int fromlines,int showinglines,unsigned char emptycolor,unsigned char transparentcolor,int grdnr);
    void PutPcx(int x,int y,int showx,int fromlines,int showinglines,unsigned char emptycolor,unsigned char transparentcolor,int grdnr);
    int  IfPcxIsReady(void);
    void PutScaledPcx(int x,int y,char emptycolor);
};
//==================
#endif   /* _PCX_W  */
