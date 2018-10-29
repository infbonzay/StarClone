#if !defined(_AUXIL_W)
#define _AUXIL_W

#include <stdio.h>
#include "const.h"

#ifdef	UNDERDOS
		#define _int3h() _asm{ int 3h}
#endif
#ifdef UNDERLINUX
		#define ltoa(i,aa) sprintf(aa,"%ld",(long) i)
		#define itoa2(i,aa) sprintf(aa,"%d",(int) i)
		#define itoa(i,aa,format) itoa2(i,aa)
#endif

#define createmaxkubik(a,b,sizex) \
					  maxkubik = (int)(sizex/SIZEXKUBIK);\
					  kubik	   = (int)((a*maxkubik+b-1)/b)

#define createmaxkubikSH(a,b,sizex) \
					  maxkubik = (int)(sizex/SIZEXKUBIK);\
					  kubik	   = (int)(a*maxkubik/b)

#define MAXMAPFILESIZE					(5*1024*1024)			//5MB
//==================================
#define sqr(a) ((a)*(a))
#define min(a,b)		(((a)<(b)) ? (a) : (b) )
#define max(a,b)		(((a)>(b)) ? (a) : (b) )

char *filename2dos(const char *fn);
char *filename2unix(const char *fn);
char *filename2temp(const char *fn);

void witoa(int var1,char *adr,int simb);
char *witoa(int var,char *adr);
void swap(int *a,int *b);
//==================================
void swapw(short *a,short *b);
//==================================
void CenterXYArea(struct XY *xy,int *xmed,int *med);

float wsqr(float a);
//==================================
void setmemd(char *adr,int count,int value);
//==================================
int CalcTotalDistance(int fromspeed,int tospeed,int acceleration);

int	 strscmps(char *strvalue,char *soundtypes[],int counttypes);
int	 rmdirrec(const char *dirname);
long long FILElength(FILE *descriptor);
long long FILElength(const char *fn);
int	 CalcGradus(int x1,int y1,int x2,int y2);
unsigned char CalcDirection(int x1,int y1,int x2,int y2);
//=================================
void calcinert(void);
void calcsintables(void);

int	 savebuff(const char *fn,void *adress,int bufsize);
unsigned int mkchecksumm(char *filename);
void Gimp_SavePal(char *fn,char *palettename,char *pal,int pixelsize);
int	 zerocmp(void *array,int nrsimb);
char *makefilename(char *firstbuf,int offsetbuf,char f1,char f2,const char *f4);
int	 check_ifhavewget(void);

//==========================================


//=================================
extern int	 inertion256[256][2];
extern float sintable[360];
extern float costable[360];
extern short sintable256[256];
extern short costable256[256];



#endif /* _AUXIL_W */
