#ifndef _MYCYCLECPP_W
#define _MYCYCLECPP_W

#include "wmem.h"
#include "mycycle.h"

//=========================================
template <typename T>
mycycle<T>::mycycle(int elems, int flg)
{
    if (elems > 0)
    {
	flags = flg;
	totalelem = elems;
	this->Flush();
	elements = (T *) wmalloc (elems * sizeof( T ));
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
int mycycle<T>::PushElem(T elem)
{
    if ( (flags & MYCYCLE_STOPTHENFULL) && IsFull())
        return MYCYCLE_FAILED;
	
    elements[firstelem++] = elem;
    if (firstelem >= totalelem)
	firstelem = 0;
    if (++maxelem > totalelem)
    {
	maxelem--;
	if (++lastelem >= totalelem)
	    lastelem = 0;
    }
    return MYCYCLE_SUCCESS;
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
