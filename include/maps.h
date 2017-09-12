#if !defined(_MAPS_W)
#define _MAPS_W

#include "starmap.h"

void CreateMiniMapPixels(struct mapinfo *map);
void minimap_showobj(struct OBJ *a);
void ObjOnMiniMap(int x,int y,int sizex,int sizey,char color,char *minimapadr);
void getminimapcoord(struct mapinfo *info,int x,int y,int *xm,int *ym);
void drawMINIMAP(void);
void drawMAP(int ignorefirstwaiting);
void MoveVisualMapPosition(int deltax,int deltay);
int  SetVisualMapPosition(int x,int y);
void SetVisualMapPositionCenter(int x,int y);
void xchgkart(void);
void putlansh(int xglob,int yglob,int x,int y,int indextile32,char wfog,char bfog);
void clearopenseeKarta(void);
void AddObjsRevealMap(void);
void makeopenseeKarta(int beginobj,int endobj);
void makeoneobjseeopen(struct OBJ *b,struct OBJstruct *c);
void calcfullinvandsee(void);
void calcfullinvandseeobj(struct OBJ *v1);
void _putcells(void);
int getplayerrace(struct mapinfo *map,int playernr);
int GetPlayerLocation(int PlayerNr);
void arbitermakewarpfield(OBJ *a,OBJstruct *b);
void saveoldtileandcreep(void);
void savemaptileadr(int x,int y,int indextile32);
void savemapcreepadr(int x,int y,int creepnr);


int mapSEE(OBJ *a,int player);
int mapSEE(int xkart,int ykart);
int mapSEE(int xkart,int ykart,int player);
int mapSEE2(int xkart,int ykart,int player);
int mapOPEN(OBJ *a,int player);
int mapOPEN(int xkart,int ykart);
int mapOPEN(int xkart,int ykart,int player);
int mapEFFECT(int xkart,int ykart,int effectmage);
int CreateAlianceBitsPlayer(int playernr);
int GetVisionBitsPlayer(int playernr);
int player_aliance(int whopl,int pl);
int player_vision(int whopl,int pl);
void opentempmap(int playernr,int xkart,int ykart,int sizex,int sizey);
int GetMapFog(int x,int y);



extern struct mapinfo map;
extern float factorx,factory;
extern int Xkartbeg,Ykartbeg,Xkartend,Ykartend;


#endif /*_MAPS_W*/
