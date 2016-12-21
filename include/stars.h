#ifndef _STARS_W
#define _STARS_W

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
    int image;		//starBmap *image //32bits
};

struct spkFile
{
    short numLayers;
    short numStarCount[];
    starRef References[];
    starBmap Bitmaps[];
};

int loadStars(char *starfn);
void unloadStars(void);
void showStars(int deltax,int deltay);

#endif
