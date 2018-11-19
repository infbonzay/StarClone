#ifndef _CROSSPLATFORM_W

#if defined (_MSC_VER)
	#pragma warning( disable : 4200 )								//disable 0 size array ex.    int myarray[];
	#define usleep(waitms) Sleep(waitms)
#endif
#ifdef UNDERLINUX
	#include <arpa/inet.h>
	#include <sys/socket.h>
	void strupr(char *str);
	void strlwr(char *str);
#endif

#endif
