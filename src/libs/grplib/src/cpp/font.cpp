
#include "grplib/grp.h"
#include "grplib/font.h"

int putfntsymbol(int x,int y,struct SC_FontHeader *font,int symbolnr,char *offsettable,int skipuplines,int skipdownlines)
{
    int i,j=0,linesskip,ypart;
    SC_FontLetterRaw *letter;
    unsigned char byte,color;
    unsigned char *letterbytes;
    if (font && symbolnr >= font->LowIndex && symbolnr <= font->HighIndex)
    {
	if (!font->Offset[symbolnr-font->LowIndex])
	    return 0;
	letter = (SC_FontLetterRaw *) (font->Offset[symbolnr-font->LowIndex] + (long) font);
	ypart = GRP_scanlineoffsets[y + letter->YOffset] + letter->XOffset + x;
	letterbytes = letter->FontBytes;
	j = 0;
	i = letter->Height;
	if (i > skipdownlines)
	do
	{
	    byte = *letterbytes++;
	    color = byte & 0x7;
	    j += byte >> 3;
	    if (j >= letter->Width)
	    {
		linesskip = j/letter->Width;
		skipuplines += linesskip;
		i -= linesskip;
		if (i <= skipdownlines)
		    break;
		j = j % letter->Width;
		ypart += GRP_screensizex * linesskip;
	    }
	    if (skipuplines >= 0)
	    {
		color = offsettable[color];
		GRP_vidmem[ypart + j] = color;
	    }
	    j++;
	}while(1);
        return (letter->XOffset + letter->Width);
    }
    else
	return 0;
}
//================================
