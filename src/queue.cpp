
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
	currentTick = 0;
	queuedElements = 0;
	FirstEmptyElement = 0;
}
//==========================================
Queue::Queue(void(*func)(void *, int), int maxelements)
{
	EndQueueCallBackFunction = func;
	allElements = (struct QUEUEELEMENT *) wmalloc(maxelements * sizeof(struct QUEUEELEMENT));
	memset(allElements, 0, maxelements * sizeof(struct QUEUEELEMENT));
	maxElements = maxelements;
	currentTick = 0;
	queuedElements = 0;
	FirstEmptyElement = 0;
}
//==========================================
Queue::~Queue(void)
{
	EmptyQueue();
	wfree(allElements);
	EndQueueCallBackFunction = NULL;
	allElements = NULL;
	maxElements = 0;
	FirstEmptyElement = 0;
}
//==========================================
void Queue::EmptyQueue(void)
{
	DestroyAllElements();
	queuedElements = 0;
}
//==========================================
int Queue::AddToQueue(void *IDqueue, int waitgameticks)
{
	if (queuedElements >= maxElements)	//queue is full
	    return QUEUEFULL;
	long executetime = currentTick + waitgameticks;
	for (int i = FirstEmptyElement ; i < maxElements; i++)
	{
	    if (allElements[i].ID == NULL)
	    {
		allElements[i].ID = IDqueue;
		allElements[i].executeTick = executetime;
		queuedElements++;
		if (++FirstEmptyElement >= maxElements)
		    FirstEmptyElement = 0;
		return QUEUEOK;
	    }
	}
	for (int i = 0 ; i < FirstEmptyElement - 1; i++)
	{
	    if (allElements[i].ID == NULL)
	    {
		allElements[i].ID = IDqueue;
		allElements[i].executeTick = executetime;
		queuedElements++;
		if (++FirstEmptyElement >= maxElements)
		    FirstEmptyElement = 0;
		return QUEUEOK;
	    }
	}
	return QUEUEFULL;
}
//==========================================
int Queue::QueueMain(int addticks)
{
	int j = 0;
	currentTick += addticks;
	int startQueuedElements = queuedElements;
	for (int i = 0; j < startQueuedElements; i++)
	{
	    if (allElements[i].ID != NULL)
    	    {
	        if (allElements[i].executeTick - currentTick <= 0 )
	        {
		    (*EndQueueCallBackFunction)(allElements[i].ID, NORMALPASSQUEUE);
		    DelQueue(i);
		    j++;
		}
	    }
	}
	//ShowAllQueue();
	return 0;
}
//==========================================
void Queue::DelQueue(int elemnr)
{
    if (allElements[elemnr].ID != NULL)
    {
        allElements[elemnr].ID = NULL;
	queuedElements--;
	FirstEmptyElement = elemnr;
    }
}
//==========================================
void Queue::DestroyAllElements(void)
{
	int i;
	for (i = 0;i < maxElements;i++)
	{
	    if (allElements[i].ID != NULL)
	    {
    		(*EndQueueCallBackFunction)(allElements[i].ID, DESTROYQUEUE);
		allElements[i].ID = NULL;
		queuedElements--;
	    }
	}
	FirstEmptyElement = 0;
}
//==========================================
void Queue::ShowAllQueue(void)
{
	QUEUEACTION *act;
	int i;
	for (i = 0;i < maxElements;i++)
	{
	    if (allElements[i].ID != NULL)
	    {
		act = (QUEUEACTION *)allElements[i].ID;
		printf("%d:	 type=%d\n", i, act->actiontype);
	    }
	}
	printf("\n");
}
//==========================================
