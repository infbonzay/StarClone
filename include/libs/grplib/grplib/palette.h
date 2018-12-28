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
void CreateTRColors(char *pal,char *outfortrbuffer);

void CreateSpecialTranspTable(char *pal,char *outfortrbuffer,float LIGHTLEVELON1);

void CreateByRGB(char *pal,int rgb,float addgrd,char *outbuf);

void CreateMono(char *pal,char *outbuf);

void CreateByRGBTable(char *pal,int rgb,int rgbe,int maxgradation,char *outbuf);

void ConvertColorsToNewPalette(unsigned char *pictpal,unsigned char *newpal,unsigned char *conversionarray,int palettesize);

#ifdef __cplusplus
    }
#endif

#endif /*  _PALETTE_W */
