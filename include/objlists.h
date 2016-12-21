#if !defined(_OBJLISTS_W)
#define _OBJLISTS_W


#define MAXCRITFUNC		10
#define MAXELEMOFFSET		1000
#define ENDLIST			-1
class OBJlists
{
private:
    int  CritFunc;
    int  (*AllCriteriesFunc[MAXCRITFUNC])(int elem_id);
    int  *elems[MAXCRITFUNC];
    int	 curelements[MAXCRITFUNC];
    int	 maxelements[MAXCRITFUNC];
    int  seekpos[MAXCRITFUNC];
public:
    int  AddCriteries(int (*func)(int elem_id),int maxelem);
    void AddElem(int elem_id);
    void SetFirstSeek(int criteria_id);
    int  FindNextElem(int criteria_id);
    void DelElem(int elem_id);
    void ClearAll(void);
    ~OBJlists();

};
#endif  /*	_OBJLISTS_W  */


