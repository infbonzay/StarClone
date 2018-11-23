#ifndef _CROSSPLATFORM_W

#define _CROSSPLATFORM_W
#if defined (_MSC_VER)

#include <direct.h>
#pragma warning( disable : 4200 )								//disable 0 size array ex.    int myarray[];

#include <windows.h>
#include <winsock.h>
#include <string.h>
#include <io.h>

#define usleep Sleep
#define mkdir(a,b) _mkdir(a)
#define chdir _chdir
#define rmdir _rmdir
#define access _access
#define getcwd _getcwd
#define readlink(a,b,c) -1
#define strncasecmp _strnicmp
#define SIGKILL PROCESS_TERMINATE
#define R_OK 0
#define strcasestr stricmp
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

void strupr(char *str);
void strlwr(char *str);

#endif

#endif
