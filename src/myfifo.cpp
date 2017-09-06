
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef USEWMALLOC
    #include "wmem.h"

    #define __myalloc wmalloc
    #define __myfree  wfree
#else
    #define __myalloc malloc
    #define __myfree  free
#endif

#include "myfifo.h"

//=========================================
//=========================================
myfifo::myfifo(int elems)
{
    totalelem = elems;
    curelem=0;
    poselem=0;
    elements = (void **) __myalloc (elems * sizeof (void *));
    memset(elements,0,elems * sizeof( void *) );
}
//=========================================
//=========================================
myfifo::~myfifo()
{
    for (int i=0;i<totalelem;i++)
    {
	DelElem(i);
    }
    __myfree(elements);
}
//=========================================
void myfifo::DelElem(int i)
{
    if (elements[i])
    {
        __myfree(elements[i]);
        elements[i]=NULL;
    }
}
//=========================================
void myfifo::EmptyElemFifo(void)
{
    for (int i=0;i<totalelem;i++)
    {
	DelElem(i);
    }
    curelem=0;
    poselem=0;
}
//=========================================
void *myfifo::AddElem(int len)
{
    void *a;
    if (elements[curelem])
	return(NULL);
    a = elements[curelem] = __myalloc(len);
    curelem++;
    if (curelem>=totalelem)
	curelem=0; 
    return(a);
}
//=========================================
void *myfifo::InsertElem(int len)
{
    void *a;
    int newelem = poselem - 1;
    if (newelem<0)
	newelem = totalelem - 1;
    if (elements[newelem])
	return(NULL);
    poselem = newelem;
    a = elements[poselem] = __myalloc(len);
    return(a);
}
//=========================================
void *myfifo::GetCurElem(void)
{
    return(elements[poselem]);
}
//=========================================
void myfifo::DelCurElem(void)
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
int myfifo::GetUsedElem(void)
{
    if (curelem<poselem)
    {
	return(totalelem+curelem-poselem);
    }
    else
	if (curelem>poselem)
	{
	    return(curelem-poselem);
	}
	else
	{
	    if (!elements[curelem])
		return(0);
	    else
		return(totalelem);
	}
}
//=========================================
int myfifo::GetFreeElem(void)
{
    return(totalelem-GetUsedElem());    
}
//=========================================

