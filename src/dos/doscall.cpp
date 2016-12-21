#include <dos.h>
#include <string.h>
#include "doscall.h"
//================================================
void RM_intr(int intr,rminfo *rm)
{
  union REGS regs;
  struct SREGS sregs;
  memset(&sregs,0,sizeof(sregs));
  regs.w.ax=0x0300;
  regs.w.bx=(short)intr; // DP
  regs.w.cx=0;
  sregs.es=FP_SEG(rm);
  regs.x.edi=FP_OFF(rm);
  int386x(0x31,&regs,&regs,&sregs);
}
//================================================
void *alloc_low_memory(long size)
{
  rminfo rm;
  memset(&rm,0,sizeof(rm));
  rm.eax=0x4800;
  rm.ebx=(size+15)/16;
  RM_intr(0x21,&rm);
  if (rm.flags&1)
    return NULL;
  else return (void *)((rm.eax&0xffff)<<4);
}
//================================================
void free_low_memory(void *ptr)
{
  rminfo rm;
  memset(&rm,0,sizeof(rm));
  rm.eax=0x4900;
  rm.es=(short)(((unsigned long)ptr)>>4); // DP
  RM_intr(0x21,&rm);
}
//================================================
long low_memory_available(void)
{
  rminfo rm;
  memset(&rm,0,sizeof(rm));
  rm.eax=0x4800;
  rm.ebx=0xffff;        // try to allocate the maximum size,
  RM_intr(0x21,&rm);    // dos while tells us how much is possible
  if (rm.flags&1)
    return rm.ebx*16;
  else                  // this shouldn't succed, but just in case...
  {
    free_low_memory((void *)((rm.eax&0xffff)<<4));
    return 0xffff*16;
  }
}
//================================================

