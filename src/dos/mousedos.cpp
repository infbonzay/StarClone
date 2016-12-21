#include "mousedos.h"

int WMouseMoveRelativX,WMouseMoveRelativY;
char mousedetect;
//====================================
void SetMousePos(int x,int y)
{
}
//====================================
void GetMouseMove(void)
{
     _asm
     {
       mov eax,0x0b
       int 0x33
       movsx edx,dx
       movsx ecx,cx
       mov WMouseMoveRelativX,ecx
       mov WMouseMoveRelativY,edx
     }
}
//====================================
int mousebuttons(void)
{
     int buttons=0;
     _asm
     {
       mov eax,0x3
       int 0x33
       mov buttons,ebx
     }
     return (buttons & 0xff);
}
//====================================
int mouseinit(void)
{
  int a;
_asm{
      mov ax,0h
      int 0x33
      and eax,0xffff
      mov a,eax
      mov mousedetect,al
}
return(a);
}
//==========================
