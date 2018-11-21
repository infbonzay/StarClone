
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "wmem.h"

#include "myfifo.h"

//=========================================
//=========================================
myfifo::myfifo(int elems)
{
	_Capacity = elems;
	_Count = 0;
	PosElem = 0;
	elements = (void **)wmalloc(elems * sizeof(void *));
	memset(elements, 0, elems * sizeof(void *));
}
//=========================================
myfifo::~myfifo()
{
	for (int i = 0;i < _Capacity;i++)
	{
		Remove(i);
	}
	wfree(elements);
}
//=========================================
void myfifo::Remove(int i)
{
	if (elements[i])
	{
		wfree(elements[i]);
		elements[i] = NULL;
	}
}
//=========================================
void myfifo::Clear(void)
{
	for (int i = 0;i < _Capacity;i++)
	{
		Remove(i);
	}
	_Count = 0;
	PosElem = 0;
}
//=========================================
void *myfifo::Add(int len)
{
	void *a;
	if (elements[_Count])
		return(NULL);
	a = elements[_Count] = wmalloc(len);
	_Count++;
	if (_Count >= _Capacity)
		_Count = 0;
	return(a);
}
//=========================================
void *myfifo::Insert(int len)
{
	void *a;
	int newelem = PosElem - 1;
	if (newelem < 0)
		newelem = _Capacity - 1;
	if (elements[newelem])
		return(NULL);
	PosElem = newelem;
	a = elements[PosElem] = wmalloc(len);
	return(a);
}
//=========================================
void *myfifo::GetCurElem(void)
{
	return(elements[PosElem]);
}
//=========================================
void myfifo::DelCurElem(void)
{
	void *a;
	a = elements[PosElem];
	if (a)
	{
		Remove(PosElem);
		PosElem++;
		if (PosElem >= _Capacity)
			PosElem = 0;
	}
}
//=========================================
int myfifo::GetUsedElem(void)
{
	if (_Count < PosElem)
	{
		return(_Capacity + _Count - PosElem);
	}
	else
		if (_Count > PosElem)
		{
			return(_Count - PosElem);
		}
		else
		{
			if (!elements[_Count])
				return(0);
			else
				return(_Capacity);
		}
}
//=========================================
int myfifo::GetFreeElem(void)
{
	return(_Capacity - GetUsedElem());
}
//=========================================

