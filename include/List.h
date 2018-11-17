#if !defined(_LIST_W)
#define _LIST_W

#include <stdlib.h>
#include <limits.h>
#include "debug.h"

#define MAXLISTELEMENTS	64


template <typename T>
struct ListElem
{
protected:
	int	 			EmptyElem;
	int	 			FirstEmptyElem;
	ListElem<T>		*Next;
	ListElem<T> 	*Prev;
	T				Elements[MAXLISTELEMENTS];
	bool			PresenceFlag[MAXLISTELEMENTS];
};

template <typename T>
class List
{
protected:
	int				Capacity;
	int				Count;
	ListElem<T> 	*First;

	void			Remove(ListElem<T> current, int elemnr);

public:
	void			Clear(void);
	void			Add(T elem);
	void			Remove(T elemnr);
	bool			Contains(T elem);

	List();
	List(int maxElements);
	~List();
};
//=========================================

template <typename T>
class Enumerate
{
protected:
	int				EnumValue;
	int				GlobalEnumValue;
	List<T>			*_list;
	ListElem<T>		*CurrentEnum;
public:
	T	GetNextElem(void);

	Enumerate(List<T> *list);
	~Enumerate();
};

//=========================================
template <typename T>
void List<T>::Clear(void)
{
	ListElem<T> *next, *curent;
	next = curent = First;
	while (next)
	{
		next = curent->Next;
		delete curent;
	}
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
	Clear();
}
//=========================================
template <typename T>
void List<T>::Add(T elem)
{
	int i, nrreturn, next_parsetimes = 0;
	ListElem<T> *curent, *next;
	if (Capacity == Count)
		return;
	if (!First)
	{
		First = new ListElem<T>();
		memset(First, 0, sizeof(ListElem<T>));
		First->EmptyElem = MAXLISTELEMENTS;
		First->FirstEmptyElem = 0;
	}
	curent = First;
	while (!curent->EmptyElem)
	{
		next_parsetimes++;
		if (!curent->Next)
		{
			curent->Next = new ListElem<T>;
			memset(curent->Next, 0, sizeof(ListElem<T>));
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
	curent->PrecenseFlag[nrreturn] = true;
	Count++;
	curent->EmptyElem--;
	if (curent->EmptyElem)
	{
		for (i = nrreturn + 1; i < MAXLISTELEMENTS; i++)
			if (!curent->PresenceFlag[i])
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
				current->Next->Prev = NULL;
		}
		delete current;
	}
	return;
}
//=========================================
template <typename T>
void List<T>::Remove(T elem)
{
	int i, j = 0;
	ListElem<T> *curent = First;
	if (!First)
		return;
	while (curent)
	{
		for (i = 0;i < MAXLISTELEMENTS;i++)
			if (curent->PresenceFlag[i] && curent->Elements[i] == elem)
				return(Remove(curent, j + i));
		j += MAXLISTELEMENTS;
		curent = curent->Next;
	}
	return;
}
//=========================================
template <typename T>
bool List<T>::Contains(T elem)
{
	int i, j = 0;
	ListElem<T> *curent = First;
	if (!First)
		return false;
	while (curent)
	{
		for (i = 0;i < MAXLISTELEMENTS;i++)
			if (curent->PresenceFlag[i] && curent->Elements[i] == elem)
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

//=========================================
template <typename T>
class ListSimple
{
public:
	int				allocatedelem;
	int				totalelem;
	int				totalmarked;
	int				curenumelemnr;
	T				*elements;
	bool			*deletemarked;

	ListSimple<T>(int neededelem);
	~ListSimple<T>();
	
	inline void Add(T elem) 
	{
		if (totalelem < allocatedelem) 
			elements[totalelem++] = elem;
		else
			DEBUGMESSCR("ListSimple:max elems(%d) exceeded\n",totalelem);
	};
	inline void MarkForDelElem(int elemnr)
	{
		if (!deletemarked[elemnr])
		{
			totalmarked++;
			deletemarked[elemnr] = true;
		}
	};
	inline int  GetMaxElements(void) { return (totalelem); };
	inline void EnumListInit(void) { curenumelemnr = 0; };
	inline void Set(int elemnr,T elem) { elements[elemnr] = elem; };
	T			GetNextListElem(int *elemnr);
	T			GetElem(int elemnr,int *retelemnr);
	int 		RetIfContains(T elem);
	bool 		Contains(T elem);
	T			GetABSNextListElem(bool *deleteflag);
	void 		DeleteOneElem(int elemnr);
	void 		DeleteMarked(void);
	void 		FreeAndEmptyAll(void);
	inline int	GetFreeElements(void){ return(allocatedelem - totalelem); };
	void		DelElem(T elem);
	void		CopyTo(ListSimple *copyIn);
	void		AppendTo(ListSimple *appendTo);

};
//=========================================
template <typename T>
ListSimple<T>::ListSimple(int neededelem)
{
	allocatedelem = neededelem;
	elements = new T[neededelem];
	deletemarked = new bool[neededelem];
	FreeAndEmptyAll();
}
//=========================================
template <typename T>
ListSimple<T>::~ListSimple()
{
	delete[] elements;
	delete[] deletemarked;
}
//=========================================
template <typename T>
void ListSimple<T>::DeleteOneElem(int elemnr)
{
	int lastelem;
	lastelem = totalelem - 1;
	if (elemnr != lastelem)
	{
		elements[elemnr] = elements[lastelem];
		deletemarked[elemnr] = deletemarked[lastelem];
	}
	deletemarked[lastelem] = false;
	totalelem--;
	lastelem--;
}
//=========================================
template <typename T>
void ListSimple<T>::DeleteMarked(void)
{
	int i, lastelem;
	if (!totalmarked)
		return;
	totalmarked = 0;
	lastelem = totalelem - 1;
	for (i = lastelem;i >= 0;i--)
	{
		if (deletemarked[i])
		{
			DeleteOneElem(i);
		}
	}
}
//=========================================
template <typename T>
T	ListSimple<T>::GetABSNextListElem(bool *deleteflag)
{
	if (curenumelemnr >= totalelem)
		return(NULL);
	if (deleteflag)
		*deleteflag = deletemarked[curenumelemnr];
	return(elements[curenumelemnr++]);
}
//=========================================
template <typename T>
T	ListSimple<T>::GetNextListElem(int *retelemnr)
{
	if (curenumelemnr >= totalelem)
		return(NULL);
	return(GetElem(curenumelemnr++, retelemnr));
}
//=========================================
template <typename T>
T	ListSimple<T>::GetElem(int elemnr,int *retelem)
{
	while (deletemarked[elemnr])
	{
		if (++elemnr >= totalelem)
			return(NULL);
	}
	if (retelem)
		*retelem = elemnr;
	return(elements[elemnr]);
}
//=========================================
template <typename T>
void ListSimple<T>::FreeAndEmptyAll(void)
{
	totalelem = 0;
	totalmarked = 0;
	memset(elements, 0, allocatedelem * sizeof(void *));
	memset(deletemarked, 0, allocatedelem * sizeof(char));
	EnumListInit();
}
//=========================================
template <typename T>
void ListSimple<T>::CopyTo(ListSimple *copyTo)
{
	copyTo->FreeAndEmptyAll();
	this->AppendTo(copyTo);
}
//=========================================
template <typename T>
void ListSimple<T>::AppendTo(ListSimple *appendTo)
{
	this->EnumListInit();
	T elem;
	while ((elem = this->GetNextListElem(NULL)))
	{
		if (!appendTo->Contains(elem))
		{
			appendTo->Add(elem);
		}
	}
}
//=========================================
template <typename T>
void ListSimple<T>::DelElem(T	elem)
{
	for (int i = 0;i < totalelem;i++)
	{
		if (elements[i] == elem)
		{
			DeleteOneElem(i);
			return;
		}
	}
}
//=========================================
template <typename T>
int ListSimple<T>::RetIfContains(T	elem)
{
	for (int i = 0;i < totalelem;i++)
	{
		if (elements[i] == elem)
		{
			return i;
		}
	}
	return -1;
}
//=========================================
template <typename T>
bool ListSimple<T>::Contains(T	elem)
{
	return (RetIfContains(elem) >= 0);
}


#endif	/*		_MYLIST_W  */


