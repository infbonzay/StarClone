#ifndef _MYCYCLEH_W
#define _MYCYCLEH_W

//=========================================
template <typename T> 
class  mycycle
{
    T		totalelem;
    T		firstelem;
    T		lastelem;
    T		maxelem;
    T		*elements;
public:
		mycycle(T elems);
		~mycycle();
    void 	PushElem(T elem);
    T		PopElem(void);
    void	Flush(void);
    inline T	IsEmpty(void){ return( maxelem == 0); };
};
//=========================================
#include "../src/mycycle.cpp"

#endif  /*	_MYCYCLE_W  */


