
#ifdef WIN32 
#include <windows.h>
#include "pthreadPort.h"

//==============================================================
int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
	return !CloseHandle(mutex->handle);
}
//==============================================================
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
	HANDLE handle = CreateMutex(NULL,FALSE,NULL);
	if(handle != NULL)
	{
		mutex->handle = handle;
		return 0;
	}
	else
	{
		return 1;
	}
}
//==============================================================
int pthread_mutex_lock(pthread_mutex_t *mutex)
{
	DWORD retvalue = WaitForSingleObject(mutex->handle,INFINITE);
	if(retvalue == WAIT_OBJECT_0)
	{
		return 0;
	}
	else
	{
		return EINVAL;
	}
}
//==============================================================
int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	return !ReleaseMutex(mutex->handle);
}
//==============================================================
#endif

