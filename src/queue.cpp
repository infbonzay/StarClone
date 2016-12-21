
#include <stdlib.h>
#include <string.h>
#include "action.h"
#include "queue.h"

//==========================================
Queue::Queue(void)
{
    EndQueueCallBackFunction = NULL;
    MaxElements = 0;
    PlayTicks=0;
    QueueElements=0;
}
//==========================================
Queue::Queue(void (*func)(void *,int ),int maxelements)
{
    EndQueueCallBackFunction = func;
    Elements = (struct QUEUEELEMENT *)wmalloc(maxelements*sizeof(struct QUEUEELEMENT ));
    memset(Elements,0,maxelements*sizeof(struct QUEUEELEMENT ));
    MaxElements = maxelements;
    PlayTicks=0;
    QueueElements=0;
}
//==========================================
Queue::~Queue(void)
{
    EmptyQueue();
    wfree(Elements);
    EndQueueCallBackFunction = NULL;
    Elements = NULL;
    MaxElements = 0;
}
//==========================================
void Queue::EmptyQueue(void)
{
    DestroyAllElements();
    QueueElements=0;
}
//==========================================
int Queue::AddToQueue(void *IDqueue,int waitgameticks)
{
    int WhereToInsert,needtocopy;
    int executetime;
    if (QueueElements>=MaxElements)	//queue is full
	return QUEUEFULL;
    executetime = PlayTicks + waitgameticks;
    //need to find where I need to insert new element sorted by time
    WhereToInsert = InsertInQueue(executetime);
    //need to do shift all memory after insertion 
    needtocopy = (QueueElements - WhereToInsert)*sizeof(struct QUEUEELEMENT);
    //if last element no need to make shift
    if (needtocopy)
	memmove(&Elements[WhereToInsert+1],&Elements[WhereToInsert],needtocopy);
    Elements[WhereToInsert].ID = IDqueue;
    Elements[WhereToInsert].timetoexecute = executetime;
    QueueElements++;
    return(WhereToInsert);
}
//==========================================
int Queue::QueueMain(int addticks)
{
    struct QUEUEELEMENT *tempqueue;
    int factor;
    int i,j;
    PlayTicks+=addticks;
    if (QueueElements)
    {
	factor = Elements[0].timetoexecute - PlayTicks;
	if (factor<=0)
	{
	    for (i=1;i<QueueElements;i++)
		if (PlayTicks<Elements[i].timetoexecute)
		    break;
	    //now i = number of queue elements to proceed
	    tempqueue = (struct QUEUEELEMENT  *)wmalloc(i*sizeof(struct QUEUEELEMENT));
	    memmove(tempqueue,&Elements[0],i*sizeof(struct QUEUEELEMENT));
	    DelQueue(0,i);
	    for (j=0;j<i;j++)
	    {
		(*EndQueueCallBackFunction)(tempqueue[j].ID,NORMALPASSQUEUE);
		tempqueue[j].ID=NULL;
	    }
	    wfree(tempqueue);
	    return 1;
	}
    }
    return 0;
}
//==========================================
void Queue::DestroyAllElements(void)
{
    int i;
    for (i=0;i<QueueElements;i++)
    {
	(*EndQueueCallBackFunction)(Elements[i].ID,DESTROYQUEUE);
    }
}
//==========================================
void Queue::DelQueue(int from,int count)
{
    if (from+count>QueueElements)
	count = QueueElements - from;
    if (count)
    {
	QueueElements-=count;
	if (QueueElements-from>0)
	    memmove(&Elements[from],&Elements[from+count],
		    (QueueElements-from)*sizeof(struct QUEUEELEMENT));
    }
}
//==========================================
int Queue::InsertInQueue(int absolutegameticks)
{
    struct QUEUEELEMENT *elem;
    int leftpos,rightpos,pos;
    if (!QueueElements)
	return 0;
    leftpos  = 0;
    rightpos = QueueElements;
    do{
	pos = (rightpos + leftpos)/2;
	elem = &Elements[pos];
	if (pos == leftpos)
	    break;
	if (elem->timetoexecute<absolutegameticks)
	    leftpos = pos;
	else
	    rightpos = pos;
    }while(1);
    if (elem->timetoexecute<absolutegameticks)
        return (pos+1);
    else
        return 0;
}
//==========================================
