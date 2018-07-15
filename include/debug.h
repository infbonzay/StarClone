#if !defined(_DEBUG_W)
#define _DEBUG_W
//===========================================
#define DEBUGOUTPUT
#define DEBUGFILE	"/output.log"

void DEBUG_INIT(int flags);
void dprintf(const int prefixlinenr,const char *prefixfunc,const char *format, ...);
void dfprintf(const int prefixlinenr,const char *prefixfunc,const char *format, ...);

#ifdef DEBUGOUTPUT
    #define DEBUGMES(...) dfprintf(__LINE__,__FUNCTION__,__VA_ARGS__)
    #define DEBUGMESSCR(...) dprintf(__LINE__,__FUNCTION__,__VA_ARGS__)
#else
    #define DEBUGMES(...) 
    #define DEBUGMESSCR(...) 
#endif
//===========================================
#endif
