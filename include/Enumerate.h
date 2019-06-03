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
	virtual ~IEnumerate<T>() {}
	virtual T GetNext() = 0;
};
//=========================================
template <typename T>
class Enumerate : IEnumerate<T>
{
protected:
	int	_PosElem;
	T	*_Elements;
	int *_MaxElements;
public:
	Enumerate<T>(int *MaxElem, T *Elems, int EnumerateFrom);
	Enumerate<T>(int *MaxElem, T *Elems);
	~Enumerate<T>();

	T GetNext();
};
//=========================================
template <typename T>
class EnumerateList : public IEnumerate<T>
{
protected:
	int				EnumValue;
	int				GlobalEnumValue;
	List<T>			*_list;
	ListElem<T>		*CurrentEnum;
public:
	T	GetNext(void);

	EnumerateList(List<T> *list);
	~EnumerateList();
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
T Enumerate<T>::GetNext(void)
{
	if (_PosElem < *_MaxElements)
	{
		return (_Elements[_PosElem++]);
	}
	return NULL;
}
//=========================================
//=========================================

//=========================================
template <typename T>
EnumerateList<T>::EnumerateList(List<T> *list)
{
	EnumValue = 0;
	GlobalEnumValue = 0;
	_list = list;
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
		while (EnumValue >= MAXLISTELEMENTS)
		{
			do {
				CurrentEnum = CurrentEnum->Next;	//go to next slot
				if (!CurrentEnum)
					return NULL;
				EnumValue = 0;
			} while (CurrentEnum->EmptyElem == MAXLISTELEMENTS);	//repeat at next if we have empty entire slot
		}
		if (CurrentEnum->PresenceFlag[EnumValue])
			break;
		GlobalEnumValue++;
	} while (1);
	return(CurrentEnum->Elements[EnumValue++]);
}


#endif
