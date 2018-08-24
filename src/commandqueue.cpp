
#include "wmem.h"
#include "queue.h"
#include "mycycle.h"
#include "commandqueue.h"

//==========================================
CommandQueue::CommandQueue(void (*callBackFunc)(COMMANDQUEUEELEMENT *),int maxElements):
mycycle(maxElements,MYCYCLE_STOPTHENFULL)
{
    EndQueueCallBackFunction = callBackFunc;
    currentTick=0;
}
//==========================================
CommandQueue::~CommandQueue(void)
{
    EmptyQueue();
}
//==========================================
void CommandQueue::EmptyQueue(void)
{
    do{
	if (IsEmpty())
	    break;
	wfree(PopElem());
    }while(1);
    Flush();
}
//==========================================
int CommandQueue::AppendToQueue(COMMANDQUEUEELEMENT *idQueue)
{
    if (IsFull())
	return QUEUEFULL;
    COMMANDQUEUEELEMENT *newelem = (COMMANDQUEUEELEMENT *) wmalloc(sizeof(COMMANDQUEUEELEMENT));
    newelem->queueaction = {
	.actiontype = idQueue->queueaction.actiontype,
	.obj = idQueue->queueaction.obj,
	.destobj = idQueue->queueaction.destobj,
	.param0 = idQueue->queueaction.param0,
	.param1 = idQueue->queueaction.param1,
	.param2 = idQueue->queueaction.param2
    };
    newelem->executeTick = idQueue->executeTick;
    PushElem(newelem);
    return(QUEUEOK);
}
//==========================================
int CommandQueue::QueueMain(long tickNr)
{
    int queuedProceed = 0;
    COMMANDQUEUEELEMENT *tempqueue;
    do{
	if (IsEmpty())
	    return(queuedProceed);
	if (GetNextElem()->executeTick != tickNr)
	    return(queuedProceed);
        tempqueue = PopElem();
	(*EndQueueCallBackFunction)(tempqueue);
	queuedProceed++;
    }while(1);
}
//==========================================
void CommandQueueAction(COMMANDQUEUEELEMENT *queueelem)
{
    printf("Perform action:\n obj:%d destobj:%d x,y=%d,%d,mode=%d\n",
	    ((OBJ *)queueelem->queueaction.obj)->SC_Unit,
	    ((OBJ *)queueelem->queueaction.destobj)->SC_Unit,
	    queueelem->queueaction.param0,
	    queueelem->queueaction.param1,
	    queueelem->queueaction.actiontype);
}
