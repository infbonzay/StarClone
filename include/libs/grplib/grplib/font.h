#if !defined(_GRP_FONT)
#define _GRP_FONT

#include <stdio.h>

#pragma pack(push,1)

struct SC_FontHeader
{
    int				Name;		//Always is "FONT"
    unsigned char	LowIndex;	//nr of first letter in file
    unsigned char	HighIndex;	//nr of the last letter in file
    unsigned char	MaxWidth;	//Maximum width of letters
    unsigned char	MaxHeight;	//Maximum height of letters
    int				Offset[];	//offset of each letter SC_FontLetterRaw from begin of file
};

struct  SC_FontLetterRaw
{
    unsigned char	Width;		//Width of the letter
    unsigned char	Height;		//Height of the letter
    unsigned char	XOffset;	//X Offset for the topleft corner of the letter.
    unsigned char	YOffset;	//Y Offset for the topleft corner of the letter.
    unsigned char	FontBytes[];	//this is all bytes for this letter
};

#pragma pack(pop)
//=======================================
#ifdef __cplusplus
    extern "C" {
#endif

    int  GRP_PutFntSymbol(int x,int y,struct SC_FontHeader *font,int symbolnr,char *offsettable,int skipuplines,int skipdownlines);

#ifdef __cplusplus
    }
#endif

#endif /*   _GRP_FONT   */

