#if !defined(_WMEM_W)
#define _WMEM_W

//#define USEWMALLOC
//====================
#ifndef USEWMALLOC

    #define wmalloc(x) malloc(x)
    #define wfree(x)   free(x)

#else
//    #define TESTMALLOC
    void *wmalloc(int memsize);
    void wfree(void *buf);
    int  checkleak(void *buf);
#endif
//=============================
#endif

