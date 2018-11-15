#ifndef _LISTH_W
#define _LISTH_W

#define MAXLISTELEMENTS	64


template <typename T>
struct ListElem
{
public:
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

	void			Remove(ListElem<T> current, int elemnr);

public:
	ListElem<T> 	*First;

	void			Clear(void);
	void			Add(T elem);
	void			Remove(T elemnr);
	bool			Contains(T elem);

	List<T>();
	List<T>(int maxElements);
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

	Enumerate<T>(List<T> *list);
	~Enumerate<T>();
};

#include "../src/List.cpp"

#endif	/*		_LISTH_W  */