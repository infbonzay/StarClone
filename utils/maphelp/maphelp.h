#if !defined(_VISION_W)
#define _VISION_W

#define ALLVIS		15
#define MAXVISX		29
#define MAXVISY		29
#define MIDX		((MAXVISX-1)/2)
#define MIDY		((MAXVISY-1)/2)
#define PI 			3.141592658

extern unsigned char MAPvision[4][MAXVISY][MAXVISX];
//=============================================
void SetVisionTables(void);
void CreateMapOffsets(void);
//=============================================
#endif

