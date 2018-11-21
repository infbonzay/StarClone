#if !defined(_MYFIFO_W)
#define _MYFIFO_W

#include "wmem.h"

//=========================================
template <typename T>
class MyFifo
{
protected:
	int			_Capacity;
	int			_Count;
	int			PosElem;
	T			*elements;
	char		flags;

public:
	MyFifo<T>(int capacity);
	~MyFifo<T>();

	inline int	Count(){ return _Count; };
	inline int	Capacity(){ return _Capacity; };
	void		Clear();
	void		Remove(int elemnr);
	T			Add(int elemsize);
	T			Insert(int elemsize);
	T			GetCurElem();
	T			At(int elemnr){ return elements[elemnr]; };
	void		DelCurElem();
	int			GetFreeElem();
	int			GetUsedElem();
};

//=========================================
template <typename T>
MyFifo<T>::MyFifo(int capacity)
{
	_Capacity = capacity;
	_Count = 0;
	PosElem = 0;
	elements = new T[capacity];
	memset(elements, 0, capacity * sizeof(T));
}
//=========================================
template <typename T>
MyFifo<T>::~MyFifo()
{
	Clear();
	delete[] elements;
}
//=========================================
template <typename T>
void MyFifo<T>::Remove(int i)
{
	if (elements[i])
	{
		wfree(elements[i]);
		elements[i] = NULL;
	}
}
//=========================================
template <typename T>
void MyFifo<T>::Clear(void)
{
	for (int i = 0;i < _Capacity;i++)
	{
		Remove(i);
	}
	_Count = 0;
	PosElem = 0;
}
//=========================================
template <typename T>
T	MyFifo<T>::Add(int elemsize)
{
	T a;
	if (elements[_Count])
		return(NULL);
	a = elements[_Count] = (T) wmalloc(elemsize);
	_Count++;
	if (_Count >= _Capacity)
		_Count = 0;
	return(a);
}
//=========================================
template <typename T>
T MyFifo<T>::Insert(int elemsize)
{
	T a;
	int newelem = PosElem - 1;
	if (newelem < 0)
		newelem = _Capacity - 1;
	if (elements[newelem])
		return(NULL);
	PosElem = newelem;
	a = elements[PosElem] = (T) wmalloc(elemsize);
	return(a);
}
//=========================================
template <typename T>
T MyFifo<T>::GetCurElem(void)
{
	return(elements[PosElem]);
}
//=========================================
template <typename T>
void MyFifo<T>::DelCurElem(void)
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
template <typename T>
int MyFifo<T>::GetUsedElem(void)
{
	if (_Count < PosElem)
	{
		return(_Capacity + _Count - PosElem);
	}
	else
	{
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
}
//=========================================
template <typename T>
int MyFifo<T>::GetFreeElem(void)
{
	return(_Capacity - GetUsedElem());
}
//=========================================

#endif


