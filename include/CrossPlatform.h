#ifndef _CROSSPLATFORM_W

#if defined (_MSC_VER)

#include <direct.h>
#pragma warning( disable : 4200 )								//disable 0 size array ex.    int myarray[];

#define usleep(waitms) Sleep(waitms)
#define mkdir(a,b) _mkdir(a)
#define chdir(a) _chdir(a)
#define rmdir(a) _rmdir(a)
#define getcwd _getcwd
#define readlink(a,b,c) -1
#define strncasecmp(x,y,z) _strnicmp(x,y,z)

#endif

#ifdef UNDERLINUX

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

void strupr(char *str);
void strlwr(char *str);

#endif

#endif
