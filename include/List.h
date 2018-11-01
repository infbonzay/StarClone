#if !defined(_LIST_W)
#define _LIST_W

#define MAXLISTELEMENTS	64
#define MYLIST_EMPTY	0
#define MYLIST_FULL		1


template <typename T> 
struct ListElem
{
	int	 			EmptyElem;
	int	 			FirstEmptyElem;
	ListElem<T>		*Next;
	ListElem<T> 	*Prev;
	T				Elements[MAXLISTELEMENTS];
	unsigned char	PresenceFlag[MAXLISTELEMENTS];
};

template <typename T> 
class List
{
private:
	int				MaxElements;
	int				Elements;
	ListElem<T> 	*First;
	int				EnumValue;
	int				GlobalEnumValue;
	ListElem<T>		*CurentEnum;

public:
	void			EnumListInit(void);
	void			Empty(void);
	void			Add(T	elem);
	void			Del(ListElem<T> curent, int elemnr);
	void			Del(T elemnr);
	T				GetNextElem(void);
//=========================================

	List();
	List(int maxElements);
	~List();
};
//=========================================

#endif	/*		_MYLIST_W  */


