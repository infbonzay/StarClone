
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
    EmptyElementNr = 0;
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
    EmptyElementNr = 0;
}
//==========================================
Queue::~Queue(void)
{
    EmptyQueue();
    wfree(allElements);
    EndQueueCallBackFunction = NULL;
    allElements = NULL;
    maxElements = 0;
    EmptyElementNr = 0;
}
//==========================================
void Queue::EmptyQueue(void)
{
    DestroyAllElements();
    queuedElements = 0;
    EmptyElementNr = 0;
}
//==========================================
int Queue::AddToQueue(void *IDqueue, int waitgameticks)
{
    int i;
    long executetime;
    if (queuedElements >= maxElements)    //queue is full
    {
        return QUEUEFULL;
    }
    i = EmptyElementNr;
    executetime = currentTick + (long) waitgameticks;
    while( i < maxElements )
    {
        if (allElements[i].ID == NULL)
        {
            allElements[i].ID = IDqueue;
            allElements[i].executeTick = executetime;
            queuedElements++;
            EmptyElementNr = i + 1;
            if (EmptyElementNr >= maxElements)
            {
                EmptyElementNr = 0;
            }    
            return(i);
        }
        i++;
    }
    i = 0;
    while( i < EmptyElementNr )
    {
        if (allElements[i].ID == NULL)
        {
            allElements[i].ID = IDqueue;
            allElements[i].executeTick = executetime;
            queuedElements++;
            EmptyElementNr = i + 1;
            return(i);
        }
        i++;
    }
    return 0;
}
//==========================================
int Queue::QueueMain(int addticks)
{
    int i;//TODO parce only queuedElements
    currentTick += (long) addticks;
    if (queuedElements)
    {
        for (i = 0 ; i < maxElements; i++)
        {
            if (allElements[i].ID != NULL)
            {
                if (currentTick - allElements[i].executeTick >= 0)
                {
                    (*EndQueueCallBackFunction)(allElements[i].ID, NORMALPASSQUEUE);
                    DelQueue(i);
                    allElements[i].ID = NULL;
                }
            }
        }
    }
    return QUEUEOK;
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
        }
    }
}
//==========================================
void Queue::DelQueue(int elementNr)
{
    if (allElements[elementNr].ID != NULL)
    {
        allElements[elementNr].ID = NULL;
        queuedElements--;
        if (elementNr < EmptyElementNr)
        {
            EmptyElementNr = elementNr;
        }    
    }
}
//==========================================
void Queue::ShowAllQueue(void)
{
    QUEUEACTION *act;
    int i;
    for (i = 0;i < queuedElements;i++)
    {
        act = (QUEUEACTION *)allElements[i].ID;
        printf("%d:     type=%d\n", i, act->actiontype);
    }
    printf("\n");
}
//==========================================
