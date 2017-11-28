
#include <stdio.h>
#include <string.h>
#include <grplib/gr8.h>

#include "auxil.h"
#include "vars.h"
#include "mpq.h"
#include "gr.h"
#include "pcx.h"
#include "fonts.h"

#define SPACEYLETTERS	0
//	fontsizes         8 10 12 14 16 16x 32 58
int spacesize[MAXFONTS]={2,2,4,6,7,8,12,15};
int spaceletters[MAXFONTS]={1,1,1,1,1,1,3,5};

int COMMANDSYMB_SIZES[TOTALCOMMANDSYMBS]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,1,3,1};

PCX dlgfilediriconspcx[5];

//================================
SC_FontHeader *loadfont(char *filename)
{
    char *mem = NULL;
    mpqloadfile(filename,&mem);
    if (mem[0]=='F' && mem[1]=='O' && mem[2]=='N' && mem[3]=='T' )
    {
	return (SC_FontHeader *)mem;
    }
    unloadfont((SC_FontHeader *)mem);
    return(NULL);
}
//================================
void unloadfont(SC_FontHeader *font)
{
    if (font)
	wfree(font);
}
//================================
void getmaxsymbolsize(int fontnr,int *sizex,int *sizey)
{
    if (sizex)
	*sizex=fonts[fontnr]->MaxWidth;
    if (sizey)
	*sizey=fonts[fontnr]->MaxHeight;
}
//================================
int putfntsymbol(int x,int y,int fontnr,int symbolnr,char *offsettable,int skipuplines,int skipdownlines)
{
    int i,j=0,pos=0,ypart,linesskip;
    SC_FontHeader *font = fonts[fontnr];
    SC_FontLetterRaw *letter;
    unsigned char byte,color;
    if (!font)
	return 0;
    if (symbolnr>=font->LowIndex&&symbolnr<=font->HighIndex)
    {
	if (!font->Offset[symbolnr-font->LowIndex])
	    return 0;
	letter = (SC_FontLetterRaw *) (font->Offset[symbolnr-font->LowIndex] + (long) font);
	ypart = (y+letter->YOffset)*GRP_screensizex+letter->XOffset+x;
	
	j=0;
	i=letter->Height;
	if (i>skipdownlines)
	do
	{
	    byte = letter->bytes[pos++];
	    color = byte&0x7;
	    j += byte >> 3;
	    if (j>=letter->Width)
	    {
		linesskip=j/letter->Width;
		skipuplines+=linesskip;
		i-=linesskip;
		if (i<=skipdownlines)
		    break;
		j=j%letter->Width;
		ypart+=GRP_screensizex*linesskip;
	    }
	    if (skipuplines>=0)
	    {
		color = offsettable[color];
		GRP_vidmem[ypart+j] = color;
	    }
	    j++;
	}while(1);
    }
    return (letter->XOffset+letter->Width);
}
//================================
int putglowtext(int x,int y,int fontnr,char *str,int fromcolor,char *table,int glowfactor)
{
    PCX *pcx;
    char symb,maincolor;
    int i,j,sizex=0,sizey=0,color,changes=0;
    j = strlen(str);
    color = (fromcolor-2) & 0x7;
    maincolor=color;
    for (i=0;i<j;i++)
    {
	symb=str[i];
	if (symb<' ')
	{
	    changes++;
	    switch(symb)
	    {
		case DEFAULTCOLORFONT:
		    color=maincolor;
		    continue;
		case '\n':
		    sizey+=fonts[fontnr]->MaxHeight;
		    sizex=0;
		    continue;
		case 0:
		case 1:
		    color=0;
		    continue;
		case STATICCOLOR1:
		    color=symb-2;
		    continue;
		case 2:
		case 3:
		case 4:
		case 6:
		case 7:
		    color=symb-2;
		    continue;
	    }
	}
	else
	    if (symb==' ')
	    {
		sizex += spacesize[fontnr] + spaceletters[fontnr];
	    }
	    else
		if (color==maincolor&&changes!=1)
		    sizex += putfntsymbol(x+sizex,y+sizey,fontnr,symb,table+color*8+glowfactor,0,0)+spaceletters[fontnr];
		else
		    sizex += putfntsymbol(x+sizex,y+sizey,fontnr,symb,table+color*8+0,0,0)+spaceletters[fontnr];
		
    }
    return(sizex);
}
//================================
int getrowmessagesize(int fontnr,char *str,int sizestr)
{
    int i,szx=0;
    char symbolnr;
    SC_FontLetterRaw *letter;
    SC_FontHeader *font = fonts[fontnr];
    if (!font)
	return(0);
    for (i=0;i<sizestr;i++)
    {
	symbolnr=str[i];
	if (symbolnr<' ')
	    continue;
	if (symbolnr==' ')
	{
	    szx += spacesize[fontnr];
	}
	else
	{
	    letter = (SC_FontLetterRaw *) (font->Offset[symbolnr-font->LowIndex] + (long) font);
	    szx += letter->XOffset+letter->Width;
	}
	if (i<sizestr-1)//at last symbol we do not add spacesize
	    szx += spaceletters[fontnr];
    }
    return(szx);
}
//================================
void putrowtext(int x,int y,int sizewinx,int fontnr,int flags,char *str,int fromcolor,char *table)
{
    int i,j,sizex,offset;
    char symbolnr;
    j=strlen(str);
    offset=((fromcolor-2) & 0x7) * 8;
    if (flags==TEXT_CENTERALIGN)
	sizex=(sizewinx-getrowmessagesize(fontnr,str,j))/2;
    else
	if (flags==TEXT_RIGHTALIGN)
	    sizex=sizewinx-getrowmessagesize(fontnr,str,j);
	else
	    sizex=0;
    for (i=0;i<j;i++)
    {
	symbolnr=str[i];

	if (symbolnr < 2)
	{
	    offset=0;
	    continue;
	}
	if (symbolnr < ' ')
	{
	    offset=((symbolnr-2) & 0x7) * 8;
	    continue;
	}
	if (symbolnr==' ')
	{
	    sizex += spacesize[fontnr] + spaceletters[fontnr];
	}
	else
	    sizex += putfntsymbol(x+sizex,y,fontnr,symbolnr,table+offset,0,0)+spaceletters[fontnr];
    }
}
//================================
int putmessage(int x,int y,int fontnr,char *str,int fromcolor,char *table,GRPFILE *dlggrp)
{
    PCX *pcx;
    char symb,onlyonecolor=0;
    unsigned char *adrrowbytes;
    int pictnr;
    int i,j,sizex=0,sizey=0,color,colorcube,maincolor,deltax,deltay,cubex,cubey;
    j = strlen(str);
    color = (fromcolor-2) & 0x7;
//    if (color==STATICCOLOR1)
//	onlyonecolor=1;
    maincolor=color;
    colorcube=0;
    for (i=0;i<j;i++)
    {
	symb=str[i];
	if (symb<' ')
	{
	    switch(symb)
	    {
		case DEFAULTCOLORFONT:
//		    if (!onlyonecolor)
			color=maincolor;
		    continue;
		case '\n':
		    sizey+=fonts[fontnr]->MaxHeight;
		    sizex=0;
		    continue;
		case 0:
		case 1:
		    color=0;
		    continue;
		case STATICCOLOR1:
		    color=symb-2;
//		    onlyonecolor=1;
		    continue;
		case 2:
		case 3:
		case 4:
		case 6:
		case 7:
//		    if (!onlyonecolor)
			color=symb-2;
		    continue;
		case COMMANDSYMB:
		    i++;
		    symb=str[i];
		    switch(symb)
		    {
			case PSISYMBZ:
			    adrrowbytes=iresp[ZERGRACE][2];
			    break;
			case PSISYMBT:
			    adrrowbytes=iresp[TERRANRACE][2];
			    break;
			case PSISYMBP:
			    adrrowbytes=iresp[PROTOSSRACE][2];
			    break;
			case MINERALSYMB:
			    adrrowbytes=iresp[gameconf.pl_race[NUMBGAMER]][0];
			    break;
			case GASSYMB:
			    adrrowbytes=iresp[gameconf.pl_race[NUMBGAMER]][1];
			    break;
			case MANASYMB:
			    adrrowbytes=imanap;
			    break;
			case EMPTYCUBE14X14:
			    adrrowbytes=iemptycubep;
			    i++;
			    colorcube=str[i];
			    break;
			case CUBESYMB:
			    cubex=str[i+1];
			    cubey=str[i+2];
			    i+=3;
			    putcube(x+sizex,y+sizey,cubex,cubey,str[i]);
			    sizex+=cubex;
			    continue;
			case ICONFOROPENDIR:
			case ICONFORCLOSENOSELDIR:
			case ICONFORCLOSESELDIR:
			case ICONFORSELFILE:
			case ICONFORNOSELFILE:
			    pcx=&dlgfilediriconspcx[symb-ICONFOROPENDIR];
			    if (pcx->IfPcxIsReady())
			    {
			        pcx->PutPcx(x+sizex,y+sizey,PCX_EMPTYCOLOR1);
			        sizex+=pcx->xsizePcx()+2;
			    }
			    continue;
			case CHANGEFONTSYMB:
			    i++;
			    fontnr=str[i];
			    continue;
			case DLGGRPSYMB:		//commsymb,dlggrpsymb,grpnr,deltax,deltay
			    if (dlggrp)
			    {
				putgrp_nopacked(x+sizex+str[i+2],y+sizey+str[i+3],dlggrp,str[i+1]);
				sizex+=dlggrp->Picture[str[i+1]].PixelPerLine;
			    }
			    i+=COMMANDSYMB_SIZES[DLGGRPSYMB];
			    continue;
		    }
		    break;
		default:
		    continue;
	    }
	    if (!colorcube)
		putrow(x+sizex,y+sizey,14,14,adrrowbytes);
	    else
	    {
		colorcube--;
		putrowwithtable(x+sizex,y+sizey,14,14,adrrowbytes,GRP_tableforunitcolor+(colorcube<<8));
		colorcube=0;
	    }
    	    sizex+=14;
	}
	else
	    if (symb==' ')
	    {
		sizex += spacesize[fontnr] + spaceletters[fontnr];
	    }
	    else
		sizex += putfntsymbol(x+sizex,y+sizey,fontnr,symb,table+color*8,0,0)+spaceletters[fontnr];
    }
    return(sizex);
}
//================================
int getlinesintext(int fontnr,char *mes,int meslen,int sizexrect)
{
    int i,j,k,from,sizexmes,showstr;
    showstr=0;
    from=0;
    sizexmes=0;
    for (i=0;i<=meslen;i++)
    {
	if (mes[i]==COMMANDSYMB)
	{
	    i+=COMMANDSYMB_SIZES[mes[i+1]];
	    continue;
	}
	else
	    if (mes[i]=='\n' || mes[i]==0 )
	    {
		showstr++;
		from = i+1;
	    }
	if (mes[i]==' ' || mes[i]==0 || mes[i]=='\n')
	{
	    getmessagesizex(fontnr,mes+from,i-from,&sizexmes,NULL);
	    if (sizexmes>sizexrect)
	    {
		i--;
		for (;i>=0;i--)
		    if (mes[i]==' ')
		    {
			showstr++;
			from = i+1;
			break;
		    }
	    }
	}
    }
    return(showstr);
}
//================================
int putmessage(int x,int y,int fontnr,char *str,int fromcolor,char *table,GRPFILE *dlggrp,int skipup,int skipdown)
{
    PCX *pcx;
    char symb,onlyonecolor=0;
    unsigned char *adrrowbytes;
    int pictnr;
    int i,j,sizex=0,sizey=0,color,colorcube,maincolor,deltax,deltay,cubex,cubey;
    j = strlen(str);
    color = (fromcolor-2) & 0x7;
//    if (color==STATICCOLOR1)
//	onlyonecolor=1;
    maincolor=color;
    colorcube=0;
    for (i=0;i<j;i++)
    {
	symb=str[i];
	if (symb<' ')
	{
	    switch(symb)
	    {
		case DEFAULTCOLORFONT:
//		    if (!onlyonecolor)
			color=maincolor;
		    continue;
		case '\n':
		    sizey+=fonts[fontnr]->MaxHeight;
		    sizex=0;
		    continue;
		case 0:
		case 1:
		    color=0;
		    continue;
		case STATICCOLOR1:
		    color=symb-2;
//		    onlyonecolor=1;
		    continue;
		case 2:
		case 3:
		case 4:
		case 6:
		case 7:
//		    if (!onlyonecolor)
			color=symb-2;
		    continue;
		case COMMANDSYMB:
		    i++;
		    symb=str[i];
		    switch(symb)
		    {
			case PSISYMBZ:
			    adrrowbytes=iresp[ZERGRACE][2];
			    break;
			case PSISYMBT:
			    adrrowbytes=iresp[TERRANRACE][2];
			    break;
			case PSISYMBP:
			    adrrowbytes=iresp[PROTOSSRACE][2];
			    break;
			case MINERALSYMB:
			    adrrowbytes=iresp[gameconf.pl_race[NUMBGAMER]][0];
			    break;
			case GASSYMB:
			    adrrowbytes=iresp[gameconf.pl_race[NUMBGAMER]][1];
			    break;
			case MANASYMB:
			    adrrowbytes=imanap;
			    break;
			case EMPTYCUBE14X14:
			    adrrowbytes=iemptycubep;
			    i++;
			    colorcube=str[i];
			    break;
			case CUBESYMB:
			    cubex=str[i+1];
			    cubey=str[i+2];
			    i+=3;
			    putcube(x+sizex,y+sizey,cubex,cubey,str[i]);
			    sizex+=cubex;
			    continue;
			case ICONFOROPENDIR:
			case ICONFORCLOSENOSELDIR:
			case ICONFORCLOSESELDIR:
			case ICONFORSELFILE:
			case ICONFORNOSELFILE:
			    pcx=&dlgfilediriconspcx[symb-ICONFOROPENDIR];
			    if (pcx->IfPcxIsReady())
			    {
			        pcx->PutPcx(x+sizex,y+sizey,PCX_EMPTYCOLOR1);
			        sizex+=pcx->xsizePcx()+2;
			    }
			    continue;
			case CHANGEFONTSYMB:
			    i++;
			    fontnr=str[i];
			    continue;
			case DLGGRPSYMB:		//commsymb,dlggrpsymb,grpnr,deltax,deltay
			    if (dlggrp)
			    {
				putgrp_nopacked(x+sizex+str[i+2],y+sizey+str[i+3],dlggrp,str[i+1]);
				sizex+=dlggrp->Picture[str[i+1]].PixelPerLine;
			    }
			    i+=COMMANDSYMB_SIZES[DLGGRPSYMB];
			    continue;
		    }
		    break;
		default:
		    continue;
	    }
	    if (!colorcube)
		putrow(x+sizex,y+sizey,14,14,adrrowbytes);
	    else
	    {
		colorcube--;
		putrowwithtable(x+sizex,y+sizey,14,14,adrrowbytes,GRP_tableforunitcolor+(colorcube<<8));
		colorcube=0;
	    }
	    sizex+=14;
	}
	else
	    if (symb==' ')
	    {
		sizex += spacesize[fontnr] + spaceletters[fontnr];
	    }
	    else
		sizex += putfntsymbol(x+sizex,y+sizey,fontnr,symb,table+color*8,skipup,skipdown)+spaceletters[fontnr];
    }
    return(sizex);
}
//================================
int getlettersizexy(int fontnr,int symbolnr,int *sizex,int *sizey)
{
    int pictnr,szx,szy;
    SC_FontLetterRaw *letter;
    SC_FontHeader *font = fonts[fontnr];
    if (!font)
	return(0);
    if (symbolnr>0xff)
    {
	switch(symbolnr&0xff)
	{
	    case PSISYMBZ:
	    case PSISYMBT:
	    case PSISYMBP:
	    case MINERALSYMB:
	    case GASSYMB:
	    case MANASYMB:
	    case EMPTYCUBE14X14:
		szx=14;
		szy=14;
		break;
	    case CUBESYMB:
		szx=16;
		szy=16;
		break;
	    default:
		szx=0;
		szy=0;
		break;
	}
    }
    else if ((symbolnr)<=' ')
    {
	if (symbolnr<=MAXCOLORFONT)
	{
	    szx=0;
    	    szy=font->MaxHeight;
	}
	else if (symbolnr==' ')
	    {
		szx = spacesize[fontnr];
		szy = font->MaxHeight;
	    }
	    else
	    {
		szx=0;
		szy=0;
	    }
    }
    else if (symbolnr>=font->LowIndex&&symbolnr<=font->HighIndex)
    {
	    if (!font->Offset[symbolnr-font->LowIndex])
	    {
		szx=0;
		szy=0;
	    }
	    else
	    {
		letter = (SC_FontLetterRaw *) (font->Offset[symbolnr-font->LowIndex] + (long) font);
		szx = letter->XOffset+letter->Width;
		szy = letter->YOffset+letter->Height;
	    }
    }
    if (sizex)
        *sizex=szx;
    if (sizey)
	*sizey=szy;
    return(szx);
}
//================================
int getspacebetweensymbols(int fontnr)
{
    return(spaceletters[fontnr]);
}
//================================
int getmessagesizex(int fontnr,char *str,int sizestr,int *lenx,int *leny)
{
//    SC_FontHeader *font = fonts[fontnr];
    int i,j,sizex=0,sizey=0;
    int sizexlen=0,sizeylen=0,maxsizexlen=0,maxsizeylen=0;
    j = sizestr;
    for (i=0;i<j;i++)
    {
	if (str[i]<=MAXCOLORFONT)
	    continue;
	else
	    if(str[i]=='\r'||str[i]=='\v')
	        continue;
	    else
	    {
		    if(str[i]=='\n')
		    {
			if (sizexlen>maxsizexlen)
			    maxsizexlen=sizexlen;
			sizexlen=0;
			maxsizeylen+=sizeylen;
			continue;
		    }
		    else
		    {
    			if (str[i]==COMMANDSYMB)
    			{
			    if (str[i+1]==CHANGEFONTSYMB)
			    {
				fontnr=str[i+2];
				i+=2;
				continue;
			    }
			    else if (str[i]==DLGGRPSYMB)
			    {
				i+=3;
				sizex=0;
			    }
			    else
			    {
				getlettersizexy(fontnr,((int)(str[i]<<8))|(str[i+1]),&sizex,&sizey);
				i++;
			    }
			}
			else
			    getlettersizexy(fontnr,str[i],&sizex,&sizey);
		        if (i<j-1)//at last symbol we do not add spacesize
		    	    sizex += spaceletters[fontnr];
			getmaxsymbolsize(fontnr,NULL,&sizey);
			if (sizey>sizeylen)
			    sizeylen=sizey;
		    }
	    }
	sizexlen+=sizex;
    }
    if (sizexlen>maxsizexlen)
	maxsizexlen=sizexlen;
    maxsizeylen+=sizeylen;
    if (lenx)
	*lenx=maxsizexlen;
    if (leny)
	*leny=maxsizeylen;
    return(j);
}
//================================
int getmessagelen(int fontnr,char *str,int *lenx,int *leny)
{
    return(getmessagesizex(fontnr,str,strlen(str),lenx,leny));
}
//================================
void putboxmessage( int cborder,int cbox,
		    int x1,int y1,int bordersize,
		    int fontnr,char *message,int messagecolor,char *tfont,GRPFILE *dlggrp)
{
    int sizex,sizey;
    int x2,y2;
    struct XY uu;
    getmessagelen(fontnr,message,&sizex,&sizey);
    sizey+=bordersize;
    y1-=sizey;
    if (y1<0)
	y1=0;
    x2=x1+sizex+bordersize;
    y2=y1+sizey+bordersize;
    if (x2>=GRP_wmaxx)
    {
        x1-=x2-GRP_wmaxx;
        x2=GRP_wmaxx;
    }
    if (y2>=GRP_wmaxy)
    {
        y1-=y2-GRP_wmaxy;
        y2=GRP_wmaxy;
    }
    uu.x1=x1;
    uu.y1=y1;
    uu.x2=x2;
    uu.y2=y2;
    wsetimage8(cbox,&uu);
    wrectangle(cborder,x1,y1,x2,y2);
    sizex=putmessage(x1+bordersize,y1+bordersize/2,fontnr,message,messagecolor,tfont,dlggrp);
}
//================================
#define MAXOUTBUF	1000
char outbuf[MAXOUTBUF];
//================================
void putmessageinrectangleL(int x,int y,int sizexrect,int sizeyrect,int rowsize,char *mes,
    int fontnr,int color,char *fonttable,GRPFILE *dlggrp,int flags,int skiplinepixels)
{
//    int i,j,k,from,sizepartmes,sizexmes,showstr=0,posx=0,yoffset=0,align=0,skipfirst=skiplinepixels;
    int i,j,k,from,sizepartmes,sizexmes,showstr=0,posx=0;
    int yoffset=skiplinepixels,align=0,skipfirst=skiplinepixels,lastline=0;
    j = strlen(mes);
    if (j>=MAXOUTBUF-2)
    {
	printf("buffer message in to low \n");
	return;
    }
//    outbuf=(char *) wmalloc(j+2);
    from=0;
    sizexmes=0;
    for (i=0;i<=j;i++)
    {
	if (mes[i]==COMMANDSYMB)
	{
	    i+=COMMANDSYMB_SIZES[mes[i+1]];
	    continue;
	}
	if (mes[i]==RIGHTALIGNSYMB)
	{
	    showstr=1;
	    align=1;
	}
	else if (mes[i]==CENTERALIGNSYMB)
	{
	    showstr=1;
	    align=2;
	}
	else
	    if (mes[i]=='\n' || mes[i]==0 )
	    {
		showstr=1;
	    }
	if (mes[i]==' ' || mes[i]==0 || mes[i]=='\n')
	{
	    getmessagesizex(fontnr,mes+from,i-from,&sizexmes,NULL);
	    if (sizexmes>sizexrect)
	    {
		//we need to go back
		if (flags&MES_SPLITTONEXTLINE)
		{
		    i=visibletext(fontnr,sizexrect,mes+from)+from;
		    showstr=1;
		}
		else
		{
		    i--;
		    for (;i>=0;i--)
			if (mes[i]==' ')
			{
		    	    showstr=1;
			    break;
			}
		}
	    }
	}
	if (showstr)
	{
	    posx=0;
	    for (k=from;k<=i;k++)
	    {
		if (mes[k]<=MAXCOLORFONT)
	    	    if (!(flags&MESINRECT_COLORS))
			continue;
		outbuf[posx++]=mes[k];
	    }
	    outbuf[posx++]=0;
	    getmessagesizex(fontnr,outbuf,i-from,&sizexmes,NULL);
	    if (align==0)
	    {
		if (lastline)
		{
		    putmessage(x,y+yoffset,fontnr,outbuf,color,fonttable,dlggrp,skiplinepixels,rowsize-(sizeyrect-yoffset));
		    break;
		}
		if (-skiplinepixels<=rowsize)
		{
		    putmessage(x,y+yoffset,fontnr,outbuf,color,fonttable,dlggrp,skiplinepixels,0);
		}
	    }
	    else if (align==1)
	    {
		putmessageinrectangleR(x,y,sizexrect,sizeyrect,rowsize,mes+i+1,fontnr,color,fonttable,dlggrp,flags);
		break;
	    }
	    else if (align==2)
	    {
		putmessageinrectangleM(x,y,sizexrect,sizeyrect,rowsize,mes+i+1,fontnr,color,fonttable,dlggrp,flags);
		break;
	    }
	    from = i+1;
	    showstr=0;
	    yoffset+=rowsize;
	    if (yoffset+rowsize>sizeyrect)
		lastline=1;
//		break;
	    skiplinepixels+=rowsize;
	    if (skiplinepixels>0)
		skiplinepixels=0;
	}
    }
//    wfree(outbuf);
}
//================================
void putmessageinrectangleR(int x,int y,int sizexrect,int sizeyrect,int rowsize,char *mes,
    int fontnr,int color,char *fonttable,GRPFILE *dlggrp,int flags)
{
    int i,j,k,from,sizepartmes,sizexmes,showstr=0,posx=0,yoffset=0;
    j = strlen(mes);
    if (j>=MAXOUTBUF-2)
    {
	printf("buffer message in to low \n");
	return;
    }
//    outbuf=(char *) wmalloc(j+2);
    from=0;
    sizexmes=0;
    for (i=0;i<=j;i++)
    {
	if (mes[i]==COMMANDSYMB)
	{
	    i+=COMMANDSYMB_SIZES[mes[i+1]];
	    continue;
	}
	if (mes[i]=='\n' || mes[i]==0)
	{
	    showstr=1;
	}
	if (mes[i]==' ' || mes[i]==0 || mes[i]=='\n')
	{
	    getmessagesizex(fontnr,mes+from,i-from,&sizexmes,NULL);
	    if (sizexmes>sizexrect)
	    {
		//we need to go back
		if (flags&MES_SPLITTONEXTLINE)
		{
		    i=visibletext(fontnr,sizexrect,mes+from)+from;
		    showstr=1;
		}
		else
		{
		    i--;
		    for (;i>=0;i--)
			if (mes[i]==' ')
			{
		    	    showstr=1;
			    break;
			}
		}
	    }
	}
	if (showstr)
	{
	    posx=0;
	    for (k=from;k<=i;k++)
	    {
		if (mes[k]<=MAXCOLORFONT)
	    	    if (!(flags&MESINRECT_COLORS))
			continue;
		outbuf[posx++]=mes[k];
	    }
	    outbuf[posx++]=0;
	    getmessagesizex(fontnr,outbuf,i-from,&sizexmes,NULL);
	    putmessage(x+sizexrect-sizexmes-1,y+yoffset,fontnr,outbuf,color,fonttable,dlggrp);
	    from = i+1;
	    yoffset+=rowsize;
	    showstr=0;
	    if (yoffset+rowsize>sizeyrect)
		break;
	}
    }
//    wfree(outbuf);
}
//================================
void putmessageinrectangleM(int x,int y,int sizexrect,int sizeyrect,int rowsize,char *mes,
    int fontnr,int color,char *fonttable,GRPFILE *dlggrp,int flags)
{
    int i,j,k,from,sizepartmes,sizexmes,showstr=0,posx=0,yoffset=0;
    j = strlen(mes);
    if (j>=MAXOUTBUF-2)
    {
	printf("buffer message in to low \n");
	return;
    }
//    outbuf=(char *) wmalloc(j+2);
    from=0;
    sizexmes=0;
    for (i=0;i<=j;i++)
    {
	if (mes[i]==COMMANDSYMB)
	{
	    i+=COMMANDSYMB_SIZES[mes[i+1]];
	    continue;
	}
	if (mes[i]=='\n' || mes[i]==0)
	{
	    showstr=1;
	}
	if (mes[i]==' ' || mes[i]==0 || mes[i]=='\n')
	{
	    getmessagesizex(fontnr,mes+from,i-from,&sizexmes,NULL);
	    if (sizexmes>sizexrect)
	    {
		//we need to go back
		if (flags&MES_SPLITTONEXTLINE)
		{
		    i=visibletext(fontnr,sizexrect,mes+from)+from;
		    showstr=1;
		}
		else
		{
		    i--;
		    for (;i>=0;i--)
			if (mes[i]==' ')
			{
		    	    showstr=1;
		    	    break;
			}
		}
	    }
	}
	if (showstr)
	{
	    posx=0;
	    for (k=from;k<=i;k++)
	    {
		if (mes[k]<=MAXCOLORFONT)
	    	    if (!(flags&MESINRECT_COLORS))
			continue;
		outbuf[posx++]=mes[k];
	    }
	    outbuf[posx++]=0;
	    getmessagesizex(fontnr,outbuf,i-from,&sizexmes,NULL);
	    putmessage(x+(sizexrect-sizexmes)/2,y+yoffset,fontnr,outbuf,color,fonttable,dlggrp);
	    from = i+1;
	    yoffset+=rowsize;
	    showstr=0;
	    if (yoffset+rowsize>sizeyrect)
		break;
	}
    }
//    wfree(outbuf);
}
//================================
int GetFontNrFromYSize(int ysize)
{
    int y,i;
    for (i=IDFONT8;i<=IDFONT50;i++)
    {
	getmaxsymbolsize(i,NULL,&y);
	if (ysize<y)
	{
	    if (i==0)
		return 0;
	    return(i-1);
	}
    }
    return(IDFONT50);
}
//=====================================================
void putsmkbuttonmes(int x,int y,int rowsize,char *mes,int fontnr,int color,char *fonttable,int glowfactor)
{
    int i,j,from,sizexmes;
    int sizexrect=0;
    j = strlen(mes);
    if (j>=MAXOUTBUF-2)
    {
	printf("buffer message in to low \n");
	return;
    }
//    char *outbuf=(char *)wmalloc(j+1);
    memcpy(outbuf,mes,j+1);
    from=0;
    for (i=0;i<=j;i++)
    {
	if (outbuf[i]=='\n' || outbuf[i]==0)
	{
	    getmessagesizex(fontnr,outbuf+from,i-from,&sizexmes,NULL);
	    from=i+1;
	    if (sizexmes>sizexrect)
		sizexrect=sizexmes;
	}    
    }
    from=0;
    for (i=0;i<=j;i++)
    {
	if (outbuf[i]=='\n' || outbuf[i]==0)
	{
	    outbuf[i]=0;
	    getmessagesizex(fontnr,outbuf+from,i-from,&sizexmes,NULL);
	    putglowtext(x+(sizexrect-sizexmes)/2,y,fontnr,outbuf+from,color,fonttable,glowfactor);
	    from = i+1;
	    y+=rowsize;
	}
    }
//    wfree(outbuf);
}
//================================
void putsmkbuttonmesL(int x,int y,int rowsize,char *mes,int fontnr,int color,char *fonttable,int glowfactor)
{
    int i,j,from,sizexmes;
    j = strlen(mes);
    if (j>=MAXOUTBUF-2)
    {
	printf("buffer message in to low \n");
	return;
    }
//    char *outbuf=(char *)wmalloc(j+1);
    memcpy(outbuf,mes,j+1);
    from=0;
    for (i=0;i<=j;i++)
    {
	if (outbuf[i]=='\n' || outbuf[i]==0)
	{
	    outbuf[i]=0;
	    getmessagesizex(fontnr,outbuf+from,i-from,&sizexmes,NULL);
	    putglowtext(x,y,fontnr,outbuf+from,color,fonttable,glowfactor);
	    from = i+1;
	    y+=rowsize;
	}
    }
//    wfree(outbuf);
}
//================================
void putsmkbuttonmesM(int x,int y,int sizexrect,int rowsize,char *mes,int fontnr,int color,char *fonttable,int glowfactor)
{
    int i,j,from,sizexmes;
    j = strlen(mes);
    if (j>=MAXOUTBUF-2)
    {
	printf("buffer message in to low \n");
	return;
    }
//    char *outbuf=(char *)wmalloc(j+1);
    memcpy(outbuf,mes,j+1);
    from=0;
    for (i=0;i<=j;i++)
    {
	if (outbuf[i]=='\n' || outbuf[i]==0)
	{
	    outbuf[i]=0;
	    getmessagesizex(fontnr,outbuf+from,i-from,&sizexmes,NULL);
	    putglowtext(x+(sizexrect-sizexmes)/2,y,fontnr,outbuf+from,color,fonttable,glowfactor);
	    from = i+1;
	    y+=rowsize;
	}
    }
//    wfree(outbuf);
}
//================================
void putsmkbuttonmesR(int x,int y,int sizexrect,int rowsize,char *mes,int fontnr,int color,char *fonttable,int glowfactor)
{
    int i,j,from,sizexmes;
    j = strlen(mes);
    if (j>=MAXOUTBUF-2)
    {
	printf("buffer message in to low \n");
	return;
    }
//    char *outbuf=(char *)wmalloc(j+1);
    memcpy(outbuf,mes,j+1);
    from=0;
    for (i=0;i<=j;i++)
    {
	if (outbuf[i]=='\n' || outbuf[i]==0)
	{
	    outbuf[i]=0;
	    getmessagesizex(fontnr,outbuf+from,i-from,&sizexmes,NULL);
	    putglowtext(x+sizexrect-sizexmes,y,fontnr,outbuf+from,color,fonttable,glowfactor);
	    from = i+1;
	    y+=rowsize;
	}
    }
//    wfree(outbuf);
}
//================================
//calc nr of visibled symbols
int visibletext(int fontnr,int sizex,char *text)
{
    int sx;
    int len,i,currsize=0;
    len=strlen(text);
    for (i=0;i<len;i++)
    {
	getlettersizexy(fontnr,text[i],&sx,NULL);
	currsize+=sx;
        if (i<len-1)
    	    currsize += spaceletters[fontnr];
	if (currsize>sizex)	
	    break;
    }
    return(i);
}
//================================
int visiblerighttext(int fontnr,int sizex,char *text)
{
    int sx;
    int len,i,j,currsize=2;	//cursorsize
    len=strlen(text);
    for (j=0,i=len-1;i>=0;i--)
    {
	getlettersizexy(fontnr,text[i],&sx,NULL);
	currsize+=sx;
        currsize += spaceletters[fontnr];
	if (currsize>sizex)	
	    return(j-1);
	j++;
    }
    return(j);
}
//================================




