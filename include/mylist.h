#if !defined(_MYLIST_W)
#define _MYLIST_W

#include "debug.h"

#define MAXLISTELEMENTS 64
#define MYLIST_FULL	1
#define MYLIST_EMPTY	0
struct listelem
{
    int  emptyelem;
    int  firstemptyelem;
    listelem *next;
    listelem *prev;
    void *elements[MAXLISTELEMENTS];
    char bitelements[MAXLISTELEMENTS];
};

class  mylist
{
    int	maxelements;
    int	maxelemnr;
    listelem *first;
    int enumvalue;
    int globalenumvalue;
    listelem *curentenum;
private:
    void resetvars(void);
public:
    void FlushList(void);
    int  AddList(void *elem);
    int  DelList(void *elem);
    int  DelList(int elemnr);
    int  SwapElements(int elemnr1,int elemnr2);
    void *GetElemNr(int elemnr);
    void *GetElemNrNoNULL(int *elemnr);
    int  GetMaxElements(void);
    int  GetMaxElemNr(void);
    void AllocAndAddList(void *elem,int elemsize);
    void FreeAndDelList(int elemnr,void (*deallocfunc)(void *elem));
    void FreeAndDelList(void *adrelem,void (*deallocfunc)(void *elem));
    void DeallocList(void);
    void DeallocList(void (*deallocfunc)(void *elem));
    void SortA(void);
    void Shift(void);
    int  HaveElem(void *elem);
    int  AddUniqueElem(void *elem);
    void EnumListInit(void);
    void *GetNextListElem(void);
    void *GetNextListElem(int *posinlist);
    void *GetLastElem(void);
    int  GetLastElemNr(void);


    mylist();
    ~mylist();
};
//=========================================
#define MAXCRITFUNC		2
#define MAXELEMOFFSET		1000
class OBJlists
{
private:
    int  CritFunc;
    int  (*AllCriteriesFunc[MAXCRITFUNC])(int elem_id);
    int  *elems[MAXCRITFUNC];
    int	 curelements[MAXCRITFUNC];
    int	 maxelements[MAXCRITFUNC];
    int  seekpos[MAXCRITFUNC];
public:
    int  AddCriteries(int (*func)(int elem_id),int maxelem);
    void AddElem(int elem_id);
    void SetFirstSeek(int criteria_id);
    int  FindNextElem(int criteria_id);
    void DelElem(int elem_id);
    void ClearAll(void);
};

class  cycles
{
    int  totalelem;
    int  curelem;
    int  poselem;
    void **elements;
public:
    cycles(int maxelems);
    ~cycles();
    void DelElem(int elemnr);
    void *AddElem(int len);
    void *GetCurElem();
    void DelCurElem();
};
//=========================================
#include <stdio.h>
class mylistsimple
{
public:
    unsigned short allocatedelem;
    unsigned short totalelem;
    unsigned short totalmarked;
    unsigned short curenumelemnr;
    void **elements;
    char *deletemarked;
    mylistsimple(int neededelem);
    ~mylistsimple();
    inline void AddElem(void *elem) { if (totalelem<allocatedelem) elements[totalelem++] = elem; else DEBUGMESSCR("mylistsimple:max elems exceeded\n"); };
    inline void MarkForDelElem(int elemnr) { if (!deletemarked[elemnr]) { totalmarked++; deletemarked[elemnr] = 1; } };
    inline int GetMaxElements(void) { return (totalelem); };
    inline void EnumListInit(void) { curenumelemnr = 0; };
    void *GetNextListElem(int *elemnr);
    void *GetElem(int elemnr,int *retelemnr);
    void *GetABSNextListElem(char *deleteflag);
    void DeleteOneElem(int elemnr);
    void DeleteMarked(void);
    void FreeAndEmptyAll(void);
    inline int   GetFreeElements(void){ return(allocatedelem - totalelem); };
    void  DelElem(void *);
};

#endif  /*	_MYLIST_W  */


