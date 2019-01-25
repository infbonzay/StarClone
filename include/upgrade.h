#ifndef _SC_UPGRADE_H

#define _SC_UPGRADE_H

#include "defs.h"
#include "starmap.h"

#define ADDUPGRADE 1
#define DELUPGRADE -1

void AddUpgradeTree(StarMapInfo *loadedmap,int nplayer,int upgrade_id);
void SetTempUpgradeTree(StarMapInfo *loadedmap,int nplayer,int upgrade_id,int incrvar);
int	 GetTempUpgradeTree(StarMapInfo *loadedmap,int nplayer,int upgrade_id);
int	 GetUpgradeTree(StarMapInfo *loadedmap,int nplayer,int upgrade_id);
int	 GetMaxUpgradeTree(StarMapInfo *loadedmap,int nplayer,int upgrade_id);

void AddTechTree(StarMapInfo *loadedmap,int nplayer,int tech_id);
void SetTempTechTree(StarMapInfo *loadedmap,int nplayer,int tech_id,int incrvar);
int	 GetTempTechTree(StarMapInfo *loadedmap,int nplayer,int tech_id);
int	 GetTechTree(StarMapInfo *loadedmap,int nplayer,int tech_id);
int	 GetMaxTechTree(StarMapInfo *loadedmap,int nplayer,int tech_id);
int	 GetUnitRestrictions(StarMapInfo *loadedmap,int nplayer,SCUNIT SC_Unit);



#define TECHDATA_DAT_CLOAKINGFIELDTECH			9
#define TECHDATA_DAT_PERSONELCLOAKINGTECH		10
#define TECHDATA_DAT_BURROWINGTECH				11
#define TECHDATA_DAT_BURROWINGTECH				11
#define TECHDATA_DAT_CARRIERCAPACITYTECH		43
#define TECHDATA_DAT_REAVERCAPACITYTECH			36
#define UPGRADES_DAT_OVERLORDTRANSP				24
#define UPGRADES_DAT_ADRENALGLANDS				28




#endif /*_UPGRADE_W*/

