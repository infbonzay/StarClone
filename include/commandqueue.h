
#if !defined(_QUEUECYCLE_W)
#define _QUEUECYCLE_W


#include "mycycle.h"
#include "queue.h"

#define QUEUENOTOBEEXECUTED	2

class CommandQueue : public mycycle<QUEUEELEMENT *>
{
protected:
    long	currentTick;
    void	(*EndQueueCallBackFunction)(void *);
    void	DestroyAllElements(void);

public:
	 CommandQueue(void (*callBackFunc)(void *),int maxElements);
	~CommandQueue(void);
    void SetExecuteTick(long ticknr);
    void FlushQueue(void);
    int  AppendToQueue(void *idQueue,int absolutegameticks);
    int  QueueMain();
};
//==========================================

#endif /*_QUEUE_W*/
