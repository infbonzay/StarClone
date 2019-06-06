#ifndef _SC_ENUMERATE_H

#define _SC_ENUMERATE_H
//=========================================
#include <stdlib.h>
#include <List.h>
//=========================================
template <typename T>
class IEnumerate
{
public:
	virtual ~IEnumerate<T>(void) {}
	virtual T GetNext(void) = 0;
	virtual T GetBack(void) = 0;
};
//=========================================
template <typename T>
class Enumerate : public IEnumerate<T>
{
protected:
	int	_PosElem;
	T	*_Elements;
	int *_MaxElements;
public:
	Enumerate<T>(int *MaxElem, T *Elems, int EnumerateFrom);
	Enumerate<T>(int *MaxElem, T *Elems);
	~Enumerate<T>();

	T GetNext(int *curpos);
	T GetNext(void);
	T GetBack(void);
};
//=========================================
template <typename T>
class EnumerateList : public IEnumerate<T>
{
protected:
	int				_PosElem;
	ListElem<T>		*CurrentEnum;
public:

	EnumerateList(List<T> *list);
	~EnumerateList();

	T GetNext(void);
	T GetBack(void);

};
//=========================================
//=========================================
//=========================================
template <typename T>
Enumerate<T>::Enumerate(int *MaxElem, T *Elems)
{
	_MaxElements = MaxElem;
	_Elements = Elems;
	_PosElem = 0;
}
//=========================================
template <typename T>
Enumerate<T>::Enumerate(int *MaxElem, T *Elems, int EnumerateFrom)
{
	_MaxElements = MaxElem;
	_Elements = Elems;
	_PosElem = EnumerateFrom;
}
//=========================================
template <typename T>
Enumerate<T>::~Enumerate()
{
}
//=========================================
template <typename T>
T Enumerate<T>::GetNext(int *curpos)
{
	if (_PosElem < *_MaxElements)
	{
		if (curpos)
		{
			*curpos = _PosElem;
		}
		return (_Elements[_PosElem++]);
	}
	return NULL;
}
//=========================================
template <typename T>
T Enumerate<T>::GetNext(void) { return GetNext(NULL); };
//=========================================
template <typename T>
T Enumerate<T>::GetBack(void)
{
	if (_PosElem >= 0)
	{
		return (_Elements[_PosElem--]);
	}
	return NULL;
}
//=========================================
//=========================================

//=========================================
template <typename T>
EnumerateList<T>::EnumerateList(List<T> *list)
{
	_PosElem = 0;
	//GlobalEnumValue = 0;
	//_list = list;
	CurrentEnum = list->First;
}
//=========================================
template <typename T>
EnumerateList<T>::~EnumerateList()
{
}
//=========================================
template <typename T>
T EnumerateList<T>::GetNext(void)
{

	T retval;
	if (!CurrentEnum)
		return NULL;
	do {
		while (_PosElem >= MAXLISTELEMENTS)
		{
			do {
				CurrentEnum = CurrentEnum->Next;	//go to next slot
				if (!CurrentEnum)
					return NULL;
				_PosElem = 0;
			} while (CurrentEnum->EmptyElem == MAXLISTELEMENTS);	//repeat at next if we have empty entire slot
		}
		if (CurrentEnum->PresenceFlag[_PosElem])
			break;
		//GlobalEnumValue++;
	} while (1);
	return(CurrentEnum->Elements[_PosElem++]);
}
//=========================================
template <typename T>
T EnumerateList<T>::GetBack(void)
{
	return NULL;
}

#endif
