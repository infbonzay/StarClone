#if !defined(_LIST_W)
#define _LIST_W

#define MAXLISTELEMENTS	64


template <typename T> 
struct ListElem
{
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
private:
	int				Capacity;
	int				Count;
	ListElem<T> 	*First;
	ListElem<T>		*CurentEnum;
	//int				EnumValue;
	//int				GlobalEnumValue;

	void			Remove(ListElem<T> curent, int elemnr);

public:
	void			Clear(void);
	void			Add(T elem);
	void			Remove(T elemnr);
	bool			Contains(T elem);

	//void			EnumInit(void);
	//T				GetNextElem(void);
//=========================================

	List();
	List(int maxElements);
	~List();
};
//=========================================

template <typename T>
class Enumerate : public List<T>
{
	int				EnumValue;
	int				GlobalEnumValue;

public:
	T				GetNextElem(void);
	
	Enumerate(List<T> *list);
	~Enumerate();
}



#endif	/*		_MYLIST_W  */


