#if !defined(_REGENOBJMAP_W)
#define _REGENOBJMAP_W

#include "man.h"
#define FULLMAPREGENERATIONCYCLES		60

class RegenObjMap
{

	int		RegenCycleNr;
	int		NrRegenObjs;
	int		RegenFromNr;

public:

	void Init(void);
	void OpenMapCycle(void);
	void OpenMapByRange(int from, int to);
	void ChangeObjPos(OBJ *o, int newposinlist);
	void AddObjList(OBJ *o);
	void DelObjList(OBJ *o);
};

#endif
