#ifndef _SC_ENUMERATE_H

#define _SC_ENUMERATE_H
//=========================================
#include <stdlib.h>

template <typename T>
class Enumerator
{
protected:
	int	_PosElem;
	T	*_Elements;
	int *_MaxElements;
public:
	Enumerator<T>(int *MaxElem, T *Elems, int EnumerateFrom);
	Enumerator<T>(int *MaxElem, T *Elems);
	~Enumerator<T>();

	T GetNext();
};

//=========================================
template <typename T>
Enumerator<T>::Enumerator(int *MaxElem, T *Elems)
{
	_MaxElements = MaxElem;
	_Elements = Elems;
	_PosElem = 0;
}
//=========================================
template <typename T>
Enumerator<T>::Enumerator(int *MaxElem, T *Elems, int EnumerateFrom)
{
	_MaxElements = MaxElem;
	_Elements = Elems;
	_PosElem = EnumerateFrom;
}
//=========================================
template <typename T>
Enumerator<T>::~Enumerator()
{
}
//=========================================
template <typename T>
T Enumerator<T>::GetNext(void)
{
	if (_PosElem < *_MaxElements)
	{
		return (_Elements[_PosElem++]);
	}
	return NULL;
}
//=========================================
#endif
