
#if !defined(_OVERLAY_W)
#define _OVERLAY_W

#include <grplib/grp.h>
#include "images.h"

//functions for special draw additional to useputgrp
#define WARPTEXTURE		6
#define WARPFROMWHITE		7
#define DRAWBURROWED		8

#define SHADOWLINEINBLACKTABLE	10
//=====================================================
#define DRAWFUNC_NORMAL		0
#define DRAWFUNC_HIDDENTRAP	1
#define DRAWFUNC_ENEMYCLOAK	2
#define DRAWFUNC_OWNCLOAK1	3
#define DRAWFUNC_ALLYCLOAK	4
#define DRAWFUNC_OWNCLOAK2	5
#define DRAWFUNC_OWNCLOAK3	6
#define DRAWFUNC_UNKNOWN1	7
#define DRAWFUNC_EMPSHOCKWAVE	8
#define DRAWFUNC_REMAPPING	9
#define DRAWFUNC_SHADOW		10
#define DRAWFUNC_UNKNOWN2	11
#define DRAWFUNC_WARP1		12
#define DRAWFUNC_UNKNOWN3	13
#define DRAWFUNC_UNKNOWN4	14
#define DRAWFUNC_UNKNOWN5	15
#define DRAWFUNC_HALLUCINATION	16
#define DRAWFUNC_WARP2		17

//=====================================================
#define CORROSIVEACIDMAXSPORES	9
#define CORROSIVEACIDDIVFACTOR	(10<<8)
#define CORROSIVEACIDMAXATRVAL	(CORROSIVEACIDMAXSPORES*CORROSIVEACIDDIVFACTOR-1)

//=====================================================
void Images_FUNC(OVERLAY_IMG *img,int func,int remap,char *grptable,char *grpmethod);
void UnloadAllOverlaysGRP(void);
void LoadOverlaysGRP(void);
void FillUnitOverlays(struct OBJstruct *b,int SC_Unit);
int  DamageFactor(int number,int maxnumber);
void ShowShieldOverlay(struct OBJ *a,struct LOFILE *lo,int directiondamage);
void ShowRechargeShield(struct OBJ *a);
void SetBunkerShowFire(struct OBJ *marine,struct OBJ *a);

int  GetCorrosiveAcidValue(struct OBJ *a);
void ShowCorrosiveAcid(struct OBJ *a,struct OBJstruct *b);
//=====================================================

#endif
