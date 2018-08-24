
#include "wmem.h"
#include "queue.h"
#include "mycycle.h"
#include "commandqueue.h"

//==========================================
CommandQueue::CommandQueue(void (*callBackFunc)(void *),int maxElements):
mycycle(maxElements,MYCYCLE_STOPTHENFULL)
{
    EndQueueCallBackFunction = callBackFunc;
    currentTick=0;
}
//==========================================
CommandQueue::~CommandQueue(void)
{
    FlushQueue();
}
//==========================================
void CommandQueue::FlushQueue(void)
{
    do{
	if (IsEmpty())
	    break;
	wfree(PopElem());
    }while(1);
    Flush();
}
//==========================================
int CommandQueue::AppendToQueue(void *idQueue,int atTickNr)
{
    if (IsFull())
	return QUEUEFULL;
    QUEUEELEMENT *newelem = (QUEUEELEMENT *) wmalloc(sizeof(QUEUEELEMENT));
    newelem->ID = idQueue;
    newelem->executeTick = atTickNr;
    PushElem(newelem);
    return(QUEUEOK);
}
//==========================================
void CommandQueue::SetExecuteTick(long ticknr)
{
    currentTick = ticknr;
}
//==========================================
int CommandQueue::QueueMain()
{
    int queuedProceed = 0;
    QUEUEELEMENT *tempqueue;
    do{
	if (IsEmpty())
	    return(queuedProceed);
	if (GetNextElem()->executeTick != currentTick)
	    return(queuedProceed);
        tempqueue = PopElem();
	(*EndQueueCallBackFunction)(tempqueue->ID);
	queuedProceed++;
    }while(1);
}
