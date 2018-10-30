#include <dos.h>
#include <conio.h>
#include "handlers.h"

//==========================================
volatile char keyactive,lastkey;
char     keystatus[256];

static  void interrupt far (*old9h)(void);
static  void interrupt far (*old8h)(void);
static  void interrupt far (*old23h)(void);
static  void interrupt far (*old24h)(void);
//==========================================
static void interrupt far new9h(void)
{
_asm{
       and eax,000000ffh
       in al,60h
       test al,80h
       jnz _xx
       mov byte ptr keyactive,1
       mov byte ptr lastkey,al
       mov byte ptr [eax]+keystatus,KEYPRESSED
       jmp short _ee
_xx:
       and al,7fh
       mov byte ptr keyactive,0
       mov byte ptr [eax]+keystatus,0
_ee:
       mov al,20h
       out 20h,al
  }
}
//==========================================
static void interrupt far new8h(void)
{
  timercomehere();
  _asm{
     mov al,20h
     out 20h,al
  }
}
//==========================================
int installvectors(void)
{
     old8h=_dos_getvect(8);
     _dos_setvect(8,new8h);
     old9h=_dos_getvect(9);
     _dos_setvect(9,new9h);
/*     old23h=_dos_getvect(0x23);
     _dos_setvect(0x23,new23h);
     old24h=_dos_getvect(0x24);
     _dos_setvect(0x24,new24h);*/
     return (0);
}
//==========================================
void uninstallvectors(void)
{
     int *timercounter=(int *)0x46c;//add timer counter
     (*timercounter)+=tick_timer;
     _dos_setvect(8,old8h);
     _dos_setvect(9,old9h);
/*     _dos_setvect(0x23,old23h);
     _dos_setvect(0x24,old24h);*/
}
//==========================================
void keyrefresh(void)
{
        //keystatus refresh
        //no needed
        //this proceed in my keyboard interrupt
}
//==========================


