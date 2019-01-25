#ifndef _SC_DEBUG_H

#define _SC_DEBUG_H
//===========================================
#define DEBUGOUTPUT
#define DEBUGFILE		"/output.log"

void DEBUG_INIT(int flags);
void dprintf(const char *prefixfile,const int prefixlinenr,const char *prefixfunc,const char *format, ...);
void dfprintf(const char *prefixfile,const int prefixlinenr,const char *prefixfunc,const char *format, ...);

#ifdef DEBUGOUTPUT
	#define DEBUGMES(...) dfprintf(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)
	#define DEBUGMESSCR(...) dprintf(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)
#else
	#define DEBUGMES(...) 
	#define DEBUGMESSCR(...) 
#endif
//===========================================
#endif
