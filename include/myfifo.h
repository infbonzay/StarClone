#if !defined(_MYFIFO_W)
#define _MYFIFO_W

class myfifo
{
public:
    int  	totalelem;
    int  	curelem;
    int  	poselem;
    void 	**elements;
    char	flags;
    
    myfifo(int maxelems);
    ~myfifo();

    void	EmptyElemFifo();
    void 	DelElem(int elemnr);
    void 	*AddElem(int len);
    void 	*InsertElem(int len);
    void 	*GetCurElem();
    void 	DelCurElem();
    int  	GetFreeElem();
    int  	GetUsedElem();
};
//=========================================


#endif


