#include <string.h>

#include "modes.h"
#include "../wmem.h"

#include <grplib/gr8.h>
#include <grplib/grp.h>

#include "gr8dos.h"
#include "doscall.h"

unsigned char DACswitch;
int TotalVMem;
void *GRP_vidmem_;
//==========================
#define RP2PP(x) (((x&0xffff0000)>>12)+(x&0xffff))
void settextmode(void)
{
   _asm{
	 mov eax,3
	 int 0x10
   }
   if (GRP_vidmem_)
      wfree(GRP_vidmem_);
}
//==========================
int detectmode(int x,int y,int bpp,int fullscreen)
{
    int retmodes=0;
    void *dosmem1,*dosmem2;
    dosmem1=alloc_low_memory(256);
    struct SvgaInfoRec *adrsvgainfo;
    struct MODES *adrmodeinfo;
    unsigned short *adrmodes;
    rminfo rm;
    memset(&rm,0,sizeof(rm));
    rm.eax=0x4f00;
    rm.es=(int)dosmem1>>4;
    rm.edi=0;
    RM_intr(0x10,&rm);
    if ((rm.eax&0xff)!=0x004f)
    {
       free_low_memory(dosmem1);
       return(-1);
    }
    adrsvgainfo=(struct SvgaInfoRec *)(rm.es<<4+rm.edi);
    DACswitch=adrsvgainfo->Abilities[0]&1;
    adrmodes=(unsigned short *)RP2PP(adrsvgainfo->pfawModes);
    dosmem2=alloc_low_memory(256);
    while (*adrmodes!=0xffff)
    {
	rm.eax=0x4f01;
	rm.ecx=(int)*adrmodes;
	rm.es=(int)dosmem2>>4;
	rm.edi=0;
	RM_intr(0x10,&rm);
	adrmodes++;
	if ((rm.eax&0xff)!=0x004f)
	{
	   break;
	}
	adrmodeinfo=(struct MODES *)(rm.es<<4+rm.edi);
	if ((adrmodeinfo->ModeAttributes&2)==2&&
	     adrmodeinfo->XResolution==x&&
	     adrmodeinfo->YResolution==y&&
	     adrmodeinfo->BitsPerPixel==bpp)
	    {
	       retmodes=rm.ecx;
	       wgranularity=adrmodeinfo->WinGranularity*256;
	       GRP_wmaxbank=(GRP_screensizex*GRP_screensizey*bpp/8+wgranularity*4-1)/(wgranularity*4);
	       GRP_wmaxbankl=(GRP_screensizex*GRP_screensizey*bpp/8+wgranularity*4-1)/(wgranularity*4);
	       GRP_wmaxdwordwritel=(GRP_wmaxbankl+1)*wgranularity;
	       DPMIReg_CS=adrmodeinfo->WinFuncPtr>>16;	   //adres of bios
	       DPMIReg_IP=adrmodeinfo->WinFuncPtr&0xffff;  //change videobankmemory
	       TotalVMem=adrsvgainfo->TotalMem;
	       break;
	    }
    }
    free_low_memory(dosmem2);
    free_low_memory(dosmem1);
    if (!retmodes)
    {
      return(-2);
    }
    _asm{
	  mov eax,0x4f02
	  mov ebx,retmodes
	  int 10h
    };
    return(0);
}
//==========================
void SetPitchAndChunk(void)
{

int k=0;
int chk=0;
int lncnt=0;
int lasty=0;
GRP_wmaxdworddesen[0]=0;
int newchk=0;
long int totalpix=0;
int j=0;
	for( int i=0; i< GRP_screensizey; i++,j++ ){
		lasty = k;
		k += GRP_screensizex;
		if( k > wgranularity*4-1 ){
			wendlineaddfactor[j] = 0;
			GRP_wmaxdworddesen[j] = (GRP_screensizex - (k - wgranularity*4))/4;
			wlinecount[chk] = lncnt+1;
			k -= wgranularity*4;
			if( k ){
				j++;
				GRP_wmaxdworddesen[j] = k/4;
			}
			wendlineaddfactor[j]=0;
			lncnt=0;
			chk++;
			newchk=1;
		}
		else{
			GRP_wmaxdworddesen[j] = GRP_screensizex/4;
			wendlineaddfactor[j] =0;
		}
		lncnt++;
	}
	if( lncnt ){
		wlinecount[chk] = lncnt;
	}
}
//==========================
void activatepalette(char *pal)
{
   if (!DACswitch)
   {
     for (int i=0;i<3*256;i++)
	 pal[i]/=4;
   }
   else
   {
     _asm{
	   mov eax,0x4f08
	   mov ebx,0x0800
	   int 10h
     }
   }
   wwritepalette(pal);
}
//==========================
void wconvertpal(int number,int count,struct RGBPERSENT *rgbpers,int bitmask)
{
char *j=(char*)wmalloc(256*3);
char *jj;
int color;
jj=j;
wreadpalette(jj);
jj+=number*3;
do{
    color=0;
    color+=(*jj)     *(rgbpers->red);
    color+=(*(jj+1)) *(rgbpers->green);
    color+=(*(jj+2)) *(rgbpers->blue);
    color=color/100;
    *jj    =0;
    *(jj+1)=0;
    *(jj+2)=0;
    if ((bitmask &4)==4)
	  *jj=(char)color;
    if ((bitmask &2)==2)
	  *(jj+1)=(char)color;
    if ((bitmask &1)==1)
	  *(jj+2)=(char)color;
    jj+=3;
}while(--count);
wvertint();
wwritepalette(j);
wfree(j);
}
//==========================
int eventwindowloop(void)
{
    return 0;
}
//==========================
void *getGRP_vidmem(void)
{
     GRP_vidmem_=wmalloc(GRP_screensizex*GRP_screensizey);
     return (GRP_vidmem_);
}


