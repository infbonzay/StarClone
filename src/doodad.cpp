
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "audio.h"

#include "man.h"
#include "vars.h"
#include "maps.h"
#include "lists.h"
#include "load.h"
#include "gener.h"
#include "gr.h"
#include "starmap.h"
#include "mpq.h"
#include "objinfo.h"
#include "objs.h"
#include "tempobj.h"
#include "unitaction.h"
#include "fog.h"
#include "images.h"
#include "iscript.h"
#include "sigorder.h"
#include "doodad.h"

//#define TESTDOODAD
//=====================================
void UnloadDoodadList(mylist *mapdoodads)
{
	mapdoodads->DeallocList();	//free every elem adr & destroy lists
}
//=================================================
#define DOODAD_ELEVATION_LEVEL	4
void CreateDoodadsFromMAP(doodad_on_map *doodad,mapinfo *loadedmap)
{
	MAIN_IMG *newimg;
	unsigned short image_id,flags;
	
	image_id  = alldattbl.sprites_dat->images_id[doodad->sprites_id];
	flags = 0;
	flags |= SC_IMAGE_FLAG_NOCHECKFORFOG;
	newimg = new MAIN_IMG(image_id,doodad->xpos<<8,doodad->ypos<<8,DOODAD_ELEVATION_LEVEL,
							0,0,0,doodad->playernr,flags,ISCRIPTNR_INIT);
	newimg->whocreate = SC_IMAGE_DOODAD_CREATOR;
	imageslist_doodads.AddElem(newimg);
	iscriptinfo.ExecuteScript(newimg);
}
//=====================================
void CreateDoodadsFromLists(mylist *mapdoodads,mapinfo *loadedmap)
{
	int iscript_flg;
	int maxelem;
	int i,doodadtype;
	OBJ *a;
	doodad_on_map *doodad;
	DEBUGMESSCR("total doodads = %d \n",mapdoodads->GetMaxElements());
	mapdoodads->EnumListInit();
	while( (doodad = (doodad_on_map *)mapdoodads->GetNextListElem()) )
	{
//			printf("%i.nrd=%d x=%d y=%d spr=%d flagunit=0x%x playernr=%d\n",i,
//				doodad->sprites_id,doodad->xpos,doodad->ypos,doodad->sprpos,doodad->flags,doodad->playernr);
			if (doodad->flags & DOODADISUNIT)
			{
//				doodad->playernr=NUMBGAMER;
				a = CreateUnitsFromMAP(doodad->xpos,doodad->ypos,doodad->sprites_id,doodad->playernr,loadedmap);
				if (!a)
					continue;
				if (doodad->flags & DOODADBOTTOMSTATE)
				{
					SetOrder(a,1,&SIGOrder_DoodadAfterBottom);
					SetDoodadState(a,DOODAD_TOP_STATE);
					SetOBJIScriptNr(a,ISCRIPTNR_ALMOSTBUILT,ISCRIPTNR_EXECUTE); //needed for doors
					DoodadChangeState(a,DOODAD_BOTTOM_STATE);
				}
				else
				{
					SetOrder(a,1,&SIGOrder_DoodadAfterTop);
					SetDoodadState(a,DOODAD_BOTTOM_STATE);
					DoodadChangeState(a,DOODAD_TOP_STATE);
				}
			}
			else
			{
				CreateDoodadsFromMAP(doodad,loadedmap);
			}
	}
	UnloadDoodadList(mapdoodads);
}
//=====================================
int	 GetDoodadState(OBJ *a)
{
	return(a->data.doodad.state1);
}
//=====================================================
void SetDoodadState(OBJ *a,int state)
{
	a->data.doodad.state1 = state;
}
//=====================================================
int	 GetDoodadMoveDirection(OBJ *a)
{
	return(a->data.doodad.movedirection);
}
//=====================================================
void SetDoodadMoveDirection(OBJ *a,int movedirection)
{
	a->data.doodad.movedirection = movedirection;
}
//=====================================================
void DoodadChangeState(OBJ *a,int state)
{
	int sound_id,scriptseterrorstatus;
//	  printf("change doodad state = %d\n",state);
	if (state == DOODAD_BOTTOM_STATE)
	{
		switch(GetDoodadState(a))
		{
			case DOODAD_TOP_STATE:
				//go bottom
				SetOrder(a,1,&SIGOrder_DoodadAfterBottom);
				scriptseterrorstatus = SetOBJIScriptNr(a,ISCRIPTNR_DOODADGOBOTTOM,ISCRIPTNR_EXECUTE);
				if (!scriptseterrorstatus)
				{
					sound_id = GetUnitSound(a,UNITSOUND_WHATFIRST,UNITSOUND_WHATFIRST);
					Play_sfxdata(GetOBJx(a),GetOBJy(a),sound_id,2);
				}
				SetDoodadMoveDirection(a,DOODAD_MOVE_TOBOTTOM);
			break;
		}
	}
	else
	{
		//after first go top trap is set to automatic top/bottom at enemy visibility
		if (!IsInvincibleUnit(a->SC_Unit))
			a->prop |= VARAUTOMATICDOODAD;
		switch(GetDoodadState(a))
		{
			case DOODAD_BOTTOM_STATE:
				SetOrder(a,1,&SIGOrder_DoodadAfterTop);
				if (CheckIscriptNr(a->mainimage,ISCRIPTNR_SPECIALSTATE1))
				{
					scriptseterrorstatus = SetOBJIScriptNr(a,ISCRIPTNR_SPECIALSTATE1,ISCRIPTNR_EXECUTE);
				}
				else
					scriptseterrorstatus = SetOBJIScriptNr(a,ISCRIPTNR_DOODADGOTOP,ISCRIPTNR_EXECUTE);
				if (!scriptseterrorstatus)
				{
					sound_id = GetUnitSound(a,UNITSOUND_WHATLAST,UNITSOUND_WHATLAST);
					Play_sfxdata(GetOBJx(a),GetOBJy(a),sound_id,2);
				}
				SetDoodadMoveDirection(a,DOODAD_MOVE_TOTOP);
			break;
		}
	}
}
//=================================
