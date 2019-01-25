#ifndef _SC_STARS_H

#define _SC_STARS_H

struct starBmap
{
	short Width;
	short Height;
	char Data[];
};

struct starRef
{
	short X;
	short Y;
	int image;			//starBmap *image //32bits
};

struct spkFile
{
	short numLayers;
	short numStarCount[];
//	  starRef References[];
//	  starBmap Bitmaps[];
};

int loadStars(const char *starfn);
void unloadStars(void);
void showStars(int deltax,int deltay);

#endif
