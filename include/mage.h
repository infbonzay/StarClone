#ifndef _SC_MAGE_H

#define _SC_MAGE_H

#include "man.h"

#define DELTAMANAONCYCLE		7		//add mana in 1 cycle
#define ADDMATRIXHP				(250<<8) //temprary defence matrix hp

#define DAMAGEIGNOREARMOR		1
#define DAMAGEINCLUDEARMOR		0

void mageeffect(int typemage,struct OBJ *a,struct OBJ *whoobjputmage);
void restoreatribute(struct OBJ *a);
void ApplyCastedMage(OBJ *a,OBJ *destobj,int typemage);
int	 accessatronunit(int aplayernr,struct OBJ *destobj,unsigned char weapon_id);
int	 accesstomage(struct OBJ *a,struct OBJ *destobj,int mode);
void addmage(struct OBJ *a,int nmage,int val1);
void magemakedamage(void);
int	 decrresourceobj(struct OBJ *a,int minus);
int	 existparasitebit(struct OBJ *a,int playernr);
int	 existatrdecloak(struct OBJ *a);
void MageAtributeEnded(struct OBJ *a,int i);
void mageattributedothings(OBJ *a);
int	 GetMageAtr(ATROBJ *a,int atrnr);
void SetMageAtr(ATROBJ *a,int atrnr,int value);
int	 DeltaMageAtr(ATROBJ *a,int atrnr,int value);
//void MakeSplashDamageToUnits(OBJ *a,int damagemin,int damagemax,int maxdamageradius,int fullradius);
void MakeMindControl(OBJ *a,int nplayer,int ncolor);
int	 ifcanworkatr_onothers(OBJ *a,int atr,int typemage);
int	 ifcanworkatr_onme(OBJ *a,int atr,int typemage);
void DecrementOBJAtr(struct OBJ *a);
void CastSpell(OBJ *a);
void CastSpellWithOutWeaponnr(OBJ *a,int castmagenr);
void calcsomespellinfo(void);


#endif /*_MAGE_W*/


