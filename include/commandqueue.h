#ifndef _SC_COMMANDQUEUE_H

#define _SC_COMMANDQUEUE_H


#include "mycycle.h"
#include "action.h"
#include "queue.h"

#define QUEUENOTOBEEXECUTED		2

struct COMMANDQUEUEELEMENT
{
		QUEUEACTION queueaction;
		long executeTick;
};

class CommandQueue : public mycycle<COMMANDQUEUEELEMENT *>
{
protected:
	long		currentTick;
	void		(*EndQueueCallBackFunction)(COMMANDQUEUEELEMENT *);
	void		DestroyAllElements(void);

public:
		 CommandQueue(void (*callBackFunc)(COMMANDQUEUEELEMENT *),int maxElements);
		~CommandQueue(void);
	void EmptyQueue(void);
	int	 AppendToQueue(COMMANDQUEUEELEMENT *idQueue);
	int	 QueueMain(long ticknr);
};
//==========================================
void CommandQueueAction(COMMANDQUEUEELEMENT *IDQueueAction);

#endif /*_QUEUE_W*/
