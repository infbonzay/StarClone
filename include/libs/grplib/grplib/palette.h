#if !defined(_PALETTE_W)
#define _PALETTE_W

#define MAKERGB(r,g,b) ((r<<16)|(g<<8)|(b))
/*
    palette transform functions (8-bit color only)
*/
#ifdef __cplusplus
    extern "C" {
#endif

//==========================
void GRP_CreateTRColors(char *pal,char *outfortrbuffer);

void GRP_CreateSpecialTranspTable(char *pal,char *outfortrbuffer,float LIGHTLEVELON1);

void GRP_CreateByRGB(char *pal,int rgb,float addgrd,char *outbuf);

void GRP_CreateMono(char *pal,char *outbuf);

void GRP_CreateByRGBTable(char *pal,int rgb,int rgbe,int maxgradation,char *outbuf);

void GRP_ConvertColorsToNewPalette(unsigned char *pictpal,unsigned char *newpal,unsigned char *conversionarray,int palettesize);

void GRP_GetGlowColors( int maxgradation, int rgb,int rgbe,unsigned char *buf );

#ifdef __cplusplus
    }
#endif

#endif /*  _PALETTE_W */
