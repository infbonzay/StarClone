#if !defined(_DOSCALL_W)
#define _DOSCALL_W 1

//================================================
typedef struct {
  long edi,esi,ebp,resv,ebx,edx,ecx,eax;
  unsigned short flags, es,ds,fs,gs,ip,cs,sp,ss;
} rminfo;
//================================================
void RM_intr(int intr,rminfo *rm);
void *alloc_low_memory(long size);
void free_low_memory(void *ptr);
long low_memory_available(void);
//================================================
#endif


