/*
    grp-lib v. 0.4.17
    Created by Vasya Botnary & Alex Botezatu
    Copyright 2016

    palette transform functions (8-bit color only)
    need to be compiled with 'unsigned char' parameter

*/

#include <math.h>
#include <stdlib.h>
#include "grplib/palette.h"

#define LIGHTLEVELON    0.5                  /* Percent of color 1 */
#define LIGHTLEVELUNDER (1-LIGHTLEVELON)     /* Percent of color 2 */

#define REDC		0
#define GREENC		1
#define BLUEC		2

#define NRELEM(palette,elem,nr)	(unsigned char)palette[(elem)*3+nr]

#define REDELEM(palette,elem) 	NRELEM(palette,elem,REDC)
#define GREENELEM(palette,elem) NRELEM(palette,elem,GREENC)
#define BLUEELEM(palette,elem)	NRELEM(palette,elem,BLUEC)

#define PLUSMINUSGRADATIONRBG(color,color2,grd,nrelem) ((float)(color2 - NRELEM(color,nrelem)) * grd / (maxgradation - 1))

#define GETR(rgb) ((rgb >> 16) & 0xff)
#define GETG(rgb) ((rgb >> 8) & 0xff)
#define GETB(rgb) (rgb & 0xff)
//==========================
//create transparent colors table of (maxpalettecolor x maxpalettecolor) size
//on return array outfortrbuffer constain this table
//example
//mix 'X'-color with 'Y'-color result 'outfortrbuffer[Y*maxpalettecolor+X]'-color
//==========================
//this function need more time on slowest machine
//best choice to use this function one time and save to file for future load
//==========================
void CreateTRColors(char *pal,char *outfortrbuffer)
{
    float fr, fg, fb;
    float fr2, fg2, fb2;
    float ir, ig, ib,lowest,coldif;

    float absr, absg, absb;

    unsigned int col,col2,findcol,bestfit = 0;


 /* Lightlevelon + lightlevelunder must equal to 1 */

 /* Transparency is created by taking two colors, multiplying the
 RGB values by a percentage, and adding the RGB values together.  The
 new color will contain a little bit of each original color. */


 /* For each of the 256 colors, we can mix with any other color, therefore
 we need a 256x256 table. */

    int maxpalettecolor=256;
    for (col2 = 0; col2 < maxpalettecolor; col2++)
    {
	fr2 = (float)REDELEM(pal,col2) * LIGHTLEVELUNDER;
	fg2 = (float)GREENELEM(pal,col2) * LIGHTLEVELUNDER;
	fb2 = (float)BLUEELEM(pal,col2) * LIGHTLEVELUNDER;
	for (col = 0; col < maxpalettecolor; col++)
	{
	    fr= (float)REDELEM(pal,col) * LIGHTLEVELON;
	    fg= (float)GREENELEM(pal,col) * LIGHTLEVELON;
	    fb= (float)BLUEELEM(pal,col) * LIGHTLEVELON;

	    ir = (long)(fr + fr2);
	    ig = (long)(fg + fg2);
	    ib = (long)(fb + fb2);

	    lowest = 655350.0;
	    for  (findcol = 0; findcol < maxpalettecolor; findcol++)
	    {
    		absr = ((float)REDELEM(pal,findcol) - ir) * 30;//r-percentage RED in any color
    		absg = ((float)GREENELEM(pal,findcol) - ig) * 59;//g-percentage GREEN in any color
    		absb = ((float)BLUEELEM(pal,findcol) - ib) * 11;//b-percentage BLUE in any color

    		coldif = sqrt(absr*absr + absg*absg + absb*absb);
    		if  (coldif < lowest)//found best equality
    		{
    		    lowest = coldif;
    		    bestfit = findcol;
    		}
	    }
	    outfortrbuffer[col2 * maxpalettecolor + col] = bestfit;
	}
    }
}
//==========================
void CreateSpecialTranspTable(char *pal,char *outfortrbuffer,float LIGHTLEVELON1)
{
    float LIGHTLEVELUNDER1 = (1 - LIGHTLEVELON1);
    float fr, fg, fb;
    float fr2, fg2, fb2;
    float ir, ig, ib,lowest,coldif;

    float absr, absg, absb;

	unsigned int col,col2,findcol,bestfit = 0;


 /* Lightlevelon + lightlevelunder must equal to 1 */

 /* Transparency is created by taking two colors, multiplying the
 RGB values by a percentage, and adding the RGB values together.  The
 new color will contain a little bit of each original color. */


 /* For each of the 256 colors, we can mix with any other color, therefore
 we need a 256x256 table. */

    int maxpalettecolor=256;
    for (col2 = 0; col2 < maxpalettecolor; col2++)
    {
	fr2 = (float)REDELEM(pal,col2) * LIGHTLEVELUNDER1;
	fg2 = (float)GREENELEM(pal,col2) * LIGHTLEVELUNDER1;
	fb2 = (float)BLUEELEM(pal,col2) * LIGHTLEVELUNDER1;
	for (col = 0; col < maxpalettecolor; col++)
	{
	    fr= (float)REDELEM(pal,col) * LIGHTLEVELON1;
	    fg= (float)GREENELEM(pal,col) * LIGHTLEVELON1;
	    fb= (float)BLUEELEM(pal,col) * LIGHTLEVELON1;

	    ir = fr + fr2;
	    ig = fg + fg2;
	    ib = fb + fb2;

	    lowest = 655350.0;
	    for  (findcol = 0; findcol < maxpalettecolor; findcol++)
	    {
    		absr = ((float)REDELEM(pal,findcol) - ir) * 30;//r-percentage RED in any color
    		absg = ((float)GREENELEM(pal,findcol) - ig) * 59;//g-percentage GREEN in any color
    		absb = ((float)BLUEELEM(pal,findcol) - ib) * 11;//b-percentage BLUE in any color

    		coldif = sqrt(absr*absr + absg*absg + absb*absb);
    		if  (coldif < lowest)//found best equality
    		{
    		    lowest = coldif;
    		    bestfit = findcol;
    		}
	    }
	    outfortrbuffer[col2 * maxpalettecolor + col] = bestfit;
	}
    }
}
//==========================
//==========================
//create table of colors with rules r,g,b and multiplicator
//r,g,b - constain percentage (in total need to be 100%) to transform
//colors in this format ,buffer return is 'maxpalettecolor' bytes
//==========================
void CreateByRGB(char *pal,int rgb,float addgrd,char *outbuf)
{
    float r1,g1,b1,r2,g2,b2;
    unsigned int col,findcol,bestfit=0;
    float absr, absg, absb,lowest,coldif;

    int maxpalettecolor = 256;
    for (col = 0; col < maxpalettecolor; col++)
    {

	r1 = (float)REDELEM(pal,col) * GETR(rgb) * addgrd/100;
	if (r1>255)
    	    r1 = 255;
	g1 = (float)GREENELEM(pal,col) * GETG(rgb) * (int)addgrd/100;
	if (g1>255)
    	    g1 = 255;
	b1 = (float)BLUEELEM(pal,col) * GETB(rgb) * (int)addgrd/100;
	if (b1>255)
    	    b1 = 255;

	lowest = 655350;
	for  (findcol = 0; findcol < maxpalettecolor; findcol++)
	{
    	    r2 = (float)REDELEM(pal,findcol);
    	    g2 = (float)GREENELEM(pal,findcol);
    	    b2 = (float)BLUEELEM(pal,findcol);

    	    absr = (r2 - r1) * GETR(rgb);
    	    absg = (g2 - g1) * GETG(rgb);
    	    absb = (b2 - b1) * GETB(rgb);

    	    coldif = sqrt(absr*absr + absg*absg + absb*absb);
    	    if  (coldif < lowest)
    	    {
    		lowest = coldif;
    		bestfit = findcol;
    	    }
	}
	outbuf[col] = bestfit;
    }
}
//==========================
//create table of colors gray gradation
//	,buffer return is 'maxpalettecolor' bytes
//==========================
void CreateMono(char *pal,char *outbuf)
{
    float r1,g1,b1,r2,g2,b2;
    float c1;
    unsigned int col,findcol,bestfit=0;
    float absr, absg, absb,lowest,coldif;

    int maxpalettecolor = 256;
    for (col = 0; col < maxpalettecolor; col++)
    {
	r1 = (float)REDELEM(pal,col) * 30;
	g1 = (float)GREENELEM(pal,col) * 59;
	b1 = (float)BLUEELEM(pal,col) * 11;
	c1 = (r1+g1+b1)/100;
	r1 = (int)c1;	//r,g,b equal in gray gradation
	g1 = (int)c1;	//
	b1 = (int)c1;	//

	lowest = 655350.0;
	for  (findcol = 0; findcol < maxpalettecolor; findcol++)
	{
    	    r2 = (float)REDELEM(pal,findcol);
    	    g2 = (float)GREENELEM(pal,findcol);
    	    b2 = (float)BLUEELEM(pal,findcol);

    	    absr = (r2-r1)*30;
    	    absg = (g2-g1)*59;
    	    absb = (b2-b1)*11;

    	    coldif = sqrt(absr*absr + absg*absg + absb*absb);
    	    if  (coldif < lowest)
    	    {
    		lowest = coldif;
    		bestfit = findcol;
    	    }
	}
	outbuf[col] = bestfit;
    }
}
//==========================
void getglowcolors( int maxgradation, int rgb,int rgbe,unsigned char *buf )
{
	int i;
	float fr, fg, fb;
	unsigned char r,g,b,re,ge,be;
	r = GETR(rgb);
	g = GETG(rgb);
	b = GETB(rgb);

	re = GETR(rgbe);
	ge = GETG(rgbe);
	be = GETB(rgbe);

	fr = r; fg = g; fb = b;
	float mgr3 = maxgradation/3;
	float mgr31 = mgr3*1.67;
	for( i=0; i<mgr3; i++ )
	{
		buf[i*3+0] = (unsigned char)(fr*(i/mgr31 + 0.4));
		buf[i*3+1] = (unsigned char)(fg*(i/mgr31 + 0.4));
		buf[i*3+2] = (unsigned char)(fb*(i/mgr31 + 0.4));
	}
	float mgr23 = mgr3*2;
	float tr = re-fr;
	float tg = ge-fg;
	float tb = be-fb;
	for( ; i<maxgradation-1; i++ )
	{
		float togo = (i-mgr3)/mgr23;
		buf[i*3+0] = (unsigned char)(fr+tr*togo);
		buf[i*3+1] = (unsigned char)(fg+tg*togo);
		buf[i*3+2] = (unsigned char)(fb+tb*togo);
	}
	if( re && ge && be )
	{
		buf[i*3+0] = 255;
		buf[i*3+1] = 255;
		buf[i*3+2] = 255;
	}
	else
	{
		buf[i*3+0] = 0;
		buf[i*3+1] = 0;
		buf[i*3+2] = 0;
	}
}
//==========================
void CreateByRGBTable(char *pal,
		 int rgb,	// start glow color
		 int rgbe,	// end glow color
		 int maxgradation,
		 char *outbuf)
{
    float r1,g1,b1,r2,g2,b2;
    float coldif,lowest;
    unsigned int col,findcol,grad,bestfit = 255;
    float absr, absg, absb;
    unsigned char *glowcolors;

    int maxpalettecolor=256;
    glowcolors = (unsigned char*)malloc( maxgradation*3 );
    getglowcolors( maxgradation, rgb,rgbe,glowcolors );
    for (col = 0; col < maxpalettecolor; col++)
    {
    	for (grad = 0; grad < maxgradation; grad++)
    	{
    	    r1 = (float)REDELEM(pal,col) * (maxgradation-grad-1);
    	    g1 = (float)GREENELEM(pal,col) * (maxgradation-grad-1);
    	    b1 = (float)BLUEELEM(pal,col) * (maxgradation-grad-1);
			r1 += glowcolors[grad*3+0]*(grad+1);
			g1 += glowcolors[grad*3+1]*(grad+1);
			b1 += glowcolors[grad*3+2]*(grad+1);
			r1 /= maxgradation;
			g1 /= maxgradation;
			b1 /= maxgradation;

			lowest = 655350.0;
			for  (findcol = 0; findcol < maxpalettecolor; findcol++)
			{
				r2 = (float)REDELEM(pal,findcol);
				g2 = (float)GREENELEM(pal,findcol);
				b2 = (float)BLUEELEM(pal,findcol);

				absr = (r2-r1)*33;
				absg = (g2-g1)*33;
				absb = (b2-b1)*33;

				coldif = sqrt(absr*absr + absg*absg + absb*absb);
				if  (coldif < lowest)
				{
					lowest = coldif;
					bestfit = findcol;
				}
			}
			outbuf[grad * maxpalettecolor + col] = bestfit;
		}
    }
    free(glowcolors);
}
//==========================
//we have 2 pictures with 2 palette
//to show picture 2 in palette 1 we need to transform colors in picture 2   to use similar from palette 1
//so pictpal the 1 palette, newpal - second palette , conversionarray    is   conversionarray[color_from_palette2] equal with color from palette1
//==========================
void ConvertColorsToNewPalette(unsigned char *pictpal,unsigned char *newpal,unsigned char *conversionarray,int palettesize)//palettesize 3 or 4 bytes per pixel
{
    float r1,g1,b1,absr,absg,absb,colorval,lowest;
    unsigned int col,col2,bestfit;

    for (col = 0; col < 256; col++)
    {
    	r1 = (float)pictpal[col*palettesize+0];
    	g1 = (float)pictpal[col*palettesize+1];
    	b1 = (float)pictpal[col*palettesize+2];
		lowest = 655350.0;
		bestfit = 0;
    	for (col2 = 0; col2 < 256; col2++)
    	{
    	    absr = ((float)newpal[col2*palettesize+0]-r1)*30;
			absg = ((float)newpal[col2*palettesize+1]-g1)*59;
    	    absb = ((float)newpal[col2*palettesize+2]-b1)*11;
    	    colorval = sqrt(absr*absr+absg*absg+absb*absb);
    	    if  (colorval < lowest)
			{
				lowest = colorval;
				bestfit = col2;
			}
			conversionarray[col] = bestfit;
		}
    }
}
//==========================

