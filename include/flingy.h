#if !defined(_FLINGY_W)
#define _FLINGY_W

#include "man.h"
#include "List.h"

#define FLINGYMOVECONTROL_FLINGY		0
#define FLINGYMOVECONTROL_WEAPON		1
#define FLINGYMOVECONTROL_ISCRIPT		2

#define FLINGY_FLAG_MOVETOOBJ							0x01
#define FLINGY_FLAG_STAYANDWAITTIME						0x02
#define FLINGY_FLAG_ACCELERATIONBIT						0x04
#define FLINGY_FLAG_BOUNCEATACK							0x08
#define FLINGY_FLAG_AFTERLAUNCHCHANGEELEVATION			0x10
#define FLINGY_IMG_NULL									0x20

//===================================================
class SC_FLINGY
{
public:
	OBJ					*atacker;
	OBJ					*destobj;
	OBJ					*destobj2;
	mylist				*listdamaged;
	int					xdest256;
	int					ydest256;
	int					currentspeed;
	unsigned char		nroftargets;
	unsigned char		timer;
	unsigned char		flingy_id;
	unsigned char		weapon_id;
	unsigned char		playernr;
	short int			flingylist_elemnr;
	short int			frommodemove;
	short int			dmgpertick;
	MAIN_IMG			*img;
	unsigned char		flags;
	SCUNIT				SC_Unit;
	unsigned char		movedticks;
	int					sizedestobj;

	SC_FLINGY(OBJ *a,OBJ *dest,int xdest,int ydest,unsigned char flingy_id);

	int	 DieFlingy(void);
	void DelFlingy(void);
	void MoveFlingy(void);
	void StayAndWaitTime(void);

};

//===================================================
class FLINGYLIST:public ListSimple<SC_FLINGY *>
{
public:
	FLINGYLIST(int nrofelems) : ListSimple(nrofelems) {};
	inline void Add(SC_FLINGY *elem) { elements[_Count] = elem; elem->flingylist_elemnr = _Count++; };
	void DeleteMarked(void);
	void Free(void);

	void RemoveOBJFromFlingyobj(OBJ *obj,OBJ *newatacker);
	void MoveAllWeaponFlingy(void);

};
//===================================================
void CalcXYOffsets(int side,int deltax,int deltay,signed char *xoffs,signed char *yoffs);

SC_FLINGY *CreateFlingyID(OBJ *a,OBJ *dest,int xstart256,int ystart256,int xdest256,int ydest256,
						  unsigned char weapon_id,unsigned char flingy_id,int deltaelevation);
SC_FLINGY *Create2FlingyID(OBJ *a,OBJ *destobj,int xstart256,int ystart256,int xdest256,int ydest256,int xdest256_2,int ydest256_2,
						  unsigned char weapon_id,unsigned char flingy_id,int deltaelevation);

SC_FLINGY *CreateWeaponFlingy(OBJ *a,OBJ *dest,int xstart256,int ystart256,int xdest256,int ydest256,
						  unsigned char weapon_id,unsigned char flingy_id,signed char launchspin,int deltaelevation);

extern FLINGYLIST weaponflingy;


#endif

