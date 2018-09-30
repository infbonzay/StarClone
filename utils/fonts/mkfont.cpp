
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <grplib/font.h>
#include "../../include/pcx.h"

#include "mkfont.h"

#define HOLEBYTE	0
#define FIRSTSYMB	'A'
//=============================================
SC_FontHeader fontHeader;
SC_FontLetterRaw (*letters)[];
unsigned char *letterBytes;
int *letterOffsets;
char *TEMPDIR;
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

//=============================================
unsigned char XLOffsetInLetter( unsigned char *pcxbytes,int pcxsizex,int pcxsizey,
				int x,int y,int lettersizex,int lettersizey)
{
    int xcnt=0;
    for (int j=0;j<lettersizex;j++,xcnt++)
    {
        for (int i=0;i<lettersizey;i++)
        {
	    if (pcxbytes[(y*lettersizey+i)*pcxsizex + x*lettersizex+j] != HOLEBYTE)
    		return(xcnt);
        }
    }
}
//=============================================
unsigned char XROffsetInLetter( unsigned char *pcxbytes,int pcxsizex,int pcxsizey,
				int x,int y,int lettersizex,int lettersizey)
{
    int xcnt=0;
    for (int j=lettersizex-1;j>=0;j--,xcnt++)
    {
        for (int i=0;i<lettersizey;i++)
        {
	    if (pcxbytes[(y*lettersizey+i)*pcxsizex + x*lettersizex+j] != HOLEBYTE)
    		return(xcnt);
        }
    }
}
//=============================================
unsigned char YLOffsetInLetter( unsigned char *pcxbytes,int pcxsizex,int pcxsizey,
				int x,int y,int lettersizex,int lettersizey)
{
    int ycnt=0;
    for (int i=0;i<lettersizey;i++,ycnt++)
    {
        for (int j=0;j<lettersizex;j++)
        {
	    if (pcxbytes[(y*lettersizey+i)*pcxsizex + x*lettersizex+j] != HOLEBYTE)
    		return(ycnt);
        }
    }
}
//=============================================
unsigned char YROffsetInLetter( unsigned char *pcxbytes,int pcxsizex,int pcxsizey,
				int x,int y,int lettersizex,int lettersizey)
{
    int ycnt=0;
    for (int i=lettersizey-1;i>=0;i--,ycnt++)
    {
        for (int j=0;j<lettersizex;j++)
        {
	    if (pcxbytes[(y*lettersizey+i)*pcxsizex + x*lettersizex+j] != HOLEBYTE)
    		return(ycnt);
        }
    }
}
//=============================================
int main(int argc,char *argv[])
{
    if (argc != 4)
    {
	printf("usage: %s font.pcx lettersizex lettersizey\n",argv[0]);
	return (-1);
    }
    FILE *f = fopen("out.fnt","wb");
    if (!f)
    {
	printf("error create font file \n");
	return(-2);
    }
    int lettersizex = atoi(argv[2]);
    int lettersizey = atoi(argv[3]);

    PCX *pcx = new PCX();
    int result = pcx->openMpqPcx(argv[1]);
    if (result != OKPCX)
    {
	printf("error open pcx %s\n",argv[1]);
	return(-2);
    }
    unsigned char *pcxbytes = (unsigned char *)pcx->GetPcxRawBytes();
    int pcxsizex = pcx->xsizePcx();
    int pcxsizey = pcx->ysizePcx(); 
    int totalLettersx = pcx->xsizePcx()/lettersizex;
    int totalLettersy = pcx->ysizePcx()/lettersizey;
    int totalLetters = totalLettersx * totalLettersy;
    
    fontHeader.Name = 0x544E4F46;	//'FONT';
    fontHeader.LowIndex = FIRSTSYMB;
    fontHeader.HighIndex = fontHeader.LowIndex + totalLetters - 1;
    fontHeader.MaxWidth = lettersizex;
    fontHeader.MaxHeight = lettersizey;

    letters = (SC_FontLetterRaw (*)[]) malloc(sizeof(SC_FontLetterRaw) * totalLetters);
    letterBytes = (unsigned char *)malloc(lettersizex * lettersizey);
    letterOffsets = (int *)malloc(totalLetters * 4);
    memset(letterOffsets,0,totalLetters*4);

    unsigned char prevbyte,nextbyte;

    fwrite(&fontHeader,sizeof(fontHeader),1,f);
    fwrite(letterOffsets,4,totalLetters,f);
    int k=0;
    for (int y=0;y<totalLettersy;y++)
    {
	for (int x=0;x<totalLettersx;x++)
	{
    	    int totalletterbytes = 0;
	    int xl,yl,xr,yr,xs,ys;
	    xl = XLOffsetInLetter(pcxbytes,pcxsizex,pcxsizey,x,y,lettersizex,lettersizey);
	    yl = YLOffsetInLetter(pcxbytes,pcxsizex,pcxsizey,x,y,lettersizex,lettersizey);
	    xr = XROffsetInLetter(pcxbytes,pcxsizex,pcxsizey,x,y,lettersizex,lettersizey);
	    yr = YROffsetInLetter(pcxbytes,pcxsizex,pcxsizey,x,y,lettersizex,lettersizey);
	    xs = lettersizex - xl - xr;
	    ys = lettersizey - yl - yr;
	    (*letters)[k].XOffset = xl;
	    (*letters)[k].YOffset = yl;
	    (*letters)[k].Width = xs;
	    (*letters)[k].Height = ys;
    	    nextbyte = HOLEBYTE;
	    int cnt=0;
	    int save = 0;
	    for (int i=0;i<ys;i++)
	    {
		for (int j=0;j<xs;j++)
		{
		    prevbyte = nextbyte;
		    nextbyte = pcxbytes[(y*lettersizey+i+yl)*pcxsizex + x*lettersizex+j+xl];
		    if (nextbyte == HOLEBYTE)
		    {
			cnt++;
			if (cnt >= 0x1f )
			{
			    save = 1;
			}
		    }
		    if (nextbyte != HOLEBYTE || save)
		    {
			int gradationcolor = 1;//calculate from pcxbytes
			letterBytes[totalletterbytes++] = (cnt<<3) | gradationcolor;
			cnt=0;
		    }
		}
	    }
	    letterOffsets[k] = ftell(f);
	    fwrite(&(*letters)[k],sizeof(SC_FontLetterRaw),1,f);
	    fwrite(letterBytes,totalletterbytes,1,f);
	    k++;
	}
    }
    fseek(f,sizeof(fontHeader),SEEK_SET);
    fwrite(letterOffsets,4,totalLetters,f);
    pcx->closePcx();
    fclose(f);
    free(letterOffsets);
    free(letterBytes);
    free(letters);
    return(0);
}
