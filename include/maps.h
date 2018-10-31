#if !defined(_MAPS_W)
#define _MAPS_W

#include "vars.h"
#include "man.h"
#include "starmap.h"

#define CHECKFORMAPBORDERS(xkart,ykart) ( (ykart) < 0 || (xkart) < 0 || (ykart) >= MAXYMAP || (xkart) >= MAXXMAP )


void CreateMiniMapPixels(struct mapinfo *map);
void minimap_showobj(struct OBJ *a);
void ObjOnMiniMap(int x,int y,int sizex,int sizey,char color,char *minimapadr);
void getminimapcoord(struct mapinfo *info,int x,int y,int *xm,int *ym);
void drawMINIMAP(void);
void drawMAP(int ignorefirstwaiting);
void AutoMoveMap(void);
void MoveVisualMapPosition(int x,int y);
void MoveVisualMapPositionCenter(int x,int y);
bool SetVisualMapPosition(int x,int y);
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
int	 getplayerrace(struct mapinfo *map,int playernr);
int	 GetPlayerLocation(int PlayerNr);
void arbitermakewarpfield(OBJ *a,OBJstruct *b);
void saveoldtileandcreep(void);
void savemaptileadr(int x,int y,int indextile32);
void savemapcreepadr(int x,int y,int creepnr);
int	 CorrectRaceType(int ret,int playernr);


int	 mapSEE(OBJ *a,int player);
int	 mapSEE(int xkart,int ykart);
int	 mapSEE(int xkart,int ykart,int player);
int	 mapSEE2(int xkart,int ykart,int player);
int	 mapOPEN(OBJ *a,int player);
int	 mapOPEN(int xkart,int ykart);
int	 mapOPEN(int xkart,int ykart,int player);
int	 mapEFFECT(int xkart,int ykart,int effectmage);
int	 CreateAlianceBitsPlayer(int playernr);
int	 GetVisionBitsPlayer(int playernr);
void opentempmap(int playernr,int xkart,int ykart,int sizex,int sizey);
int	 GetMapFog(int x,int y);
void ClearFinalOBJ(OBJ *a0);

extern struct mapinfo 	map;

inline int GetVisionBitsPlayer(int playernr) { return (map.pl_visionbits[playernr]); };
inline int player_aliance(int whopl,int pl) { return (map.pl_allied[whopl][pl]); }; //0-enemy,1-neutral,2-alience,3-my
inline int player_vision(int whopl,int pl) { return (map.pl_vision[whopl][pl]); };
inline int mapEFFECT(int xkart,int ykart,int effectmage) { return(map.mapbits.mageeffect[effectmage][ykart*MAXXMAP+xkart]); };
//=============================================
class ScreenMapInfo
{
public:
	int		MinimapPosX;		//position of minimap on screen //Xkart && Ykart
	int		MinimapPosY;
	int		SizeX32;			//size of screen in 32x32 item //widthkart && hightkart
	int		SizeY32;

	int		ScrollX;			//factor to scroll
	int		ScrollY;

	int		MinimapStartX;		//pos of minimap in minimap area //xkart/begin/end & tkart/begin/end
	int		MinimapStartY;		//
	int		MinimapEndX;		//
	int		MinimapEndY;		//
};

extern float 			factorx,factory;
extern ScreenMapInfo	*screenMapInfo;

#endif /*_MAPS_W*/
