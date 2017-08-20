#if !defined(_WMEM_W)
#define _WMEM_W

#define USEWMALLOC
//====================
#ifndef USEWMALLOC

    #define wmalloc(x) malloc(x)
    #define wfree(x)   free(x)

#else
    #define TESTMALLOC
    #define CHECKALLMALLOC

    void *wmalloc(int memsize);
    void wfree(void *buf);
    int  checkleak(void *buf);
    void checkallwmalloc(void *buf,int incrval);

#endif
//=============================
#endif

