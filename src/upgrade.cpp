
#include "starmap.h"
#include "upgrade.h"

//===================================
//upgrade +1
void AddUpgradeTree(mapinfo *loadedmap,int playernr,int upgrade_id)
{
	loadedmap->upgr_restrictions.start_upgrade[playernr][upgrade_id]++;
}
//===================================
//used in time of construction to prevent the same upgrade from other place
void SetTempUpgradeTree(mapinfo *loadedmap,int playernr,int upgrade_id,int incrvar)
{
	loadedmap->upgr_restrictions.currentmake[playernr][upgrade_id]+=incrvar;
}
//===================================
int GetTempUpgradeTree(mapinfo *loadedmap,int playernr,int upgrade_id)
{
	return(loadedmap->upgr_restrictions.currentmake[playernr][upgrade_id]);
}
//===================================
//get current upgrade level
int GetUpgradeTree(mapinfo *loadedmap,int playernr,int upgrade_id)
{
	return(loadedmap->upgr_restrictions.start_upgrade[playernr][upgrade_id]);
}
//===================================
//get max posible upgrades levels
int GetMaxUpgradeTree(mapinfo *loadedmap,int playernr,int upgrade_id)
{
	return(loadedmap->upgr_restrictions.max_upgrade[playernr][upgrade_id]);
}
//===================================
//tech +1
void AddTechTree(mapinfo *loadedmap,int playernr,int tech_id)
{
	loadedmap->tech_restrictions.tech_researched[playernr][tech_id]++;
}
//===================================
//used in time of construction to prevent research the same tech from other place
void SetTempTechTree(mapinfo *loadedmap,int playernr,int tech_id,int incrvar)
{
	loadedmap->tech_restrictions.currentmake[playernr][tech_id]+=incrvar;
}
//===================================
int	 GetTempTechTree(mapinfo *loadedmap,int playernr,int tech_id)
{
	return(loadedmap->tech_restrictions.currentmake[playernr][tech_id]);
}
//===================================
//get current tech level
int GetTechTree(mapinfo *loadedmap,int playernr,int tech_id)
{
	if (CODEFORWITHOUTTECHTREE)
		return(1);
	//if unused is set, this technology researched by default.(eg.defencivematric,parasite,...)
	if (alldattbl.techdata_dat->Unused[tech_id])
		return(1);
	return(loadedmap->tech_restrictions.tech_researched[playernr][tech_id]);
}
//===================================
//get max posible tech levels
int GetMaxTechTree(mapinfo *loadedmap,int playernr,int tech_id)
{
	return(loadedmap->tech_restrictions.tech_available[playernr][tech_id]);
}
//===================================
//get unit restrictions
int GetUnitRestrictions(mapinfo *loadedmap,int playernr,SCUNIT SC_Unit)
{
	if (loadedmap->prod_restrictions.use_defaults[playernr][SC_Unit])
	{
		return(loadedmap->prod_restrictions.global_restrictions[SC_Unit]);
	}
	else
	{
		return(loadedmap->prod_restrictions.player_restrictions[playernr][SC_Unit]);
	}
}
//===================================
