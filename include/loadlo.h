#if !defined(_LOADLO_W)
#define _LOADLO_W

//#include "man.h"
//===============================================
struct LOFILE
{
	int CountSides;
	int CountPictsPerSides;
	int offseteachline[];
};
//===============================================
//LOFILE *loadlo(char *filename);
//void unloadlo(LOFILE *temp);
signed char *GetLoXY(LOFILE *temp,int nr_side,int nr_pos);
//void SetLoXY(LOFILE *temp,int nr_side,int count,int deltax,int deltay);
//LOFILE *createlo(int count,int deltax,int deltay);
//void GetXYFromLO(struct OBJstruct *b,int mode,int line,int *x,int *y);
//===============================================
//DESCRIPTION OF .LO* FILES
//lof - 'F'ire or blood if build/unit have damaged
//lod - if build/unit on ground and go from 'D'own to up
//lou - if build/unit on ground and go from 'U'p to down
//los - vespene gas exits from tube and goes to 'S'ky
//lol - special (mage cast like yamatogun,siegemode)
//loa -	 from where goes 'A'ir atack bullet
//loo - 'Own' minerals (for drone,probe,scv)
//lob - where 'B'irth from cocoon
//log - from where goes 'G'ound atack bullet
//lox - ????
//===============================================

#endif /*_LOADLO_W*/

