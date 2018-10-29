
#include <stdlib.h>
#include <string.h>

#include "action.h"
#include "wmem.h"
#include "queue.h"

//==========================================
Queue::Queue(void)
{
	EndQueueCallBackFunction = NULL;
	maxElements = 0;
	currentTick=0;
	queuedElements=0;
}
//==========================================
Queue::Queue(void (*func)(void *,int ),int maxelements)
{
	EndQueueCallBackFunction = func;
	allElements = (struct QUEUEELEMENT *) wmalloc(maxelements * sizeof(struct QUEUEELEMENT ));
	memset(allElements,0,maxelements * sizeof(struct QUEUEELEMENT ));
	maxElements = maxelements;
	currentTick=0;
	queuedElements=0;
}
//==========================================
Queue::~Queue(void)
{
	EmptyQueue();
	wfree(allElements);
	EndQueueCallBackFunction = NULL;
	allElements = NULL;
	maxElements = 0;
}
//==========================================
void Queue::EmptyQueue(void)
{
	DestroyAllElements();
	queuedElements=0;
}
//==========================================
int Queue::AddToQueue(void *IDqueue,int waitgameticks)
{
	int whereToInsert,needtocopy;
	int executetime;
	if (queuedElements >= maxElements)	//queue is full
		return QUEUEFULL;
	executetime = currentTick + waitgameticks;
	//need to find where I need to insert new element sorted by time
	whereToInsert = InsertInQueue(executetime);
	//need to do shift all memory after insertion 
	needtocopy = (queuedElements - whereToInsert) * sizeof(struct QUEUEELEMENT);
	//if last element no need to make shift
	if (needtocopy)
		memmove(&allElements[whereToInsert+1],&allElements[whereToInsert],needtocopy);
	allElements[whereToInsert].ID = IDqueue;
	allElements[whereToInsert].executeTick = executetime;
	queuedElements++;
	return(whereToInsert);
}
//==========================================
int Queue::QueueMain(int addticks)
{
	struct QUEUEELEMENT *tempqueue;
	int factor;
	int i,j;
	currentTick += addticks;
	if (queuedElements)
	{
		factor = allElements[0].executeTick - currentTick;
		if (factor <= 0)
		{
			for (i=1;i<queuedElements;i++)
				if (currentTick < allElements[i].executeTick)
					break;
//			ShowAllQueue();
			//now i = number of queue elements to proceed
			tempqueue = (struct QUEUEELEMENT  *) wmalloc(i * sizeof(struct QUEUEELEMENT));
			memmove(tempqueue,&allElements[0],i * sizeof(struct QUEUEELEMENT));
			DelQueue(0,i);
			for (j=0;j<i;j++)
			{
				(*EndQueueCallBackFunction)(tempqueue[j].ID,NORMALPASSQUEUE);
				tempqueue[j].ID=NULL;
			}
			wfree(tempqueue);
//			ShowAllQueue();
			return 1;
		}
	}
	return 0;
}
//==========================================
void Queue::DestroyAllElements(void)
{
	int i;
	for (i=0;i<queuedElements;i++)
	{
		(*EndQueueCallBackFunction)(allElements[i].ID,DESTROYQUEUE);
	}
}
//==========================================
void Queue::DelQueue(int from,int count)
{
	if (from+count > queuedElements)
		count = queuedElements - from;
	if (count)
	{
		queuedElements-=count;
		if (queuedElements-from>0)
			memmove(&allElements[from],&allElements[from+count],
					(queuedElements-from)*sizeof(struct QUEUEELEMENT));
	}
}
//==========================================
int Queue::InsertInQueue(int absolutegameticks)
{
	struct QUEUEELEMENT *elem;
	int leftpos,rightpos,pos;
	if (!queuedElements)
		return 0;
	leftpos	 = 0;
	rightpos = queuedElements;
	do{
		pos = (rightpos + leftpos)/2;
		elem = &allElements[pos];
		if (pos == leftpos)
			break;
		if (elem->executeTick < absolutegameticks)
			leftpos = pos;
		else
			rightpos = pos;
	}while(1);
	if (elem->executeTick < absolutegameticks)
		return (pos+1);
	else
		return 0;
}
//==========================================
void Queue::ShowAllQueue(void)
{
	QUEUEACTION *act;
	int i;
	for (i=0;i<queuedElements;i++)
	{
		act = (QUEUEACTION *) allElements[i].ID;
		printf("%d:	 type=%d\n",i,act->actiontype);
	}
	printf("\n");
}
//==========================================
