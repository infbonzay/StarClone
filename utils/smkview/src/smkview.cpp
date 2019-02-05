#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "SDL/SDL.h"
#include "smacker.h"

#include <grplib/gr8.h>
#include <grplib/grp.h>
#include <grplib/usegrp.h>
#include <grplib/palette.h>


/*max gradation for red,green,blue,shadows,white*/
#define MAXGRD 32
#define MAXPLAYERCOLOR  15
#define MONOPLAYERCOLOR (MAXPLAYERCOLOR+1)

#define ENDPLAYERCOLOR  MONOPLAYERCOLOR

#define	XW	640
#define	YW	480

int DELTAX,DELTAY;

char *_tored_,*_togreen_,*_toblue_,*_toblack_,*_towhite_,*_tomono_;

char palette[256*3];

char dependencycolor[256];

SDL_Surface *sdlsurface;

char keyactive,lastkey;

int active;
/*====================================
//set palette now*/
void activatepallette(char *pal)
{
    SDL_Color rememberpal[256];
    int i,err;
    for(i=0;i<256;i++)
    {
       rememberpal[i].r=pal[i*3+0];
       rememberpal[i].g=pal[i*3+1];
       rememberpal[i].b=pal[i*3+2];
    }
    err = SDL_SetPalette(sdlsurface,SDL_LOGPAL|SDL_PHYSPAL,rememberpal,0,256);
}
/*========================================*/
int LoadingAllPalettes(void)
{
    int i,j;
    FILE *f = fopen("pal/palette.dat","rb");	
    if (!f)
	printf("file pal/palette.dat not found\n");
    else
    {
	fread(palette,3,256,f);
	fclose(f);
    }
    char *transp = (char *)malloc(256*256);
    f = fopen("pal/transptable.dat","rb");
    if (!f)
    {
	printf("file pal/transptable.dat not found\nCreating...");
	fflush(stdout);
	GRP_CreateTRColors(palette,transp);
	printf("\ndone. now saveing to file pal/transptable.dat\n");
	f = fopen("pal/transptable.dat","wb");
	fwrite(transp,256,256,f);
    }
    else
	fread(transp,256,256,f);
    fclose(f);
    GRP_SetTranspTable(transp);
    f = fopen("pal/usercolor.dat","rb");
    if (!f)
	printf("file pal/usercolor.dat not found\n");
    else
    {
	fread(dependencycolor,1,256,f);
	fclose(f);
	GRP_SetUserPlayerColors(0,16,dependencycolor);
    }
    _toblack_ = (char *)malloc(MAXGRD*256);
    f = fopen("pal/toblack.grd","rb");
    if (!f)
    {
	printf("file pal/toblack.grd not found\nCreating...");
	fflush(stdout);
	GRP_CreateByRGBTable(palette,MAKERGB(0,0,0),MAKERGB(0,0,0), MAXGRD,_toblack_);
	printf("\ndone. now saveing to file pal/toblack.grd\n");
	f = fopen("pal/toblack.grd","wb");
	fwrite(_toblack_,MAXGRD,256,f);
    }
    else
	fread(_toblack_,MAXGRD,256,f);
    fclose(f);
    
    
    _towhite_ = (char *)malloc(MAXGRD*256);
    f = fopen("pal/towhite.grd","rb");
    if (!f)
    {
	printf("file pal/towhite.grd not found\nCreating...");
	fflush(stdout);
	GRP_CreateByRGBTable(palette,MAKERGB(240,240,240),MAKERGB(240,240,240),
		MAXGRD,_towhite_);
	printf("\ndone. now saveing to file pal/towhite.grd\n");
	f = fopen("pal/towhite.grd","wb");
	fwrite(_towhite_,MAXGRD,256,f);
    }
    else
	fread(_towhite_,MAXGRD,256,f);
    fclose(f);

    _tored_ = (char *)malloc(MAXGRD*256);
    f = fopen("pal/tored.grd","rb");	
    if (!f)
    {
	printf("file pal/tored.grd not found\nCreating...");
	fflush(stdout);
	GRP_CreateByRGBTable(palette,MAKERGB(120,0,0),MAKERGB(244,228,144),
		MAXGRD,_tored_);
	printf("\ndone. now saveing to file pal/tored.grd\n");
	f = fopen("pal/tored.grd","wb");
	fwrite(_tored_,MAXGRD,256,f);
    }
    else
	fread(_tored_,MAXGRD,256,f);
    fclose(f);
    
    _togreen_ = (char *)malloc(MAXGRD*256);
    f = fopen("pal/togreen.grd","rb");	
    if (!f)
    {
	printf("file pal/togreen.grd not found\nCreating...");
	fflush(stdout);
	GRP_CreateByRGBTable(palette,MAKERGB(0,140,0),MAKERGB(252,252,56),
		MAXGRD,_togreen_);
	printf("\ndone. now saveing to file pal/togreen.grd\n");
	f = fopen("pal/togreen.grd","wb");
	fwrite(_togreen_,MAXGRD,256,f);
    }
    else
	fread(_togreen_,MAXGRD,256,f);
    fclose(f);
    
    _toblue_ = (char *)malloc(MAXGRD*256);
    f = fopen("pal/toblue.grd","rb");	
    if (!f)
    {
	printf("file pal/toblue.grd not found\nCreating...");
	fflush(stdout);
	GRP_CreateByRGBTable(palette,MAKERGB(0,60,150),MAKERGB(204,248,248),
		MAXGRD,_toblue_);
	printf("\ndone. now saveing to file pal/toblue.grd\n");
	f = fopen("pal/toblue.grd","wb");
	fwrite(_toblue_,MAXGRD,256,f);
    }
    else
	fread(_toblue_,MAXGRD,256,f);
    fclose(f);

    _tomono_ = (char *)malloc(256);
    
    f = fopen("pal/tomono.grd","rb");	
    if (!f)
    {
	printf("file pal/tomono.grd not found\nCreating...");
	fflush(stdout);
	GRP_CreateMono(palette,_tomono_);
	printf("\ndone. now saveing to file pal/tomono.grd\n");
	f = fopen("pal/tomono.grd","wb");
	fwrite(_tomono_,1,256,f);
    }
    else
	fread(_tomono_,1,256,f);
    GRP_SetPlayerColors(MONOPLAYERCOLOR,1,0,256,_tomono_);

    fclose(f);
    return 0;
}
/*====================================*/
int eventwindowloop(void)
{
    int exitevent = 0;
    SDL_Event event;
    while ( SDL_PollEvent(&event))
    {
        switch (event.type)
        {
	    case SDL_KEYDOWN:
		keyactive = event.key.keysym.sym;	
		lastkey = keyactive;
		break;
	    case SDL_KEYUP:
		keyactive = 0;
		break;
            case SDL_ACTIVEEVENT:			
                if ( event.active.state & SDL_APPACTIVE )
                {
                    if ( event.active.gain )
                        active = 1;
                    else
                        active = 0;
                }
                break;
            case SDL_QUIT:				
                exitevent = 1;
                break;
        }
    }
    return (exitevent);
}
/*===========================================*/
void putrow(int x,int y,int sizex,int sizey,unsigned char *buff)
{
    int i,j;
    unsigned char pixel;
    char *vidbuf;
    vidbuf=GRP_vidmem+GRP_scanlineoffsets[y]+x;
    for (i=0;i<sizey;i++)
    {
/*	memcpy(vidbuf,buff,sizex);
//	buff+=sizex;
//	vidbuf+=wmaximx;
*/
	for (j=0;j<sizex;j++)
	{
	    pixel=*buff++;
	    if (pixel)
	    {
		*vidbuf++=pixel;
	    }
	    else
		vidbuf++;
	}
/*	buff+=sizex-sizex;*/
	vidbuf+=GRP_screensizex-sizex;
    }
}
/*==========================*/
unsigned char *smk_picture;
struct smk_t *mysmk;
/*==========================*/
void smk_show(struct smk_t *tempsmk)
{
    static int pos;
    unsigned int w,h;
    unsigned char scale;
    if (tempsmk)
    {
	smk_info_video(tempsmk,&w,&h,&scale);
	putrow(0,0,w,h,smk_picture);
	pos++;
	if (!(pos%100))
	{
	    if (!smk_next(tempsmk,SMK_FULL_DECODE))
		smk_first(tempsmk,SMK_FULL_DECODE);
	    smk_picture=smk_get_video(mysmk);
	}
    }
}
/*==========================*/
int main(int count,char *argv[])
{
    int i,j;
    if (count != 2)
    {
	printf("usage:%s filename.smk\n",argv[0]);
	return -1;
    }
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)==-1)
    {
	printf("error initialize sdl\n");
	return -3;
    }
    sdlsurface = SDL_SetVideoMode(XW,YW,8,SDL_SWSURFACE|SDL_HWPALETTE);
    if (!sdlsurface)
    {
	printf("error to set graph mode\n");
	return -4;
    }
    if (GRP_InitLib(XW,YW))
    {
	printf("error initialize grplib\n");
	return -1;
    }
    mysmk=smk_open_file(NULL,argv[1],SMK_MODE_MEMORY);
    smk_enable_video(mysmk,1);
    smk_first(mysmk,SMK_FULL_DECODE);
    smk_picture=smk_get_video(mysmk);

    if (LoadingAllPalettes())
    {
	printf("error to load palettes files\n");
    }
    GRP_SetVideoBuffer(sdlsurface->pixels);
/*    activatepallette(palette);*/
    activatepallette((char *)smk_get_palette(mysmk));

    int exitflag=0,colors=0,gradation=0;
    active = 1;
    printf("press \n1/2- to change color of unit\n3/4 - next/prev texture grp\n"
	   "9/0 - +/- gradation intencity(0..%d)\n" 
	   "ESC to exit\n",MAXGRD-1);

    unsigned int frames,totalframes;
    smk_info_all(mysmk,&frames,&totalframes,NULL);
    printf("%d/%d\n",frames,totalframes);

    char *background=(char *)malloc(XW*YW);
    
    for (i=0;i<YW;i++)
        for (j=0;j<XW;j++)
	    background[i*XW+j]=(char)j;

    int nrsprite = 0,unitcolor=0,grdfactor=MAXGRD/2;
    

    do{
	exitflag = eventwindowloop();
	if (exitflag)
	    break;
	if (keyactive=='1')	
	    unitcolor --;	
	if (keyactive=='2')
	    unitcolor ++;	
	if (unitcolor>ENDPLAYERCOLOR)
	    unitcolor=ENDPLAYERCOLOR;
	if (unitcolor<0)
	    unitcolor=0;
	if (keyactive=='3')	
	    nrsprite--;		
	if (keyactive=='4')	
	    nrsprite++;		
	if (nrsprite<0)
	    nrsprite = 0;
	if (keyactive=='0')	
	    grdfactor++;	
	if (keyactive=='9')
	    grdfactor--;	
	if (keyactive=='5'){
	    DELTAX++;
	}
	if (keyactive=='6'){
	    DELTAX--;
	}
	if (keyactive=='7'){
	    DELTAY++;
	}
	if (keyactive=='8'){
	    DELTAY--;
	}
	if (grdfactor<0)	
	    grdfactor=0;
	if (grdfactor>MAXGRD-1)
	    grdfactor=MAXGRD-1;
	if (active)	
	{
    	    if (SDL_MUSTLOCK(sdlsurface))
        	SDL_LockSurface(sdlsurface);

	    GRP_SetVideoBuffer(sdlsurface->pixels);

	    memmove((char*)sdlsurface->pixels,background,XW*YW);
	    smk_show(mysmk);

	    if (SDL_MUSTLOCK(sdlsurface))
    		SDL_UnlockSurface(sdlsurface);
	    SDL_UpdateRect(sdlsurface, 0, 0, 0, 0);
	  }
	keyactive = 0;
    }while(lastkey!=SDLK_ESCAPE);  
    smk_close(mysmk);

    free(background);		 
    SDL_Quit();
    return 0;
}


