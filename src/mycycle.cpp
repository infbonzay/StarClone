#ifndef _MYCYCLECPP_W
#define _MYCYCLECPP_W

#include "wmem.h"
#include "mycycle.h"

//=========================================
template <typename T>
mycycle<T>::mycycle(T elems)
{
    if (elems > 0)
    {
	totalelem = elems;
	this->Flush();
	elements = (T *) wmalloc (elems * sizeof( T ));
//	memset(elements,0,elems * sizeof(T));
    }
}
//=========================================
template <typename T>
mycycle<T>::~mycycle()
{
    if (elements)
	wfree(elements);
}
//=========================================
template <typename T>
void mycycle<T>::Flush(void)
{
    firstelem = 0;
    lastelem = 0;
    maxelem = 0;
}
//=========================================
template <typename T>
void mycycle<T>::PushElem(T elem)
{
    elements[firstelem++] = elem;
    if (firstelem >= totalelem)
	firstelem = 0;
    if (++maxelem > totalelem)
    {
	maxelem--;
	if (++lastelem >= totalelem)
	    lastelem = 0;
    }
}
//=========================================
template <typename T>
T mycycle<T>::PopElem(void)
{
    T ret;
    if (maxelem)
    {
	maxelem--;
	ret = elements[lastelem++];
	if (lastelem >= totalelem)
	    lastelem = 0;
    }
    return(ret);
}
//=========================================

#endif
