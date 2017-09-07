
#if !defined(_QUEUE_W)
#define _QUEUE_W


#define QUEUEFULL		-1
#define INCREMENTTICKS		1

#define NORMALPASSQUEUE		0
#define DESTROYQUEUE		1

struct QUEUEELEMENT
{
    void *ID;
    int timetoexecute;
};

class Queue
{
    long PlayTicks;
    int MaxElements;
    int QueueElements;
    struct QUEUEELEMENT *Elements;
    void (*EndQueueCallBackFunction)(void *,int );
    void DestroyAllElements(void);
    
    int InsertInQueue(int absolutegameticks);
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
	return PlayTicks;
    }
    //========================
    inline int GetQueueElements(void)
    {
	return QueueElements;
    }
    //========================
    inline struct QUEUEELEMENT *GetElemNr(int nr)
    {
	return &Elements[nr];
    }
//==========================================
};


#endif /*_QUEUE_W*/
