#ifndef _SC_REGENOBJMAP_H

#define _SC_REGENOBJMAP_H

#include "man.h"
#define FULLMAPREGENERATIONCYCLES		60

class RegenObjMap
{

	int		RegenCycleNr;
	int		NrRegenObjs;
	int		RegenFromNr;
	int		EnumFirst;

public:

	void Init(void);
	void OpenMapCycle(void);
	void OpenMapByRange(int from, int to);
	void ChangeObjPos(OBJ *o, int newposinlist);
	void AddObjList(OBJ *o);
	void DelObjList(OBJ *o);
	void ClearEnumerateObj(void);
	OBJ* GetNextObj(void);
	int ObjPosInList(void);
	void ObjPosInList(int newEnumPos);
};

#endif
