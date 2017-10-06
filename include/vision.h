#if !defined(_VISION_W)
#define _VISION_W

#define ALLVIS		15
#define MAXVISX		29
#define MAXVISY		29
#define MIDX		((MAXVISX-1)/2)
#define MIDY		((MAXVISY-1)/2)
#define MAXSTORONSVETA		8
#define MAXSEERANGE		14
#define MINSEERANGE		2
#define SEERANGEIFNOTREADY	7

#define MAXANGLES		16
#define MAXOFFSETELEMENTS	(MAXVISX*MAXVISY)
//=======================================
struct MAPOFFSETELEMENT
{
    unsigned char	rangevision;		//1 - (ALLVIS-1)
    signed char		xoffset;		//-MAXXVIS .. +MAXXVIS
    signed char		yoffset;		//-MAXYVIS .. +MAXYVIS
};
struct MAPVISIONOFFSETS
{
    unsigned int offsetelemnr[MAXANGLES];
    MAPOFFSETELEMENT mapelement[MAXOFFSETELEMENTS];
};
//=======================================

extern unsigned char MAPvision[4][MAXVISY][MAXVISX];
extern MAPVISIONOFFSETS *mapvisiontables;

//=============================================
void SetVisionTables(void);
void LoadVisionBinTables(void);
void UnloadVisionBinTables(void);

//=============================================
#endif

