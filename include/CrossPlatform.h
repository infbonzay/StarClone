#ifndef _SC_CROSSPLATFORM

#define _SC_CROSSPLATFORM

#if defined (_MSC_VER)

#include <direct.h>
#pragma warning( disable : 4200 )								//disable 0 size array ex.    int myarray[];

#include <windows.h>
#include <winsock.h>
#include <string.h>
#include <io.h>
//#include <ws2tcpip.h>

#define usleep(a) Sleep(((DWORD) a)/1000)
#define mkdir(a,b) _mkdir(a)
#define chdir _chdir
#define close _close
#define itoa(value, buffer, radix) _itoa_s(value, buffer, sizeof(buffer) - 1, radix)
#define strcat(dest, source) strcat_s(dest, sizeof(dest)-1, source)
#define strcpy(dest, source) strcpy_s(dest, sizeof(dest)-1, source)
#define strncat(dest, source, size) strncat_s(dest, sizeof(dest)-1, source, size)
#define strncpy(dest, source, size) strncpy_s(dest, sizeof(dest)-1, source, size)

#define stricmp _stricmp
#define strlwr(str) _strlwr_s(str, sizeof(str) - 1)
#define strupr(str) _strupr_s(str, sizeof(str) - 1)
#define rmdir _rmdir
#define access _access
#define getcwd _getcwd
#define readlink(a,b,c) (strncpy(b,a,c) != NULL)
#define strncasecmp _strnicmp
#define SIGKILL PROCESS_TERMINATE
#define R_OK 0
#define strcasestr stricmp
#define inet_aton(a,b) 0

inline bool kill(int dwProcessId, int uExitCode)
{
	DWORD dwDesiredAccess = uExitCode;
	BOOL  bInheritHandle = FALSE;
	HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
	if (hProcess == NULL)
		return FALSE;
	BOOL result = TerminateProcess(hProcess, -1);
	CloseHandle(hProcess);
	return TRUE;
}

inline char *strchrnul(char *text, char symb) 
{
	char *poschar = strchr(text,symb);
	if (poschar)
		return poschar;
	else 
		return text + strlen(text);
}

#endif

#ifdef UNDERLINUX

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <ifaddrs.h>
#include <net/if.h>

void strupr(char *str);
void strlwr(char *str);

#endif

#endif
