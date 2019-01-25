#ifndef _SC_MYLIST_H

#define _SC_MYLIST_H

#include "debug.h"

#define MAXLISTELEMENTS 64
#define MYLIST_FULL		1
#define MYLIST_EMPTY	0
struct listelem
{
	int	 emptyelem;
	int	 firstemptyelem;
	listelem *next;
	listelem *prev;
	void *elements[MAXLISTELEMENTS];
	char bitelements[MAXLISTELEMENTS];
};

class  mylist
{
	int maxelements;
	int maxelemnr;
	listelem *first;
	int enumvalue;
	int globalenumvalue;
	listelem *curentenum;
private:
	void resetvars(void);
public:
	void FlushList(void);
	int	 AddList(void *elem);
	int	 DelList(void *elem);
	int	 DelList(int elemnr);
	int	 SwapElements(int elemnr1,int elemnr2);
	void *GetElemNr(int elemnr);
	void *GetElemNrNoNULL(int *elemnr);
	int	 Count(void);
	int	 GetMaxElemNr(void);
	void AllocAndAddList(void *elem,int elemsize);
	void FreeAndDelList(int elemnr,void (*deallocfunc)(void *elem));
	void FreeAndDelList(void *adrelem,void (*deallocfunc)(void *elem));
	void DeallocList(void);
	void DeallocList(void (*deallocfunc)(void *elem));
	void SortA(void);
	void Shift(void);
	int	 HaveElem(void *elem);
	int	 AddUniqueElem(void *elem);
	void EnumListInit(void);
	void *GetNextListElem(void);
	void *GetNextListElem(int *posinlist);
	void *GetLastElem(void);
	int	 GetLastElemNr(void);


	mylist();
	~mylist();
};
//=========================================

#endif	/*		_MYLIST_W  */


