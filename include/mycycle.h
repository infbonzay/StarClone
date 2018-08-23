#ifndef _MYCYCLEH_W
#define _MYCYCLEH_W

//=========================================
#define MYCYCLE_FAILED		0
#define MYCYCLE_SUCCESS		1
#define MYCYCLE_INFINITE	0
#define MYCYCLE_STOPTHENFULL	0x01


template <typename T> 
class  mycycle
{
    T		totalelem;
    T		firstelem;
    T		lastelem;
    T		maxelem;
    T		*elements;
    char	flags;
public:
		mycycle(T elems);
		mycycle(T elems,int flg);
		~mycycle();
    int 	PushElem(T elem);
    T		PopElem(void);
    void	Flush(void);
    inline int	IsEmpty(void){ return( maxelem == 0); };
    inline int	IsFull(void){ return( maxelem == totalelem); };
};
//=========================================
#include "../src/mycycle.cpp"

#endif  /*	_MYCYCLE_W  */


