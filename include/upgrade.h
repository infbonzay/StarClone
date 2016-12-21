#if !defined(_UPGRADE_W)
#define _UPGRADE_W

#include "man.h"
#include "maps.h"

#define ADDUPGRADE 1
#define DELUPGRADE -1

void AddUpgradeTree(mapinfo *loadedmap,int nplayer,int upgrade_id);
void SetTempUpgradeTree(mapinfo *loadedmap,int nplayer,int upgrade_id,int incrvar);
int  GetTempUpgradeTree(mapinfo *loadedmap,int nplayer,int upgrade_id);
int  GetUpgradeTree(mapinfo *loadedmap,int nplayer,int upgrade_id);
int  GetMaxUpgradeTree(mapinfo *loadedmap,int nplayer,int upgrade_id);

void AddTechTree(mapinfo *loadedmap,int nplayer,int tech_id);
void SetTempTechTree(mapinfo *loadedmap,int nplayer,int tech_id,int incrvar);
int  GetTempTechTree(mapinfo *loadedmap,int nplayer,int tech_id);
int  GetTechTree(mapinfo *loadedmap,int nplayer,int tech_id);
int  GetMaxTechTree(mapinfo *loadedmap,int nplayer,int tech_id);
int  GetUnitRestrictions(mapinfo *loadedmap,int nplayer,int SC_Unit);



#define TECHDATA_DAT_CLOAKINGFIELDTECH		9
#define TECHDATA_DAT_PERSONELCLOAKINGTECH	10
#define TECHDATA_DAT_BURROWINGTECH		11
#define TECHDATA_DAT_BURROWINGTECH		11
#define TECHDATA_DAT_CARRIERCAPACITYTECH	43
#define TECHDATA_DAT_REAVERCAPACITYTECH		36
#define UPGRADES_DAT_OVERLORDTRANSP		24
#define UPGRADES_DAT_ADRENALGLANDS		28




#endif /*_UPGRADE_W*/

