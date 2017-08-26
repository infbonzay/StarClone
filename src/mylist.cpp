
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "wmem.h"
#include "mylist.h"

//=========================================
void mylist::resetvars(void)
{
    maxelements = 0;
    maxelemnr = 0;
    first = NULL;
    EnumListInit();
}
//=========================================
mylist::mylist()
{
    resetvars();
}
//=========================================
//=========================================
mylist::~mylist()
{
//    FlushList();
    listelem *curent,*nextprocess;
    curent = first;
    if (curent)
	while(curent)
	{
	    if (!curent->next)
	    {
		nextprocess = curent->prev;
		wfree(curent);
		curent = nextprocess;
		if (curent)
		    curent->next=NULL;
	    }
	    else
		curent=curent->next;
	}
    resetvars();
}
//=========================================
void mylist::AllocAndAddList(void *elem,int elemsize)
{
    //prevent allocation 0 bytes
    if (!elemsize)
	elemsize=1;
    void *adrnewelem = wmalloc(elemsize);
    strncpy((char *)adrnewelem,(char *)elem,elemsize);
    AddList(adrnewelem);
}
//=========================================
void mylist::FreeAndDelList(int elemnr,void (*deallocfunc)(void *elem))
{
    void *adrelem=GetElemNr(elemnr);
    if (adrelem)
    {
	if (deallocfunc)
	    (*deallocfunc)(adrelem);
	wfree(adrelem);
	DelList(elemnr);
    }
}
//=========================================
void mylist::FreeAndDelList(void *adrelem,void (*deallocfunc)(void *elem))
{
    if (adrelem)
    {
	if (deallocfunc)
	    (*deallocfunc)(adrelem);
	wfree(adrelem);
	DelList(adrelem);
    }
}
//=========================================
void mylist::DeallocList(void)
{
    DeallocList(NULL);
}
//=========================================
void mylist::DeallocList(void (*deallocfunc)(void *elem))
{
    for (int i=maxelemnr-1;i>=0;i--)
    {
	FreeAndDelList(i,deallocfunc);
    }
}
//=========================================
//=========================================
void mylist::FlushList(void)
{
    listelem *curent=first;
    listelem *next;
    while(curent)
    {
	next=curent->next;
	wfree(curent);
	curent=next;
    }
    resetvars();
}
//=========================================
int mylist::AddList(void *elem)
{
    int i,nrreturn,next_parsetimes=0;
    listelem *curent,*next;
    if (!first)
    {
	first = (listelem *) wmalloc(sizeof(listelem));
	memset(first,0,sizeof(listelem));
	first->emptyelem = MAXLISTELEMENTS;
	maxelemnr += MAXLISTELEMENTS;
	first->firstemptyelem = 0;
    }
    curent=first;
    while(!curent->emptyelem)
    {
	next_parsetimes++;
	if (!curent->next)
	{
	    curent->next = (listelem *) wmalloc(sizeof(listelem));
	    memset(curent->next,0,sizeof(listelem));
	    next = curent->next;
	    next->prev = curent;
	    next->emptyelem = MAXLISTELEMENTS;
	    next->firstemptyelem = 0;
	    next->prev = curent;
	    next->next = NULL;
	    curent = next;
	    maxelemnr += MAXLISTELEMENTS;
	}
	else
	    curent = curent->next;
    }
    nrreturn = curent->firstemptyelem;
    curent->elements[nrreturn] = elem;
    curent->bitelements[nrreturn] = MYLIST_FULL;
    maxelements++;
    curent->emptyelem--;
    if (curent->emptyelem)
    {
	for (i=0;i<MAXLISTELEMENTS;i++)
	    if (curent->bitelements[i]==MYLIST_EMPTY)
	    {
		curent->firstemptyelem = i;
		break;
	    }
    }
    return (nrreturn+next_parsetimes*MAXLISTELEMENTS);
}
//=========================================
int mylist::DelList(int elemnr)
{
    listelem *curent;
    curent = first;
    if (!curent)
	return -1;
    while(elemnr>=MAXLISTELEMENTS)
    {
	curent = curent->next;
	if (!curent)
	    return -1;
	elemnr -= MAXLISTELEMENTS;
    }
    if (curent->elements[elemnr]==NULL)//cannot delete deleted entry
	return(-1);
    curent->elements[elemnr] = NULL;
    curent->bitelements[elemnr] = MYLIST_EMPTY;
    maxelements--;
    if (!maxelements)
    {
	FlushList();
    }
    else
    {
	if (++curent->emptyelem<MAXLISTELEMENTS)
	{
	    if (elemnr<=curent->firstemptyelem)
		curent->firstemptyelem = elemnr;
	}
	else
	{
	    //here emptyelem=maxlistelements, entire slot is empty, need to reorganize this
/*	    if (curent->prev)
	    {
		curent->prev->next=curent->next;
		if (curent->next)
		    curent->next->prev=curent->prev;
	    }
	    else
	    {
		first=curent->next;
	        if (curent->next)
		    curent->next->prev=NULL;
	    }
	    curentenum=curent->next;
    	    globalenumvalue-=enumvalue;
    	    enumvalue=0;
	    wfree(curent);
*/
	}
    }
    return 0;
}
//=========================================
int mylist::DelList(void *elem)
{
    int i,j=0;
    listelem *curent;
    curent = first;
    if (!first)
	return -1;
    while(curent)
    {
	for (i=0;i<MAXLISTELEMENTS;i++)
	    if (curent->elements[i]==elem)
		return(DelList(j+i));
	j+=MAXLISTELEMENTS;
	curent = curent->next;
    }
    return -1;
}
//=========================================
int mylist::SwapElements(int elemnr1,int elemnr2)
{
    void *tempelem;
    char tempelembit;
    int elemnrtemp;
    listelem *curent1,*curent2;
    curent1 = first;
    if (!first)
	return -1;
    if (elemnr1==elemnr2)
	return -1;
    if (elemnr1>elemnr2)
    {
	elemnrtemp = elemnr1;
	elemnr1 = elemnr2;
	elemnr2 = elemnrtemp;
    }
    if (elemnr2>=maxelemnr)
	return -2;
    while(elemnr1>=MAXLISTELEMENTS)
    {
	curent1 = curent1->next;
	if (!curent1)
	    return -3;
	elemnr1 -= MAXLISTELEMENTS;
	elemnr2 -= MAXLISTELEMENTS;
    }
    curent2=curent1;
    while(elemnr2>=MAXLISTELEMENTS)
    {
	curent2 = curent2->next;
	if (!curent2)
	    return -4;
	elemnr2 -= MAXLISTELEMENTS;
    }
    tempelem = curent2->elements[elemnr2];
    curent2->elements[elemnr2] = curent1->elements[elemnr1];
    curent1->elements[elemnr1] = tempelem;
    tempelembit = curent2->bitelements[elemnr2];
    curent2->bitelements[elemnr2] = curent1->bitelements[elemnr1];
    curent1->bitelements[elemnr1] = tempelembit;
    if (!curent2->bitelements[elemnr2])
	    curent2->firstemptyelem=elemnr2;
    if (!curent1->bitelements[elemnr1])
	    curent1->firstemptyelem=elemnr1;
    return 0;
}
//=========================================
void *mylist::GetElemNr(int elemnr)
{
    listelem *curent;
    curent = first;
    if (!first)
	return NULL;
    while(elemnr>=MAXLISTELEMENTS)
    {
	curent = curent->next;
	if (!curent)
	    return NULL;
	elemnr -= MAXLISTELEMENTS;
    }
    return(curent->elements[elemnr]);
}
//=========================================
void *mylist::GetElemNrNoNULL(int *elemnr)
{
    int elemcur;
    listelem *curent;
    elemcur = *elemnr;
//    if (*elemnr>=maxelements)
//	return NULL;
    curent = first;
    if (!first)
	return NULL;
    do{
	while(elemcur>=MAXLISTELEMENTS)
	{
	    curent = curent->next;
	    if (!curent)
		return NULL;
	    elemcur -= MAXLISTELEMENTS;
	}
	for (;elemcur<MAXLISTELEMENTS;elemcur++,(*elemnr)++)
	{
	    if (curent->elements[elemcur])
		return (curent->elements[elemcur]);
	}
    }while(1); 
}
//=========================================
int mylist::GetMaxElements(void)
{
    return(maxelements);
}
//=========================================
int mylist::GetMaxElemNr(void)
{
    return(maxelemnr);
}
//=========================================
void mylist::SortA(void)
{
    int i,j;
    char *elem1;
    char *elem2;
    for (i=0;i<GetMaxElements();i++)
    {
	for (j=i+1;j<GetMaxElements();j++)
	{
	    elem1=(char *)GetElemNr(i);
	    elem2=(char *)GetElemNr(j);
	    if (strcmp(elem1,elem2)>0)
		SwapElements(i,j);
	}
    }
}
//=========================================
void mylist::Shift(void)
{
    int i,ii,j,swapedtimes=0;
    void *elem,*elem2;
    j=GetMaxElemNr();
    for (i=0;i<j;i++)
    {
	elem=GetElemNr(i);
	if (!elem)
	{
	    for (ii=i+1;ii<j;ii++)
	    {
		elem2=GetElemNr(ii);
		if (elem2)
		{
		    SwapElements(i,ii);
		    if (++swapedtimes>=maxelements)
		    {
			return;
		    }
		    break;
		}
	    }
	    if (ii==j)
		break;
	}
    }
}
//=========================================
int mylist::HaveElem(void *elem)
{
    int i;
    listelem *curent;
    curent = first;
    while(curent)
    {
	for (i=0;i<MAXLISTELEMENTS;i++)
	{
	    if (curent->elements[i] == elem)
		return(1);
	}
	curent = curent->next;
    }
    return(0);
}
//=========================================
int mylist::AddUniqueElem(void *elem)
{
    if (!HaveElem(elem))
	return(AddList(elem));
    return(-1);
}
//=========================================
void mylist::EnumListInit(void)
{
    enumvalue=0;
    globalenumvalue=0;
    curentenum = first;
}
//=========================================
void *mylist::GetNextListElem(void)
{
    return(GetNextListElem(NULL));
}
//=========================================
void *mylist::GetNextListElem(int *posinlist)
{
    void *retval;
    if (!curentenum)
	return NULL;
    do{
	while(enumvalue>=MAXLISTELEMENTS)
	{
	    do{
		curentenum = curentenum->next;	//go to next slot
		if (!curentenum)
		    return NULL;
		enumvalue=0;
	    }while(curentenum->emptyelem==MAXLISTELEMENTS);	//repeat at next if we have empty entire slot
	}
	retval=curentenum->elements[enumvalue++];
	globalenumvalue++;
    }while(!retval);				//find next elem if curent is NULL
    if (posinlist)
	*posinlist=globalenumvalue-1;
    return(retval);
}
//=========================================
int mylist::GetLastElemNr(void)
{
    listelem *curent,*prev;
    int find=-1,i;
    curent=first;
    if (!curent)
	return(-1);
    while(curent)
    {
	prev=curent;
	curent=curent->next;
    }
    i=MAXLISTELEMENTS-1;
    while(i>=0&&find==-1)
    {
	if (prev->bitelements[i])
	    find=i;
	else
	    i--;
    }
    return(i);
}
//=========================================
void *mylist::GetLastElem(void)
{
    listelem *curent,*prev;
    int i;
    void *find=NULL;
    curent=first;
    if (!curent)
	return(NULL);
    while(curent)
    {
	prev=curent;
	curent=curent->next;
    }
    i=MAXLISTELEMENTS-1;
    while(i>=0&&find==NULL)
    {
	find=prev->elements[i--];
    }
    return(find);
}

//=========================================
cycles::cycles(int elems)
{
    totalelem = elems;
    curelem=0;
    poselem=0;
    elements = (void **) wmalloc (elems * sizeof (void *));
    memset(elements,0,elems * sizeof( void *) );
}
//=========================================
//=========================================
cycles::~cycles()
{
    for (int i=0;i<totalelem;i++)
    {
	DelElem(i);
    }
    wfree(elements);
}
//=========================================
void cycles::DelElem(int i)
{
    if (elements[i])
    {
        wfree(elements[i]);
        elements[i]=NULL;
    }
}
//=========================================
void *cycles::AddElem(int len)
{
    void *a;
    if (elements[curelem])
	return(NULL);
    a = elements[curelem] = wmalloc(len);
    curelem++;
    if (curelem>=totalelem)
	curelem=0; 
    return(a);
}
//=========================================
void *cycles::GetCurElem(void)
{
    void *a;
    a = elements[poselem];
    return(a);
}
//=========================================
void cycles::DelCurElem(void)
{
    void *a;
    a = elements[poselem];
    if (a)
    {
	DelElem(poselem);
	poselem++;
	if (poselem>=totalelem)
	    poselem=0;
    }
}
//=========================================
mylistsimple::mylistsimple(int neededelem)
{
    allocatedelem = neededelem;
    elements = (void **) wmalloc(allocatedelem*sizeof(void *));
    deletemarked = (char *)wmalloc(allocatedelem*sizeof(char));
    FreeAndEmptyAll();
}
//=========================================
mylistsimple::~mylistsimple()
{
    wfree(elements);
    wfree(deletemarked);
}
//=========================================
void mylistsimple::DeleteOneElem(int elemnr)
{
    int lastelem;
    lastelem = totalelem-1;
    if (elemnr != lastelem)
    {
	elements[elemnr] = elements[lastelem];
	deletemarked[elemnr] = deletemarked[lastelem];
    }
    else
	deletemarked[elemnr] = 0;
    totalelem--;
    lastelem--;
}
//=========================================
void mylistsimple::DeleteMarked(void)
{
    int i,lastelem;
    if (!totalmarked)
	return;
    totalmarked=0;
    lastelem = totalelem-1;
    for (i=lastelem;i>=0;i--)
    {
	if (deletemarked[i])
	{
	    if (i != lastelem)
	    {
		elements[i] = elements[lastelem];
		deletemarked[i] = deletemarked[lastelem];
	    }
	    else
		deletemarked[i] = 0;
	    totalelem--;
	    lastelem--;
	}
    }
}
//=========================================
void *mylistsimple::GetABSNextListElem(char *deleteflag)
{
    if (curenumelemnr >= totalelem)
	return(NULL);
    if (deleteflag)
	*deleteflag = deletemarked[curenumelemnr];
    return(elements[curenumelemnr++]);
}
//=========================================
void *mylistsimple::GetNextListElem(int *retelemnr)
{
    if (curenumelemnr>=totalelem)
	return(NULL);
    return(GetElem(curenumelemnr++,retelemnr));
}
//=========================================
void *mylistsimple::GetElem(int elemnr,int *retelem)
{
    while(deletemarked[elemnr])
    {
	if (++elemnr>=totalelem)
	    return(NULL);
    }
    if (retelem)
	*retelem = elemnr;
    return(elements[elemnr]);
}
//=========================================
void mylistsimple::FreeAndEmptyAll(void)
{
    totalelem=0;
    totalmarked=0;
    memset(elements,0,allocatedelem*sizeof(void *));
    memset(deletemarked,0,allocatedelem*sizeof(char));
    EnumListInit();
}
//=========================================
void mylistsimple::DelElem(void *elem)
{
    int i;
    int lastelem = totalelem-1;
    for (i=0;i<totalelem;i++)
    {
	if (elements[i] == elem)
	{
	    if (i != lastelem)
	    {
		elements[i] = elements[lastelem];
		deletemarked[i] = deletemarked[lastelem];
	    }
	    else
	    	deletemarked[i] = 0;
	    totalelem--;
	    return;
	}
    }
}



//=========================================
