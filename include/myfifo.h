#if !defined(_MYFIFO_W)
#define _MYFIFO_W



class myfifo
{
public:
    int  	totalelem;	
    int  	curelem;	
    int  	poselem;	
    void 	**elements;	
    
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
//    inline int  GetUseBytes(void) { return(usebytes[poselem]); };
//    inline void SetUseBytes(int bytes) { usebytes[poselem] = bytes; };
};
//=========================================


#endif


