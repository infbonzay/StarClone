#if !defined(_VISION_W)
#define _VISION_W

#define ALLVIS			15
#define MAXVISX			30
#define MAXVISY			30
#define MIDX			((MAXVISX-1)/2)
#define MIDY			((MAXVISY-1)/2)
#define SEERANGEIFNOTREADY	7
#define MINSEERANGE		2

#define MAXANGLES		16
#define MAXOFFSETELEMENTS	(MAXVISX*MAXVISY)
//=======================================
#pragma pack(push,1)
struct MAPOFFSETELEMENT
{
    unsigned char	rangevision;		//1 - (ALLVIS-1)
    signed char		xoffset;		//-ALLVIS .. +ALLVIS
    signed char		yoffset;		//-ALLVIS .. +ALLVIS
};
struct MAPVISIONOFFSETS
{
    unsigned int offsetelemnr[MAXANGLES];
    MAPOFFSETELEMENT mapelement[MAXOFFSETELEMENTS];
};
#pragma pack(pop)
//=======================================

//extern unsigned char MAPvision[4][MAXVISY][MAXVISX];
extern MAPVISIONOFFSETS *mapvisiontables[4];

//=============================================
void SetVisionTables(void);
void LoadVisionBinTables(void);
void UnloadVisionBinTables(void);

//=============================================
#endif

