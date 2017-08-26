#if !defined(_WMEM_W)
#define _WMEM_W

//#define USEWMALLOC
//====================
#ifndef USEWMALLOC

    #define wmalloc(x) malloc(x)
    #define wfree(x)   free(x)

#else
    #include <string.h>
    
    #define TESTMALLOC
    #define CHECKALLMALLOC

    void *wmalloc(int memsize);
    void wfree(void *buf);
    int  checkleak(void *buf);
    int  checkbuf(void *buf,int incrval,char *mes);
    int  checkallwmalloc(char *mes);
    int checkleakbefore(void *buf);
    int checkleakafter(void *buf);

#endif
//=============================
#endif

