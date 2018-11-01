#include <stdlib.h>
#include <limits.h>

#include "List.h"

template <typename T>
void List<T>::EnumListInit(void)
{
	EnumValue = 0;
	GlobalEnumValue = 0;
	CurentEnum = First;
}
//=========================================
template <typename T>
void List<T>::Empty(void)
{
	Elements = 0;
	First = NULL;
	EnumListInit();
}
//=========================================
template <typename T>
List<T>::List(int maxElements)
{
	MaxElements = maxElements;
	Empty();
}
//=========================================
template <typename T>
List<T>::List(void) : List(INT_MAX)
{
	Empty();
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
	Empty();
}
//=========================================
template <typename T>
void List<T>::Add(T elem)
{
	int i,nrreturn,next_parsetimes = 0;
	ListElem<T> *curent,*next;
	if (MaxElements == Elements)
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
	curent->PrecenseFlag[nrreturn] = MYLIST_FULL;
	Elements++;
	curent->EmptyElem--;
	if (curent->EmptyElem)
	{
		for ( i=nrreturn+1; i<MAXLISTELEMENTS; i++ )
			if (curent->PresenceFlag[i] == MYLIST_EMPTY)
			{
				curent->FirstEmptyElem = i;
				break;
			}
	}
}
//=========================================
template <typename T>
void List<T>::Del(ListElem<T> curent, int elemNr)
{
	if (!curent)
		return;
	if (curent->PresenceFlag[elemNr] == MYLIST_EMPTY)
		return;
	curent->Elements[elemNr] = NULL;
	curent->Presence[elemNr] = MYLIST_EMPTY;
	Elements--;
	if (++curent->EmptyElem < MAXLISTELEMENTS)
	{
		if (elemNr < curent->FirstEmptyElem)
			curent->FirstEmptyElem = elemNr;
	}
	else
	{
		//here emptyelem=maxlistelements, entire slot is empty, need to reorganize this
		if (curent->Prev)
		{
			curent->Prev->Next = curent->Next;
			if (curent->Next)
				curent->Next->Prev = curent->Prev;
		}
		else
		{
			First = curent->Next;
			if (curent->Next)
				curent->Next->Prev=NULL;
		}
		CurentEnum = curent->Next;
		GlobalEnumValue -= EnumValue;
		EnumValue = 0;
		delete curent;
	}
	return;
}
//=========================================
template <typename T>
void List<T>::Del(T elem)
{
	int i,j=0;
	ListElem<T> *curent = First;
	if (!First)
		return;
	while(curent)
	{
		for (i=0;i<MAXLISTELEMENTS;i++)
			if (curent->Elements[i] == elem)
				return(Del(curent,j+i));
		j += MAXLISTELEMENTS;
		curent = curent->Next;
	}
	return;
}
//=========================================
template <typename T>
T List<T>::GetNextElem(void)
{
	T retval;
	if (!CurentEnum)
		return NULL;
	do{
		while(EnumValue >= MAXLISTELEMENTS)
		{
			do{
				CurentEnum = CurentEnum->Next;	//go to next slot
				if (!CurentEnum)
					return NULL;
				EnumValue = 0;
			}while(CurentEnum->EmptyElem == MAXLISTELEMENTS);	//repeat at next if we have empty entire slot
		}
		retval = CurentEnum->Elements[EnumValue];
		GlobalEnumValue++;
	}while(!CurentEnum->PresenceFlag[EnumValue++]);							//find next elem if curent is NULL
	return(retval);
}
//=========================================
