
#ifndef __PHREADPORT_H__
#define __PHREADPORT_H__

#ifdef WIN32

	typedef struct pthread_mutex_tag 
	{
		HANDLE handle;
	} pthread_mutex_t;

	typedef struct pthread_mutexattr_tag 
	{
		int attr;
	} pthread_mutexattr_t;

	int pthread_mutex_destroy(pthread_mutex_t *mutex);
	int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
	int pthread_mutex_lock(pthread_mutex_t *mutex);
	int pthread_mutex_unlock(pthread_mutex_t *mutex);

#else
	#include <pthread.h>
#endif

extern pthread_mutex_t Lock_ReadMpqBlocks;

#endif
