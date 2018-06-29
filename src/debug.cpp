#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vars.h"
#include "auxil.h"
#include "debug.h"

#define DEBUGPREFIXSTRING	"DEBUG : "
char DEBUGFLAGS;
char DEBUGOUTFILE[1024];
//===========================================
void MAKEDEBUGOUTFILE(void)
{
#ifdef DEBUGOUTPUT
    strcpy(DEBUGOUTFILE,GAMEPATH);
    makefilename(DEBUGOUTFILE,strlen(DEBUGOUTFILE),0,0,DEBUGFILE);
#endif
}
//===========================================
void DEBUG_INIT(int flags)
{
#ifdef DEBUGOUTPUT
    DEBUGFLAGS=flags;
    if (!DEBUGOUTFILE[0])
	MAKEDEBUGOUTFILE();
    unlink(DEBUGOUTFILE);
#endif
}
//===========================================
void dfprintf(const char *prefix,const char *format, ...)
{
#ifdef DEBUGOUTPUT
    va_list args;
    va_start(args, format);

    FILE *f=fopen(DEBUGOUTFILE,"a");
    if (f)
    {
	fprintf(f,DEBUGPREFIXSTRING "%s:",prefix);
	vfprintf(f,format, args);
	fclose(f);
    }
    va_end(args);
#endif
}
//===========================================
void dprintf(const char *prefix,const char *format, ...)
{
#ifdef DEBUGOUTPUT
    va_list args;
    va_start(args, format);

    printf(DEBUGPREFIXSTRING "%s:",prefix);
    vprintf(format, args);
    va_end(args);
#endif
}

