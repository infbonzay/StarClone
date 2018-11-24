#if !defined(_REFRESHMAP_W)
#define _REFRESHMAP_W

#include "const.h"
#define MAPREGENERATION			60

class RefreshMap
{

	struct _BE_ diapazone[MAPREGENERATION];
	int		NrObjRegen;
	int		MinObjRegen;
	int		MaxRegen;

public:

	void OpenMapByRangeOfUnits(void);
	void Init(void);
	void UpdateSequenseObjs(void);
	void CreateObjRegen(void);

};

#endif
