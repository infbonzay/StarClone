
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "pcx.h"
#include "auxil.h"
#include <grplib/grp.h>
#include <grplib/font.h>

char *TEMPDIR;
char *GAMEPATH;
struct GAMECONF
{

}gameconf;

void CreateCRC(unsigned char *buf,int a,int b)
{
}
void LockAudio(void)
{
}
void UnlockAudio(void)
{
}

//================================
SC_FontHeader *myfont;
SC_FontHeader *loadfont(char *filename)
{
    long fs = FILElength(filename);
    FILE *f = fopen(filename,"r");
    if (!f)
	return NULL;
    char *mem = (char *)malloc(fs);
    fread(mem,fs,1,f);
    fclose(f);
    if (mem[0]=='F' && mem[1]=='O' && mem[2]=='N' && mem[3]=='T' )
    {
	return (SC_FontHeader *)mem;
    }
    free(mem);
    return(NULL);
}
//=============================================
PCXH pcxh;

unsigned char C1 = 0xc1;
char pal[256*3];
int main(int argc,char *argv[])
{
    if (argc != 3)
    {
	printf("usage: %s fontfile.fnt colortablefile.pcx\n",argv[0]);
	return (-1);
    }
    SC_FontHeader *myfont  = loadfont(argv[1]);
    if (!myfont)
    {
	printf("error open font file \n");
	return(-2);
    }
    PCX *pcx = new PCX();
    int result = pcx->openMpqPcx(argv[2]);
    if (result != OKPCX)
    {
	printf("error open pcx %s\n",argv[2]);
	return(-3);
    }
    InitGrpLib(myfont->MaxWidth*16,1024);
    GRP_vidmem = (char *) malloc(GRP_screensizex*GRP_screensizey);
    memset(GRP_vidmem,0,GRP_screensizex*GRP_screensizey);
    int x=0,y=0;
    for (int i=myfont->LowIndex;i<=myfont->HighIndex;i++)
    {
	putfntsymbol(x,y,myfont,i,pcx->GetPcxRawBytes(),0,0);
	x += myfont->MaxWidth;
	if (x >= myfont->MaxWidth*16)
	{
	    x = 0;
	    y += myfont->MaxHeight;
	}
    }
    memcpy(pal,pcx->GetRawPal768(),3*256);
    FILE *f = fopen("out.pcx","wb");
    if (!f)
    {
	printf("error open for write\n");
	return(-4);
    }
    pcxh.Flag = 0x0a;
    pcxh.Version = 2;
    pcxh.Encoding = 1;
    pcxh.BitsPerPixel = 8;
    pcxh.NPlanes = 1;
    pcxh.xmax = myfont->MaxWidth*16-1;
    pcxh.ymax = 1024-1;
    pcxh.HDPI = 300;
    pcxh.VDPI = 300;
    pcxh.BytesPerLine = myfont->MaxWidth*16;
    pcxh.PaletteInfo = 2;
    pcxh.HscreenSize = myfont->MaxWidth*16;
    pcxh.VscreenSize = 1024;
    fwrite(&pcxh,sizeof(PCXH),1,f);
    for (int y=0;y<1024;y++)
    {
	for (int x=0;x<myfont->MaxWidth*16;x++)
	{
	    fwrite(&C1,1,1,f);
	    fwrite(&GRP_vidmem[y * myfont->MaxWidth + x],1,1,f);
	}
    }
    //fwrite(&pal,1,1,f);
    fwrite(&pal,3*256,1,f);
    fclose(f);
    QuitGrpLib();
    pcx->closePcx();

    return(0);
}
