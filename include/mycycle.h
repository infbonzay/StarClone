#ifndef _MYCYCLEH_W
#define _MYCYCLEH_W


#include "wmem.h"

//=========================================
#define MYCYCLE_FAILED			0
#define MYCYCLE_SUCCESS			1
#define MYCYCLE_INFINITE		0
#define MYCYCLE_STOPTHENFULL	0x01


template <typename T>
class  mycycle
{
	int			totalelem;
	int			firstelem;
	int			lastelem;
	int			maxelem;
	T			*elements;
	char		flags;
public:
				mycycle<T>(int elems);
				mycycle<T>(int elems,int flg);
				~mycycle();
	int			PushElem(T elem);
	T			PopElem(void);
	void		Flush(void);
	inline int	IsEmpty(void){ return( maxelem == 0); };
	inline int	IsFull(void){ return( maxelem == totalelem); };
	inline T	GetNextElem(void) { return elements[lastelem]; };
};
//=========================================
template <typename T>
mycycle<T>::mycycle(int elems, int flg)
{
	if (elems > 0)
	{
		flags = flg;
		totalelem = elems;
		this->Flush();
		elements = (T *)wmalloc(elems * sizeof(T));
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
	if ((flags & MYCYCLE_STOPTHENFULL) && IsFull())
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

#endif	/*		_MYCYCLE_W	*/


