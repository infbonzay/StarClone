#if !defined(_DEBUG_W)
#define _DEBUG_W
//===========================================
#define DEBUGOUTPUT
#define DEBUGFILE	"/output.log"

void DEBUG_INIT(int flags);
int dprintf(const char *prefix,const char *format, ...);
int dfprintf(const char *prefix,const char *format, ...);

#ifdef DEBUGOUTPUT
    #define DEBUGMES(...) dfprintf(__FUNCTION__,__VA_ARGS__)
    #define DEBUGMESSCR(...) dprintf(__FUNCTION__,__VA_ARGS__)
#else
    #define DEBUGMES(...) 
    #define DEBUGMESSCR(...) 
#endif
//===========================================
#endif
