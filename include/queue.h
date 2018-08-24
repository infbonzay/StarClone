
#if !defined(_QUEUE_W)
#define _QUEUE_W


#define QUEUEFULL		-1
#define QUEUEEMPTY		1
#define QUEUEOK			0
#define INCREMENTTICKS		1

#define NORMALPASSQUEUE		0
#define DESTROYQUEUE		1

struct QUEUEELEMENT
{
    void 	*ID;
    long 	executeTick;
};

class Queue
{
protected:
    long	currentTick;
    int		maxElements;
    int		queuedElements;
    struct QUEUEELEMENT *allElements;

    void	(*EndQueueCallBackFunction)(void *,int );
    void	DestroyAllElements(void);
    int 	InsertInQueue(int absolutegameticks);

public:
	 Queue();
	 Queue(void (*func)(void *,int ),int maxelements);
	~Queue(void);
    void EmptyQueue(void);
    int  AddToQueue(void *IDqueue,int waitgameticks);
    int  QueueMain(int addticks);
    void DelQueue(int from,int count);
    void ShowAllQueue(void);

    //========================
    inline int GetPlayTicks(void)
    {
	return currentTick;
    }
    //========================
    inline int GetQueueElements(void)
    {
	return queuedElements;
    }
    //========================
    inline struct QUEUEELEMENT *GetElemNr(int nr)
    {
	return &allElements[nr];
    }
//==========================================
};


#endif /*_QUEUE_W*/
