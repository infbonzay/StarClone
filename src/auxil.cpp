#include <string.h>
#include <stdlib.h>

#include <dirent.h>
#include <math.h>
#include <sys/stat.h>

#include "debug.h"
#include "wmem.h"
#include "network_general.h"
#include "stringfiles.h"
#include "objinfo.h"
#include "man.h"
#include "images.h"
#include "auxil.h"

short sintable256[256],costable256[256];
int   inertion256[256][2];
//=================================
int CalcTotalDistance(int topspeed,int minspeed,int acceleration)
{
    int distance = 0;
    while(topspeed >= minspeed)
    {
	distance += topspeed;
	topspeed -= acceleration;
    }
    return(distance);
}
//=================================
char *filename2dos(char *fn)
{
    char *ret;
    int i,j;
    j = strlen(fn);
    ret=(char *)wmalloc(j+1);
    memcpy(ret,fn,j);
    ret[j]=0;
    for (i=0;i<j;i++)
	if (fn[i]=='/')
	    fn[i]='\\';
    return(ret);
}
//=================================
char *filename2unix(char *fn)
{
    char *ret;
    int i,j;
    j = strlen(fn);
    ret=(char *)wmalloc(j+1);
    memcpy(ret,fn,j);
    ret[j]=0;
    for (i=0;i<j;i++)
	if (ret[i]=='\\')
	    ret[i]='/';
    return(ret);
}
//==================================
char *filename2temp(char *fn)
{
    char *ret;
    int i,j,j2;
    j = strlen(fn);
    j2 = strlen(TEMPDIR);
    ret=(char *)wmalloc(j+1+j2);
    memcpy(ret,TEMPDIR,j2);
    memcpy(ret+j2,fn,j);
    ret[j+j2]=0;
    return(ret);
}
//==================================
int zerocmp(void *array,int nrsimb)
{
    int i;
    int sum=0;
    for (i=0;i<nrsimb;i++)
	sum+=((unsigned char *)array)[i];
    return(sum);
}
//==================================
void witoa(int var1,char *adr,int simb)
{
    int i;
    adr[simb]=0;
    for (i=1;i<=simb;i++)
    {
	adr[simb-i]=(var1%10)+'0';
	var1/=10;
	if (!var1) 
	    break;
    }
}
//=============================
char *witoa(int var,char *adr)
{
    int i;
    if (var<10)
	;
    else
    if (var<100)
        adr+=1;
    else
    if (var<1000)
        adr+=2;
    else
    if (var<10000)
        adr+=3;
    else
    if (var<100000)
        adr+=4;
    else
        adr+=5;
    for (i=0;;i++)
    {
	adr[-i]=(var%10)+'0';
	var/=10;
	if (!var) 
	    break;
    }
    adr[i+1]=0;
    return(adr);
}
//=============================
void swap(int *a,int *b)
{
     int c,d;
     c = *a;
     d = *b;
     *a = d;
     *b = c;
}
//==================================
void swapw(short *a,short *b)
{
     short c,d;
     c = *a;
     d = *b;
     *a = d;
     *b = c;
}
//==================================
void setmemd(char *adr,int count,int value)
{
     memset(adr,value,count<<2);
}
//==================================
void CenterXYArea(struct XY *xy,int *xmed,int *ymed)
{
    *xmed=(xy->x2-xy->x1)/2+xy->x1;
    *ymed=(xy->y2-xy->y1)/2+xy->y1;
}
//==================================
float wsqr(float a)
{
    return (a*a);
}
//==================================
int strscmps(char *strvalue,char *soundtypes[],int counttypes)
{
    int i=0;
    for (i=0;i<counttypes;i++)
	if (!strcmp(strvalue,soundtypes[i]))
	    return i;
    return -1;
}
//==================================
void recalcxy(struct OBJ *x)
{
//    x->deltax=(short int)(x->destx-x->sourcex);
//    x->deltay=(short int)(x->desty-x->sourcey);
//    x->Sq=hypot(x->deltax,x->deltay);
//    x->Stotal=0;
}
//==================================
int rmdirrec(const char *dirname)
{
    DIR *dir;
    struct dirent *entry;
    char path[PATH_MAX];

    dir = opendir(dirname);
    if (dir == NULL) {
        return -1;
    }
    while ((entry = readdir(dir)) != NULL) 
    {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) 
        {
            snprintf(path, (size_t) PATH_MAX, "%s/%s", dirname, entry->d_name);
            if (entry->d_type == DT_DIR) 
            {
                rmdirrec(path);
            }
//            printf("(not really) Deleting: %s\n", path);
	    unlink(path);
        }

    }
    closedir(dir);
//    printf("(not really) Deleting: %s\n", dirname);
    rmdir(dirname);
    return 0;
}
//=======================================
long long FILElength(FILE *descriptor)
{
    long long old,len;
    old = ftell(descriptor);
    if (old==-1)
	return(-1);
    fseek(descriptor,0,SEEK_END);
    len = ftell(descriptor);
    fseek(descriptor,old,SEEK_SET);
    return(len);
}
//=======================================
long long FILElength(char *fn)
{
    struct stat st;
    memset(&st,0,sizeof(struct stat));
    if (stat(fn, &st))
	return(-1);
    return(st.st_size);
}
//=======================================
int CalcGradus(int x1,int y1,int x2,int y2)
{
    int deltax,deltay,alfa;
    deltax = x2-x1;
    deltay = y2-y1;
    if (deltax>=0)
    {
	if (!deltay)
    	    alfa=90;
        else
	    if (deltay<0)//I
            {
    		alfa=(int) ((float)(atan((float)deltax/-deltay)*180/PI));
            }
            else	//IV
            {
        	alfa=180-(int) ((float)(atan((float)deltax/deltay)*180/PI));
            }
    }
    else
    {
	if (!deltay)
    	    alfa=270;
	else
    	    if (deltay<0)//II
            {
        	alfa=360-(int )((float)(atan((float)deltax/deltay)*180/PI));
            }
            else	//III
            {
        	alfa=180+(int) ((float)(atan((float)-deltax/deltay)*180/PI));
            }
    }
    return(alfa);
}
//=======================================
unsigned char CalcDirection(int x1,int y1,int x2,int y2)
{
    return((256*CalcGradus(x1,y1,x2,y2)+179)/360);
}
//=================================
void calcinert(void)
{
    int i,sni,csi;
    float j,cs,sn;
    for (i=0;i<256;i++)
    {
      j = i*360*PI/180/256;
      cs = cos(j) * 65536;
      sn = sin(j) * 65536;
      inertion256[i][0] = (int) sn;
      inertion256[i][1] = (int) -cs;
    }
}
//=================================
void calcsintables(void)
{
    int i;
    unsigned char j;
    for (i=0;i<256;i++)
    {
	sintable256[i] = sin((i+64)*2*PI/256)*256;
	costable256[i] = cos((i+64)*2*PI/256)*256;
    }
}
//===========================================
int savebuff(const char *fn,void *address,int bufsize)
{
    FILE *f;
    f=fopen(fn,"wb");
    if (!f)
	return(-1);
    fwrite(address,bufsize,1,f);
    fclose(f);
}
//=================================
unsigned int mkchecksumm(char *filename)
{
    unsigned char *buff;
    unsigned int chksum;
    long long fsize;
    FILE *f;
    f=fopen(filename,"r");
    if (!f)
	return(0x00000000);
    fsize=FILElength(f);
    if (fsize>MAXMAPFILESIZE)
	return(0x00000000);
    buff=(unsigned char *)wmalloc(fsize);
    fread(buff,fsize,1,f);
    chksum=CreateCRC(buff,fsize,BUFF_NOCRYPTIZE);
    wfree(buff);
    fclose(f);
    return(chksum);
}
//=================================
void Gimp_SavePal(char *fn,char *palettename,char *pal,int pixelsize)	//pixelsize 3 or 4 bytes RGB or RGB0
{
//	creating palette file for GIMP
    int i;
    FILE *f=fopen(fn,"w");
    if (f)
    {
	fprintf(f,"GIMP Palette\nName: %s\n#\n",palettename);
	for (i=0;i<256;i++)
	{
	    fprintf(f,"%3d %3d %3d\n",pal[i*pixelsize+0],pal[i*pixelsize+1],pal[i*pixelsize+2]);
	}
	fclose(f);
    }
}
//=================================
char *makefilename(char *firstbuf,int offsetbuf,char f1,char f2,char *f4)
{
    int add=0;
    if (f1)
    {
	firstbuf[offsetbuf+add]=f1;
	add++;
    }
    if (f2)
    {
	firstbuf[offsetbuf+add]=f2;
	add++;
    }
    if (f4)
	strcpy(firstbuf+offsetbuf+add,f4);
//    DEBUGMESSCR("%s\n",firstbuf);
    return(firstbuf);
}
//==========================================
int check_ifhavewget(void)
{
    int len,ret;
    unlink(".temp");
    ret=system("wget --version > .temp");
    len=FILElength(".temp");
    unlink(".temp");
    if (len>1)
        return (1);
    else
	return(0);
}




