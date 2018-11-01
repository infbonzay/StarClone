#if !defined(_LIST_W)
#define _LIST_W

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



#endif	/*		_MYLIST_W  */


