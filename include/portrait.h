
#if !defined(_PORTRAIT_W)

#define _PORTRAIT_W

#include "defs.h"
#include "man.h"
#include "smacker/smacker.h"

#define HOLDPORTRAIT		0x01

#define NOSOUNDFILENR		-1

#define SMKSIZEPORTRAITX	60
#define SMKSIZEPORTRAITY	56

#define MAXMIXITERATION		4
#define MINMIXITERATION		1

#define SMKICONPOSX		413
#define SMKICONPOSY		410


#define SMKATSTART		-1
#define SMKNONE			0
#define SMKNORMAL		1
#define SMKTALK			2
#define SMKNORMAL2		3
#define SMKRETURNTOADVISOR	4
#define SMKIN			5
#define SMKOUT			6


#define PORTRAIT_NO_NOISE	0
#define PORTRAIT_IN_NOISE	1
#define PORTRAIT_OUT_NOISE	2
//===================================
struct SMKPLAY
{
    signed char		fadein;			// fadein iterations
    signed char		fadeout;		// fadeout iterations
    smk_t		*smk;
    unsigned char 	*vidbuff;
    MPQIDS		smkfile;
    unsigned char 	smkscroll;
};
//===================================
class PORTRAIT
{
public:
    SCUNIT		SC_Unit;
    struct OBJ 		*obj;
    char		whichuse;		//0-no use mixing,1-specialin,2-specialout
    char 		talksmk;
    SMKPLAY		smkplay;
    MPQIDS		nextload;
    int			showedtime;
    unsigned char 	typeofport;
    signed char		remainplayedtimes;
    
    unsigned char	nextplaynormal2;
    unsigned char 	*mergepixels;
    unsigned char 	*mixbuff;		//mixing buffer
    unsigned char	flags;
    inline SCUNIT GetShowedPortrait(void){ return(SC_Unit); };
    inline void HoldPortrait(void){ flags |= HOLDPORTRAIT; };
    inline void UnHoldPortrait(void){ flags &= ~HOLDPORTRAIT; };
};
//===================================
void GetINOUTsmk(void);
int  loadSMK(SMKPLAY *tempplay,int smkflags);
void unloadSMK(SMKPLAY *templay);
int  unloadstaticsmk(void);
int  loadstaticsmk(void);
void changeunitportrait(HANDLE hmpq,int smknr,SCUNIT SC_Unit);

void showportrait(void);
void drawmainportrait(int x,int y,unsigned char *pixelsbuff);
void mergepixels(int iteration,unsigned char *outbuf,unsigned char *buf,unsigned char *buf2);
int  SetPortrait(SCUNIT SC_Unit,int typeofport,int soundfilenr,int showedtime);
int  SetPortraitFromSound(SCUNIT SC_Unit,int soundmode,int soundfilenr);
void ShowAdvisorPortrait(void);
void SetPortraitOBJ(OBJ *a);
OBJ *GetPortraitOBJ(void);
void SetPortraitShowedTime(int showedtime);

//===================================
extern char scrollportrait,mixportrait;
extern PORTRAIT staticport;

extern unsigned char playbuffer[];
extern MPQIDS	smkin,smkout;


#endif
