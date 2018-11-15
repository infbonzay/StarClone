#ifndef _LISTCPP_W
#define _LISTCPP_W

#include <stdlib.h>
#include <limits.h>

#include "List.h"
//=========================================
template <typename T>
void List<T>::Clear(void)
{
	Count = 0;
	First = NULL;
}
//=========================================
template <typename T>
List<T>::List(int maxElements)
{
	Capacity = maxElements;
	Clear();
}
//=========================================
template <typename T>
List<T>::List(void) : List(INT_MAX)
{
}
//=========================================
template <typename T>
List<T>::~List(void)
{
	ListElem<T> *next,*curent;
	next = curent = First;
	while(next)
	{
		next = curent->Next;
		delete curent;
	}
	Clear();
}
//=========================================
template <typename T>
void List<T>::Add(T elem)
{
	int i,nrreturn,next_parsetimes = 0;
	ListElem<T> *curent,*next;
	if (Capacity == Count)
		return;
	if (!First)
	{
		First = new ListElem<T>();
		memset(First,0,sizeof(ListElem<T>));
		First->EmptyElem = MAXLISTELEMENTS;
		First->FirstEmptyElem = 0;
	}
	curent = First;
	while(!curent->EmptyElem)
	{
		next_parsetimes++;
		if (!curent->Next)
		{
			curent->Next = new ListElem<T>;
			memset(curent->Next,0,sizeof(ListElem<T>));
			next = curent->Next;
			next->Prev = curent;
			next->EmptyElem = MAXLISTELEMENTS;
			next->FirstEmptyElem = 0;
			next->Prev = curent;
			next->Next = NULL;
			curent = next;
		}
		else
			curent = curent->Next;
	}
	nrreturn = curent->FirstEmptyElem;
	curent->Elements[nrreturn] = elem;
	curent->PresenceFlag[nrreturn] = true;
	Count++;
	curent->EmptyElem--;
	if (curent->EmptyElem)
	{
		for ( i=nrreturn+1; i<MAXLISTELEMENTS; i++ )
			if (curent->PresenceFlag[i])
			{
				curent->FirstEmptyElem = i;
				break;
			}
	}
}
//=========================================
template <typename T>
void List<T>::Remove(ListElem<T> current, int elemNr)
{
	if (!current)
		return;
	if (!current->PresenceFlag[elemNr])
		return;
	current->Elements[elemNr] = NULL;
	current->Presence[elemNr] = false;
	Count--;
	if (++current->EmptyElem < MAXLISTELEMENTS)
	{
		if (elemNr < current->FirstEmptyElem)
			current->FirstEmptyElem = elemNr;
	}
	else
	{
		//here emptyelem=maxlistelements, entire slot is empty, need to reorganize this
		if (current->Prev)
		{
			current->Prev->Next = current->Next;
			if (current->Next)
				current->Next->Prev = current->Prev;
		}
		else
		{
			First = current->Next;
			if (current->Next)
				current->Next->Prev=NULL;
		}
		delete current;
	}
	return;
}
//=========================================
template <typename T>
void List<T>::Remove(T elem)
{
	int i,j=0;
	ListElem<T> *curent = First;
	if (!First)
		return;
	while(curent)
	{
		for (i=0;i<MAXLISTELEMENTS;i++)
			if (curent->PresenceFlag[i] && curent->Elements[i] == elem)
				return(Remove(curent,j+i));
		j += MAXLISTELEMENTS;
		curent = curent->Next;
	}
	return;
}
//=========================================
template <typename T>
bool List<T>::Contains(T elem)
{
	int i,j=0;
	ListElem<T> *curent = First;
	if (!First)
		return false;
	while(curent)
	{
		for (i=0;i<MAXLISTELEMENTS;i++)
			if (curent->PresenceFlag[i])
			if (curent->Elements[i] == elem)
				return true;
		j += MAXLISTELEMENTS;
		curent = curent->Next;
	}
	return false;
}
//=========================================
template <typename T>
Enumerate<T>::Enumerate(List<T> *list)
{
	EnumValue = 0;
	GlobalEnumValue = 0;
	_list = list;
	CurrentEnum = list->First;
}
//=========================================
template <typename T>
Enumerate<T>::~Enumerate()
{
}
//=========================================
template <typename T>
T Enumerate<T>::GetNextElem(void)
{

	T retval;
	if (!CurrentEnum)
		return NULL;
	do{
		while(EnumValue >= MAXLISTELEMENTS)
		{
			do{
				CurrentEnum = CurrentEnum->Next;	//go to next slot
				if (!CurrentEnum)
					return NULL;
				EnumValue = 0;
			}while(CurrentEnum->EmptyElem == MAXLISTELEMENTS);	//repeat at next if we have empty entire slot
		}
		GlobalEnumValue++;
	}while(CurrentEnum->PresenceFlag[EnumValue++]);
	retval = CurrentEnum->Elements[--EnumValue];
	return(retval);
}

#endif
