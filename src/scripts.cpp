#include <stdio.h>
#include <grplib/usegrp.h>

#include "mpqwrapper.h"
#include "defsunits.h"
#include "creep.h"
#include "debug.h"

#include "auxil.h"
#include "man.h"
#include "vars.h"
#include "key.h"
#include "gr.h"
#include "load.h"
#include "wmem.h"
#include "gener.h"
#include "mouse.h"
#include "scripts.h"
#include "mpq.h"
#include "lists.h"
#include "loadlo.h"
#include "putobj.h"
#include "objinfo.h"
#include "vision.h"
#include "doodad.h"
#include "portrait.h"
#include "market.h"
#include "weapons.h"
#include "overlay.h"
#include "stringfiles.h"
#include "scripts.h"

#ifdef WITHSDL
        #include "sdl/grsdl.h"
#endif
#ifdef UNDERDOS
        #include "dos/gr8dos.h"
        #include "dos/handlers.h"
#endif
//=================================
#ifdef UNDERLINUX
     #include <ctype.h>
     void strupr(char *str)
     {
         int i=0;
         while(str[i])
         {
            str[i] = toupper(str[i]);
            i++;
         }
     }
//==================================
     void strlwr(char *str)
     {
         int i=0;
         while(str[i])
         {
            str[i] = tolower(str[i]);
            i++;
         }
     }
#endif
//==================================
void parsespecialtext(char *namemage,char *strvalue)
{
    int i,ii,j,len,symbinline;
    len=strlen(strvalue);
    for (i=0,ii=0,symbinline=0;i<len;i++)
    {
        switch(strvalue[i])
        {
	    case '_':
//		for (j=i+1;j<len;j++)
//		    if (strvalue[j]==' ')
//			break;
		if (symbinline>=17)//&&j-i>=10)
		{
	    	    namemage[ii++]='\n';
	    	    namemage[ii++]=' ';
		    symbinline=1;
		}
		else
		{
	    	    namemage[ii++]=' ';
	    	    symbinline++;
	        }
	        break;
	    case '?':
	        if (strvalue[i+1]!='?')
		{
		    namemage[ii++]=strvalue[i+1]-'0';
	    	    symbinline++;
		}
		i++;
		break;
	    default:
		namemage[ii++]=strvalue[i];
	    	symbinline++;
	    	break;
	}
    }
    namemage[ii]=0;
}
//=====================================================
int getallnumbers(char *strvalue,int *maxnrforread,int maxnrs)
{
    int curentpos=0,tempbufpos=0,nrtosave=0;
    int number1=-1,number2;
    int i;
    char simbol;
    char tempbuf[256];
    unsigned int len=strlen(strvalue);
    if (len>=sizeof(tempbuf))
    {
	printf("temp string buffer exceeded\n");//need change
	return -1;
    }
need_to_repeat_pass_1:
    do{
	simbol=strvalue[curentpos++];
	while((simbol>='0'&&simbol<='9')||(simbol=='-'&&(!tempbufpos)))
	{
	    tempbuf[tempbufpos++]=simbol;
	    simbol=strvalue[curentpos++];
	}
	tempbuf[tempbufpos]=0;
	tempbufpos=0;
	number1 = atoi(tempbuf);    
	if (!simbol)
	{
	    if (nrtosave<maxnrs)
		maxnrforread[nrtosave++] = number1;
	    return nrtosave;
	}    
	if (simbol==',')
	{
	    if (nrtosave<maxnrs)
		maxnrforread[nrtosave++] = number1;
	    number1=-1;
	    goto need_to_repeat_pass_1;
	}    
	else
	if (simbol=='-'&&number1!=-1)
	{
	    simbol=strvalue[curentpos++];
	    while((simbol>='0'&&simbol<='9')||(simbol=='-'&&(!tempbufpos)))
	    {
		tempbuf[tempbufpos++]=simbol;
		simbol=strvalue[curentpos++];
	    }
	    tempbuf[tempbufpos]=0;
	    tempbufpos=0;
	    number2 = atoi(tempbuf);
    	    if (number2>number1)
	    {
		for (i=number1;i<=number2;i++)
		    if (nrtosave<maxnrs)
			maxnrforread[nrtosave++] = i;
	    }
	    else
	    {
		for (i=number1;i>=number2;i--)
		    if (nrtosave<maxnrs)
			maxnrforread[nrtosave++] = i;
	    }
	    number1=-1;
	    number2=-1;
	}
	else
	if (simbol==':')
	{
	    simbol=strvalue[curentpos++];
	    while(simbol>='0'&&simbol<='9')
	    {
		tempbuf[tempbufpos++]=simbol;
		simbol=strvalue[curentpos++];
	    }
	    tempbuf[tempbufpos]=0;
	    tempbufpos=0;
	    number2 = atoi(tempbuf);
	    for (i=0;i<number2;i++)
	        if (nrtosave<maxnrs)
		    maxnrforread[nrtosave++] = number1;
	    number1=-1;
	    number2=-1;
	}
	else
	if (simbol=='+')
	{
	    simbol=strvalue[curentpos++];
	    while(simbol>='0'&&simbol<='9')
	    {
		tempbuf[tempbufpos++]=simbol;
		simbol=strvalue[curentpos++];
	    }
	    tempbuf[tempbufpos]=0;
	    tempbufpos=0;
	    number2 = atoi(tempbuf);
	    for (i=0;i<number2;i++)
	        if (nrtosave<maxnrs)
		    maxnrforread[nrtosave++] = number1+i;
	    number1=-1;
	    number2=-1;
	}
	if (!simbol)
	{
	    return nrtosave;
	}
    }while(1);
}
//=================================
int createmantable(void)
{
    char strid[200],strvalue[200],sss[200];
    long int pos;
    int intvalue,nrofobj=0,j;
    FILE *f;
    j=0;
    while(1)
    {
	if (!filemans[j][0])
	    break;
	f=fopen(filemans[j],"r");
	if (!f)
	{
	    printf("can't open %s file\n",filemans[j]);
	    j++;
	    continue;
    	}
//	sprintf(sss,"open [%s]",filemans[j]);
//    	savelog(sss,0);
	fseek(f,1,SEEK_SET);
	while (!feof(f))
	{
    	    pos=ftell(f);
    	    fscanf(f,"%s",strid);
    	    if (!strcmp(strid,rezu[0]))
    	    {
        	fscanf(f,"%s",strvalue);//read "="
        	fscanf(f,"%s",strvalue);
        	intvalue=atoi(strvalue);
        	if (pos_in_file_man[intvalue].pos!=0)
		{
    		    sprintf(sss,"dublicate objects [%d]!!!",intvalue);
    		    savelog(sss,0);
        	    gameend(forexit);
		}
		pos_in_file_man[intvalue].pos=pos;
		pos_in_file_man[intvalue].filenr=j;
		nrofobj++;
    	    }
	}//feof
	j++;
    }//while(1)
    fclose(f);
    return (nrofobj);
}
//=====================================================
OBJstruct *creatememforobjstruct(int intvalue)
{
    OBJstruct *adr=(struct OBJstruct *)wmalloc(sizeof(struct OBJstruct));
    memset(adr,0x00,sizeof(struct OBJstruct));
    allobj[intvalue]=adr;
    return(adr);
}
//=====================================================
void checkstrforrefer(char *str,int *refer,int *mulfactor)
{
    char *pos,*pos2;
    pos=strchr(str,'[');
    if (pos)
    {
	pos2=strchr(str,'*');
	if (pos2)
	{
	    pos2[0]=0;
	    if (mulfactor)
		*mulfactor=atoi(pos+1);
	    pos=pos2;
	}
	else
	    if (mulfactor)
		*mulfactor=1;
	pos[0]=0;
	pos2=strchr(pos+1,']');
	pos2[0]=0;
	if (refer)
	    *refer=atoi(pos+1);
    }
}
//=====================================================
int readrecordmans(FILE *f,int objnr)
{
HANDLE nr_mpq;
SCUNIT SC_Unit;
int sprites_id;

int mcost,gcost;
unsigned int j;
int k=0;

unsigned char flingy_id,movecontrol;
unsigned int topspeed;

int unitpict,unitpictpluseffect,soundfrom,soundto;
int t,t1,i,jj,ii,noerr,u,temp1,temp2,temp3,exitlo,soundtype;
int intvalue,intvalue2,kk;
float fvalue;
int nr_readed,offsfirstpict,maxsprites;
int maxsides,typesofmove,nrgrp;
int maxnormalmove,maxshadowmove,maxmove,firsttypeofmove;
int grpfirstnr,fileformat,nrofhits;
int lonr,typemode,speed,acceleration,images_id,images_tbl;
ATROBJ *adratr;
struct OBJstruct *adr,*fromotherobj,*tempobj;
//MOVESTRUCT *allmove;

int temptrigger_atspritenr;

int maxnrforread[100];
char strid[200],strvalue[200],strvalue2[200];
while(1)
{
    noerr=0;
    strcpy(strid,"");
    strcpy(strvalue,"");
    fscanf(f,"%s = %s\n",strid,strvalue);
    k++;
    strupr(strid);
    strupr(strvalue);
    intvalue=atoi(strvalue);
    fvalue=atof(strvalue);
    if (!strid[0])
        return(-1);
    for (j=0;j<sizeof(rezu)/sizeof(char *);j++)
    {
	if (!strcmp(strid,rezu[j]))
        {
             switch(j)
             {
             case 0://S [NEWOBJECT] numberspr nameobj
                if (intvalue>=NUMBRSPROBJ)
		{
            	    sprintf(forexit,"Error! object nr [%d] >= %d(NUMBRSPROBJ)",
                	    intvalue,NUMBRSPROBJ);
                    gameend(forexit);
		}     
		adr=creatememforobjstruct(intvalue);
		adr->additionaltext=-1;
		adr->creeptype=NOCREEPBUILD;
                SC_Unit=intvalue;
                adr->SC_Unit=intvalue;
		adr->portraits=FillSMKInfo(SC_Unit);
//		FillUnitOverlays(adr,SC_Unit);
		adr->armorupgnr = -1;
    		adr->sightupgnr = -1;
                adr->manaupgnr  = -1;
                adr->speedupgnr = 0xff;
                adr->atackspeedupgnr = -1;
                adr->SC_CreateFromUnit=SC_NOUNITNR;
    
		//we need to calculate repair cost for every tick
		GetCostUnit(SC_Unit,&mcost,&gcost);
		if (IsMechanical(SC_Unit))
		{
		    adr->repairmin=(mcost<<8)/3/GetUnitBuildTime(SC_Unit);
		    adr->repairgas=(gcost<<8)/3/GetUnitBuildTime(SC_Unit);
		    adr->repairlife=GetUnitMaxHealth(SC_Unit)/GetUnitBuildTime(SC_Unit);
		}
		else
		{
		    adr->repairmin=0;
		    adr->repairgas=0;
		    adr->repairlife=0;
                }
                //**************read name*************
                fscanf(f,"%s \n",strvalue);
		    break;
             case  1: //endobj
		return(0);
             case  2://B NOADDSTATATCONSTR
                  if (intvalue)
                  {
                     adr->UNITprop |= NOADDSTATSATCONSTR;
                  }
                  break;
             case  3://I SPEEDUPG
                adr->speedupgnr = intvalue;
                fscanf(f,"%s \n",strvalue);
                adr->speeduppercentage = (atof(strvalue)*25600/100);
                break;
             case  4://I ULTRALISKSECONDARMOR
		adr->armorupgnr=intvalue;
                break;
	    case   5://I SIGHTUPG
    		adr->sightupgnr=intvalue;
                break;
            case   6://i MANAUPG
                adr->manaupgnr=intvalue;
                break;
            case   7://[i],i,i,i,i,.. PROPERTIES
                if (intvalue>=128)
                {
            	    adr->UNITprop |= VARSELECT4NOTSEEPROP;
                    intvalue -=128;
                }
		if (intvalue>MAXLINESPROPERTIES)
		{
		    sprintf(forexit,"Error! obj[%d] line[%d] maxline=%d , exceeded"
				    " max value=[%d]",adr->SC_Unit,
				    k,intvalue,MAXLINESPROPERTIES);
		    gameend(forexit);
		}
		adr->maxlineprop = intvalue;
                int temp;
		for (t=0;t<adr->maxlineprop;t++)
                {
            	    for (t1=0;t1<9;t1++)
                    {
                	fscanf(f,"%s \n",strvalue);
                        temp = atoi(strvalue);
			adr->modemove[t][t1]=atoi(strvalue);
                    }
                }
                break;
            case  8://i TIMELEFT
                adr->maxtimeleft=intvalue;
            	break;
            case  9://CREATEFROMUNIT
            	    adr->SC_CreateFromUnit = intvalue;
            	    break;
            case 10://i ATACKSPEEDUPG
                adr->atackspeedupgnr=intvalue;
                  break;
            case 11:	//CREEP
		adr->creeptype = intvalue;
	        break;
    	    case 12://I CLOAKRANGE
		adr->cloakrange = intvalue;
		break;
    	    case 13://I ADDITIONALTEXT
		adr->additionaltext=intvalue;
		break;
	    default:
		fscanf(f,"%s \n",strvalue);
        	sprintf(forexit,"line [%d], [DIRECTIVE %s] in OBJ=[%d] not found next is[%s]",k,
            		strid,adr->SC_Unit,strvalue);
                gameend(forexit);
		break;
            }
            noerr=1;
            break;
          }//if
     }//for
     if (!noerr)
     {
    	if (strid[0]=='#')
    	{
	    fgets(strvalue,199,f);
	    continue;
	}
	fscanf(f,"%s \n",strvalue);
        sprintf(forexit,"line [%d],OBJ=[%d] %s not found next is [%s]",k,adr->SC_Unit,strid,strvalue);
        gameend(forexit);
     }
}
}
//=================================
char *LANGVALUES[2]={"ENGLISH","RUSSIAN"};
char *YESNO[2]={"NO","YES"};
char *PORTRAITS[3]={"YES","STATIC","NO"};
//=================================
void saveconf(void)
{
    char cfgname[512];
    sprintf(cfgname,"%s/" SC_CONFIGFILE ,GAMEPATH);
    FILE *f=fopen(cfgname,"w");
    fprintf(f,"%s = %s\n",rezo[0],LANGVALUES[gameconf.lang]);
    fprintf(f,"%s = %s\n",rezo[1],GAMEPATH);
    fprintf(f,"%s = %s%s\n",rezo[2],GAMEPATH,"/maps");
    fprintf(f,"%s = %d\n",rezo[3],gameconf.speedconf.gamespeed);
    fprintf(f,"%s = %d\n",rezo[4],gameconf.audioconf.musicvolume);
    fprintf(f,"%s = %d\n",rezo[5],gameconf.audioconf.soundvolume);
    fprintf(f,"%s = %s\n",rezo[6],YESNO[gameconf.audioconf.unitspeach]);
    fprintf(f,"%s = %s\n",rezo[7],YESNO[gameconf.audioconf.advisorsounds]);
    fprintf(f,"%s = %s\n",rezo[8],YESNO[gameconf.audioconf.buildsounds]);
    fprintf(f,"%s = %s\n",rezo[9],YESNO[gameconf.audioconf.subtitles]);
    fprintf(f,"%s = %d\n",rezo[10],gameconf.videoconf.gamma);
    fprintf(f,"%s = %d\n",rezo[11],gameconf.videoconf.saturate);
    fprintf(f,"%s = %s\n",rezo[12],YESNO[gameconf.videoconf.animation]);
    fprintf(f,"%s = %s\n",rezo[13],PORTRAITS[gameconf.videoconf.portraits]);
    fprintf(f,"%s = 2 ./starinstall.exe ./broodinstall.exe\n",rezo[21]);
    fprintf(f,"%s = 3 ./StarDat.mpq 50 ./BrooDat.mpq 90 ./StarClone.mpq 100\n",rezo[14]);

    fprintf(f,"%s = %d %d %d %d\n",rezo[15],gameconf.grmode.x,gameconf.grmode.y,gameconf.grmode.s,gameconf.grmode.fullscreen);
    fprintf(f,"%s = %d\n",rezo[16],gameconf.speedconf.mousescroll);
    fprintf(f,"%s = %d\n",rezo[17],gameconf.speedconf.keyscroll);
    fprintf(f,"%s = %s\n",rezo[18],YESNO[gameconf.speedconf.cputhrottling]);
    fprintf(f,"%s = %s\n",rezo[19],YESNO[gameconf.videoconf.visiblemap]);
    fprintf(f,"%s = %d\n",rezo[20],gameconf.networkconf.networklatency);
    fprintf(f,"%s = %d\n",rezo[22],gameconf.videoconf.preview);
    fclose(f);
}
//=================================
void loaddefcfg(void)
{
    gameconf.lang=0;
    gameconf.speedconf.gamespeed = FASTSPEED;
    gameconf.speedconf.mousescroll = NORMALSPEED;
    gameconf.speedconf.keyscroll = NORMALSPEED;
    gameconf.speedconf.cputhrottling = 1;
    gameconf.audioconf.musicvolume = 50;
    gameconf.audioconf.soundvolume = 50;
    gameconf.audioconf.unitspeach = 1;
    gameconf.audioconf.advisorsounds = 1;
    gameconf.audioconf.buildsounds = 1;
    gameconf.audioconf.subtitles = 1;
    gameconf.videoconf.gamma = MAXGAMMA/2;
    gameconf.videoconf.saturate = MAXSATURATE/2;
    gameconf.videoconf.animation = 1;
    gameconf.videoconf.portraits = CONF_VIDEO_PORTRAITS_ANIMATED;
    gameconf.grmode.x = 640;
    gameconf.grmode.y = 480;
    gameconf.grmode.s = 8;
    gameconf.grmode.fullscreen = 0;
    gameconf.videoconf.visiblemap = 0;
    gameconf.networkconf.networklatency = 0;
    gameconf.videoconf.preview = 0;
    saveconf();
}
//=================================
//char    *rezo[14]={  "LANG","GAMEPATH","MAPPATH","GAMESPEED","MUSICVOLUME","SOUNDVOLUME","UNITSPEACH",
//            	    "ADVISORSOUND","BUILDSOUND","SUBTITLES","GAMMA","ANIMATION","PORTRAITS"
//	    	    "MPQFILES","GRMODE","MOUSESCROLLSPEED","KEYSCROLLSPEED"
MPQPRIO *mpqprio;
//=================================
void unloadcfg(void)
{
    if (mpqprio)
    {
	wfree(mpqprio);	
	mpqprio=NULL;
    }
}
//=================================
int loadcfg(char *filename,int *mpqresult)
{
    unsigned int j,totalmpqs,i,result;
    int k=0,intvalue,err,err2,ii,jj,mpqfounded=1;
    static char strid[200],strvalue[200],rem[200];
    HANDLE installmpq,hmpq;
    FILE *f;
    f=fopen(filename,"r");
    if (!f)
    {
	loaddefcfg();
	return(-2);
    }
    while (!feof(f))
    {
	strcpy(strid,"");
        strcpy(strvalue,"");
        strcpy(rem,"");
	do{
    	    strid[0]=0;
	    fscanf(f,"%s = %s",strid,strvalue);
    	    k++;
	}while(strid[0]=='#');
	if (strlen(strid)==0)
	    break;
        strupr(strid);
//        strupr(strvalue);
        intvalue=atoi(strvalue);
    	err=1;
	for (j=0;j<sizeof(rezo)/sizeof(char *);j++)
	{
    	    if (!strcmp(strid,rezo[j]))
    	    {
                switch(j)
		{
            case  0://B LANG
                    if (!strcmp(strvalue,"ENGLISH"))
                      gameconf.lang=0;
                    else
                        if (!strcmp(strvalue,"RUSSIAN"))
                        gameconf.lang=1;
                        else
                        {
                            fclose(f);
                            return(k);
                        }
		    err=0;
                    break;
            case  1://S GAMEPATH
                    strcpy(GAMEPATH,strvalue);
		    err=0;
                    break;
            case  2://S MAPPATH
                    strcpy(GAMEMAPPATH,strvalue);
		    err=0;
                    break;
            case  3://I GAMESPEED
		    if (intvalue<VERYSLOWSPEED)
			intvalue=VERYSLOWSPEED;
		    else
			if (intvalue>FASTESTSPEED)
			    intvalue=FASTESTSPEED;
                    gameconf.speedconf.gamespeed=intvalue;
		    err=0;
                    break;
    	    case 4://I MUSICVOLUME
		    if (intvalue<0)
			intvalue=0;
		    else
			if (intvalue>100)
			    intvalue=100;
                    gameconf.audioconf.musicvolume=intvalue;
		    err=0;
                    break;
            case 5://I SOUNDVOLUME
		    if (intvalue<0)
			intvalue=0;
		    else
			if (intvalue>100)
			    intvalue=100;
                    gameconf.audioconf.soundvolume=intvalue;
		    err=0;
                    break;
            case 6://I UNITSPEACH
                    gameconf.audioconf.unitspeach=!strcmp(strvalue,"YES");
		    err=0;
                    break;
            case 7://I ADVISORSOUND
                    gameconf.audioconf.advisorsounds=!strcmp(strvalue,"YES");
		    err=0;
                    break;
            case 8://I BUILDSOUND
                    gameconf.audioconf.buildsounds=!strcmp(strvalue,"YES");
		    err=0;
                    break;
            case 9://I SUBTITLES
                    gameconf.audioconf.subtitles=!strcmp(strvalue,"YES");
		    err=0;
                    break;
            case 10://I GAMMA
		    if (intvalue<0)
			intvalue=0;
		    else
			if (intvalue>=MAXGAMMA)
			    intvalue=MAXGAMMA-1;
                    gameconf.videoconf.gamma=intvalue;
		    err=0;
                    break;
            case 11://I SATURATE
		    if (intvalue<0)
			intvalue=0;
		    else
			if (intvalue>=MAXSATURATE)
			    intvalue=MAXSATURATE-1;
                    gameconf.videoconf.saturate=intvalue;
		    err=0;
                    break;
            case 12://I ANIMATION
                    gameconf.videoconf.animation=!strcmp(strvalue,"YES");
		    err=0;
                    break;
            case 13://I PORTRAITS
		    if (!strcmp(strvalue,"YES"))
                	gameconf.videoconf.portraits=CONF_VIDEO_PORTRAITS_ANIMATED;
            	    else
            		if (!strcmp(strvalue,"STATIC"))
                	    gameconf.videoconf.portraits=CONF_VIDEO_PORTRAITS_STATIC;
                	else
            		    if (!strcmp(strvalue,"NO"))
                		gameconf.videoconf.portraits=CONF_VIDEO_PORTRAITS_DISABLED;
                	    else
                	    {
                		printf("error value for %s \n",strid);
                		return(-1);
                	    }
		    err=0;
                    break;
             case 14://MPQFILES
            	    totalmpqs=intvalue;
	    	    if (intvalue)
		    {
	    		MPQPRIO temp_prio;
	    		mpqprio = (MPQPRIO *)wmalloc(intvalue*sizeof(MPQPRIO));
	    		for (ii=0;ii<intvalue;ii++)
			{
                	    fscanf(f,"%s \n",strvalue);		//load mpq filefilename
                	    fscanf(f,"%s \n",rem);		//load priority of this mpq (high value-high priority(first access))
			    
			    strncpy(mpqprio[ii].mpqfilename,strvalue,MAX_PATH-1);
			    mpqprio[ii].prio = atoi(rem);
			}
			//priority sort
			for (ii=0;ii<intvalue;ii++)
			    for (jj=ii+1;jj<intvalue;jj++)
			    if (mpqprio[jj].prio>mpqprio[ii].prio)
			    {
				temp_prio.prio = mpqprio[jj].prio;
				strcpy(temp_prio.mpqfilename,mpqprio[jj].mpqfilename);
				
				mpqprio[jj].prio=mpqprio[ii].prio;
				strcpy(mpqprio[jj].mpqfilename,mpqprio[ii].mpqfilename);
				
				mpqprio[ii].prio = temp_prio.prio;
				strcpy(mpqprio[ii].mpqfilename,temp_prio.mpqfilename);
			    }
			mpqfounded=intvalue;
			for (ii=0,i=0;ii<intvalue;ii++)
			{
			    hmpq = LoadMpq(mpqprio[ii].mpqfilename);
			    if (!hmpq)
			    {
    			        printf("can't find %s file\n",mpqprio[ii].mpqfilename);
			    }
			    else
			    {
			        mpqprio[ii].presence=1;
				printf("Archive %s have %d files\n",mpqprio[ii].mpqfilename,GetMpqFilesNr(hmpq));
				mpqfounded--;
			    }
			}
		    }//if
		    err=0;
		    break;
             case 15://I GRMODE
                    gameconf.grmode.x=intvalue;
                    fscanf(f,"%s \n",strvalue);
                    gameconf.grmode.y=atoi(strvalue);
                    fscanf(f,"%s \n",strvalue);
                    gameconf.grmode.s=atoi(strvalue);
                    fscanf(f,"%s \n",strvalue);
                    gameconf.grmode.fullscreen=atoi(strvalue);
		    err=0;
            	    break;
            case  16://I MOUSESCROLLSPEED
		    if (intvalue<VERYSLOWSPEED)
			intvalue=VERYSLOWSPEED;
		    else
			if (intvalue>FASTESTSPEED)
			    intvalue=FASTESTSPEED;
                    gameconf.speedconf.mousescroll=intvalue;
		    err=0;
                    break;
            case  17://I KEYSCROLLSPEED
		    if (intvalue<VERYSLOWSPEED)
			intvalue=VERYSLOWSPEED;
		    else
			if (intvalue>FASTESTSPEED)
			    intvalue=FASTESTSPEED;
                    gameconf.speedconf.keyscroll=intvalue;
		    err=0;
                    break;
            case  18://I CPUTHROTTLING
                    gameconf.speedconf.cputhrottling=!strcmp(strvalue,"YES");
		    err=0;
                    break;
            case 19://I VIEWENTIREMAP
                    gameconf.videoconf.visiblemap=!strcmp(strvalue,"YES");
		    err=0;
                    break;
            case 20://I NETWORKLATENCY
		    if (intvalue<0)
			intvalue=0;
		    else
			if (intvalue>2)
			    intvalue=2;
                    gameconf.networkconf.networklatency=intvalue;
		    err=0;
                    break;
            case 21://INSTALLFILES
            	    //this files need's only for campaigns
		    for (ii=0;ii<intvalue;ii++)
		    {
            		fscanf(f,"%s \n",strvalue);
            		installmpq=NULL;
			result = wr_SFileOpenArchive(strvalue,0,MPQOPENPARAM,&installmpq);
			if (result)
			{
			    if (SFileHasFile(installmpq,STAR_INSTALL_SOMEFILE))
			    {
				install1mpq=installmpq;
    				printf("Install archive %s have %d files\n",strvalue,GetMpqFilesNr(install1mpq));
			    }
			    if (SFileHasFile(installmpq,BROOD_INSTALL_SOMEFILE))
			    {
				install2mpq=installmpq;
    				printf("Install archive %s have %d files\n",strvalue,GetMpqFilesNr(install2mpq));
			    }
			}
		    }
		    if (!install1mpq)
		    {
		        printf("Not found star install file. Star campaigns will be disabled\n");
		    }
		    if (!install2mpq)
		    {
		        printf("Not found brood install file. Brood campaigns will be disabled\n");
		    }
		    err=0;
            	    break;
	    case 22://firstrun
                    gameconf.videoconf.preview = intvalue;
		    err=0;
		    break;
	    	}//switch
    	    }//for,if
	}
	if (err)
	{
    	    printf("CFG: wrong parameter %s\n",strid);
    	    return(-1);
	}
    }//while
    fclose(f);
    if (mpqresult)
	*mpqresult=(totalmpqs<<8)|mpqfounded;
    return(0);
}
//=====================================================
int readmageproperties(void)
{
    char *statstr;
    OBJstruct *adr;
    FILE *f;
    unsigned int i,j,k,images_id,images_tbl;
    int intvalue,whatobj,ii,whatmage,grpfirstnr,nrgrp,totalmage=0;
    int grpnr;
    float fltvalue;
    int picttype,maxsprites,nr_readed,fileformat,maxupg;
    char c;
    int maxnrforread[200];
    static char strid[200],strvalue[200],strval2[200],fname[200];;
    f=fopen(filemageprop,"r");

while(!feof(f))
{
    strcpy(strid,"");
    strcpy(strvalue,"");
    fscanf(f,"%s = %s\n",strid,strvalue);
    strupr(strid);
//      strupr(strvalue);
    intvalue=atoi(strvalue);
    fltvalue=atof(strvalue);
    for (j=0;j<sizeof(rezmp)/sizeof(char *);j++)
	if (!strcmp(strid,rezmp[j]))
        {
            switch(j){
            case 0://S [NEWMAGE] numbermage namemage whatpict_in_icons.grp nratrmage
        	whatobj=intvalue;
	        maxupg=0;
	        totalmage++;
                fscanf(f,"%s \n",strvalue);
//              strlwr(strvalue);
                fscanf(f,"%s \n",strval2);
                mageprop[whatobj].icon_id=atoi(strval2);
                mageprop[whatobj].keyfororder=255;
		mageprop[whatobj].order_id=255;
		mageprop[whatobj].createweapon=255;

//		parsespecialtext(mageprop[whatobj].namemage,strvalue);
		mageprop[whatobj].type_id=ORDERS_NONE;
		mageprop[whatobj].obj_id=-1;
		mageprop[whatobj].depend.maxdepend=0;
		mageprop[whatobj].depend.type_id[0]=ORDERS_NONE;
		mageprop[whatobj].depend.type_id[1]=ORDERS_NONE;
		mageprop[whatobj].depend.type_id[2]=ORDERS_NONE;

#ifdef DEBUGOUTPUT
		strcpy(mageprop[whatobj].namemage,strvalue);
#endif
        	fscanf(f,"%s \n",strvalue);
        	intvalue = atoi(strvalue);
		if (intvalue>=MAXMAGEATR)
		{
            	    sprintf(forexit,"Error! [%d] mage atr > %d (MAXMAGEATR)",whatobj,MAXMAGEATR);
		    savelog(forexit,0);
    		    fclose(f);
		    return(-1);
		}
	        if (intvalue!=-1)
		{
		    if (magenrfrommageatr[intvalue])
	    	    {
            		sprintf(forexit,"Error! mage atr already exist and = [%d]mage!!!",magenrfrommageatr[intvalue]);
		        savelog(forexit,0);
    			fclose(f);
			return(-1);
	    	    }
	    	    magenrfrommageatr[intvalue]=whatobj;
		}
		mageprop[whatobj].atronobj = intvalue;
		mageprop[whatobj].soundmode = -1;
		mageprop[whatobj].race=NEUTRALRACE;
                break;
            case 1://I MINDISTANCE
            	mageprop[whatobj].mindistance = (intvalue << 8);
	        break;
            case 2://I DIAPAZONE
               mageprop[whatobj].diapazone = (intvalue << 8);
                break;
            case 3://I rightway
                if (!strcmp(strvalue,"YES"))
                {
            	    mageprop[whatobj].atr |= ORDER_RIGHTWAY;
            	}
            	else
		{
		    int ormask = 0x00;
		    int andmask = 0x00;
		    fscanf(f,"%s \n",strvalue);
            	    if (!strcmp(strvalue,"ONTERRAIN"))
            	    {
			ormask |= WTF_ONTERRAIN;
            		mageprop[whatobj].atr |= ORDER_ONTERRAIN;
		    }else if (!strcmp(strvalue,"ONUNIT"))
            	    {
			andmask |= WTF_ONTERRAIN;
		    }
		    else
		    {
        		sprintf(forexit,"Error! load [%s] need to be (ONUNIT | ONTERRAIN)",strvalue);
			savelog(forexit,0);
			fclose(f);
            		return(-3);
		    }
		    fscanf(f,"%s \n",strvalue);
		    int maxtypesofunit = atoi(strvalue);
            	    for (int i=0;i<maxtypesofunit;i++)
		    {
			fscanf(f,"%s \n",strvalue);
            		strupr(strvalue);
            		if (!strcmp(strvalue,"ONSPELLCASTER"))
            		{
			    ormask |= WTF_STARCLONE_ONSPELLCASTER;
			}
			else if (!strcmp(strvalue,"NONBUILD"))
            		{
			    ormask |= WTF_NONBUILD;
		        }
			else if (!strcmp(strvalue,"ONAIR"))
            		{
			    ormask |= WTF_AIR;
		        }
			else if (!strcmp(strvalue,"ONGROUND"))
            		{
			    ormask |= WTF_GROUND;
		        }
			else if (!strcmp(strvalue,"ONORGANIC"))
            		{
			    ormask |= WTF_ORGANIC;
		        }
			else if (!strcmp(strvalue,"MECHANICALORGANIC"))
            		{
			    ormask |= WTF_MECHANICALORGANIC;
		        }
			else if (!strcmp(strvalue,"OWN"))
            		{
			    ormask |= WTF_OWN;
		        }
			else if (!strcmp(strvalue,"NOUNITDESTINATION"))
            		{
            		    //cast on place were is unit where in casting time
            		    mageprop[whatobj].atr |= ORDER_IGNOREUNITDESTINATION;
		        }
			else
			{
        		    sprintf(forexit,"Error! load [%s] need to be (ONSPELLCASTER|NONBUILD)",strvalue);
			    savelog(forexit,0);
			    fclose(f);
            		    return(-3);
			}
		    }
		    if (mageprop[whatobj].createweapon != 255)
		    {
			alldattbl.weapons_dat->TargetFlags[mageprop[whatobj].createweapon] |= ormask;	//change default target flags
			alldattbl.weapons_dat->TargetFlags[mageprop[whatobj].createweapon] &= ~andmask;	//change default target flags
		    }
		}
                break;
	    case 4://IGNOREREQUEREMENTS
            	    mageprop[whatobj].atr |= ORDER_IGNOREREQ;
		    break;
            case 5://I TIMEMAGEACTIVE
                    mageprop[whatobj].timemageactive=intvalue;
        	    break;
    	    case 6://IMAGES_ID
		    mageprop[whatobj].images_id[0]=intvalue;
                    fscanf(f,"%s \n",strvalue);
		    mageprop[whatobj].images_id[1]=atoi(strvalue);
                    fscanf(f,"%s \n",strvalue);
		    mageprop[whatobj].images_id[2]=atoi(strvalue);
		    break;
	    case 7://ORDERID
		    mageprop[whatobj].order_id=intvalue;
		    break;
	    case 8://DEPENDONTYPEID = NRsOFDEPEND [dependtypeid1 dependobjid1 ... ]
		    mageprop[whatobj].depend.maxdepend=intvalue;
		    for (i=0;i<intvalue;i++)
		    {
                	fscanf(f,"%s %s \n",strvalue,strval2);
			mageprop[whatobj].depend.obj_id[i]=atoi(strval2);
			mageprop[whatobj].depend.obj_id2[i]=SC_NOUNITNR;
			if (!strcmp(strvalue,"UNIT"))
			{
			    mageprop[whatobj].depend.type_id[i]=ORDERS_UNIT;
			}
			else
			    if (!strcmp(strvalue,"UPGRADE"))
			    {
				mageprop[whatobj].depend.type_id[i]=ORDERS_UPGRADE;
			    }
		    	    else
				if (!strcmp(strvalue,"TECHNOLOGY"))
				{
				    mageprop[whatobj].depend.type_id[i]=ORDERS_TECHNOLOGY;
				}
				else
				    if (!strcmp(strvalue,"UNITANDADDON"))
				    {
					mageprop[whatobj].depend.type_id[i]=ORDERS_UNIT;
                			fscanf(f,"%s \n",strvalue);
					mageprop[whatobj].depend.obj_id2[i]=atoi(strvalue);
				    }
				    else
				    {
					printf("error dependon...%s\n",strvalue);
				    }
		    }
		    break;
	    case 9://CREATEWEAPON
		    mageprop[whatobj].createweapon=intvalue;
		    break;
	    case 10://queuedtomovelist
		    mageprop[whatobj].atr |= ORDER_CANBEQUEUED;
		    break;
            case 11://CANNOTBEINTERRUPTED
		    mageprop[whatobj].atr |= ORDER_CANNOTBEINTERRUPTED;
		    break;
            case 12://SOUNDONCASTANDHIT
        	    mageprop[whatobj].sound_id[SOUNDONCAST] = intvalue;
            	    fscanf(f,"%s \n",strvalue);
		    mageprop[whatobj].sound_id[SOUNDONHIT]  = atoi(strvalue);
		    break;
	    case 13://RACE
		    if (!strcmp(strvalue,"ZERG"))
		    {
			mageprop[whatobj].race=ZERGRACE;
		    }
		    else
			if (!strcmp(strvalue,"TERRAN"))
			{
			    mageprop[whatobj].race=TERRANRACE;
			}
			else
			    if (!strcmp(strvalue,"PROTOSS"))
			    {
				mageprop[whatobj].race=PROTOSSRACE;
			    }
			    else
			    {
                		sprintf(forexit,"MAGE:error [%s] in [%d] not found",strvalue,whatobj);
        			savelog(forexit,0);
     				fclose(f);
				return(-6);
			    }
		    break;
	    case 14://SHOWHALLUCINATED
		    mageprop[whatobj].atr |= ORDER_SHOWHALLUCINATED;
		break;
	    case 15://ORDERTYPE
		if (!strcmp(strvalue,"ORDER"))
		{
		    mageprop[whatobj].type_id=ORDERS_MODEMOVE;
		    intvalue=1;
		}
		else
		    if (!strcmp(strvalue,"UNIT"))
		    {
			mageprop[whatobj].type_id=ORDERS_UNIT;
			intvalue=1;
		    }
		    else
			if (!strcmp(strvalue,"UPGRADE"))
			{
			    mageprop[whatobj].type_id=ORDERS_UPGRADE;
			    intvalue=3;
			}
			else
			    if (!strcmp(strvalue,"TECHNOLOGY"))
			    {
				mageprop[whatobj].type_id=ORDERS_TECHNOLOGY;
				intvalue=1;
			    }
			    else
			    {
				printf("error ordertype %s\n",strvalue);
				gameend("");
			    }
            	fscanf(f,"%s \n",strvalue);
		mageprop[whatobj].obj_id=atoi(strvalue);
		for (i=0;i<intvalue;i++)
		{
            	    fscanf(f,"%s \n",strvalue);
		    mageprop[whatobj].stattxt_id_disable[i]=atoi(strvalue);
		}
            	fscanf(f,"%s \n",strvalue);
	    	mageprop[whatobj].stattxt_id_enable=atoi(strvalue);
	    	statstr=alldattbl.stattxt_tbl->get_TBL_STR(mageprop[whatobj].stattxt_id_enable);
	    	if (statstr[1]<=7)
	    	    mageprop[whatobj].keyfororder=toupper(statstr[0]);
		break;
	    default:
                  sprintf(forexit,"MAGE:error [%s] in [%d] not found",strid,whatobj);
        	  savelog(forexit,0);
     		  fclose(f);
		  return(-6);
             }
             break;
        }
    if (j>=sizeof(rezmp)/sizeof(char *))
    {
        if (strid[0]=='#')
	    continue;
        sprintf(forexit,"MAGE:error [%s] in [%d] not found",strid,
	          whatobj);
        savelog(forexit,0);
	fclose(f);
	return(-6);
    }
}
fclose(f);
return(totalmage);
}
//=====================================================
#define MAXFIDSMKFILE	4
#define MAXTLKSMKFILE	3
char typeofsmk1[MAXFIDSMKFILE]={SMKNORMAL,SMKNORMAL2,SMKNORMAL2,SMKNORMAL2};
char typeofsmk2[MAXTLKSMKFILE]={SMKTALK,SMKTALK,SMKTALK};
//=====================================================
void FreeSMKInfo(SMKPORTRAITS *smkinfo)
{
    wfree(smkinfo);
}
//=====================================================
SMKPORTRAITS *FillSMKInfo(SCUNIT SC_Unit)
{
    SMKPORTRAITS *portraits=NULL;
//    HANDLE hmpq;
    MPQIDS tempfile;
    int portdata_id,portdata_tbl,i,len,typeofsmk,mpqfilenr;
    char smkfilename[40];
    portdata_id=alldattbl.units_dat->portdata_id[SC_Unit];
    if (portdata_id!=-1)
    {
	portraits=(SMKPORTRAITS *)wmalloc(sizeof(SMKPORTRAITS));
	memset(portraits,0,sizeof(SMKPORTRAITS));
	//the fidxx.smk portraits
	portdata_tbl=alldattbl.portdata_dat->portdata_tbl[portdata_id];
	strcpy(smkfilename,"portrait\\");
	strcat(smkfilename,alldattbl.portdata_tbl->get_TBL_STR(portdata_tbl-1));
	strcat(smkfilename,"X.smk");
	len=strlen(smkfilename)-5;
	strlwr(smkfilename);
	for (i=0;i<MAXFIDSMKFILE;i++)
	{
	    smkfilename[len]=i+'0';
	    typeofsmk=typeofsmk1[i];
/*	    hmpq=GetFilePosInArchive(NULL,smkfilename,&mpqfilenr);
	    if (hmpq)
	    {
		portraits->mpqfilenr[typeofsmk][portraits->max[typeofsmk]]=mpqfilenr;
		portraits->max[typeofsmk]++;
	    }
*/
	    if (GetMPQFileID(NULL,smkfilename,&tempfile)!=-1)
	    {
		portraits->mpqfilenr[typeofsmk][portraits->max[typeofsmk]]=tempfile.mpqfilenr;
		portraits->max[typeofsmk]++;
	    }
	    if (!i)
	    {
//		portraits->hmpq=hmpq;
		portraits->hmpq=tempfile.hmpq;
		portraits->maxscrollSMKNORMAL=alldattbl.portdata_dat->smk_change[portdata_id];//value to change to SMKNORMAL2
		if (portraits->maxscrollSMKNORMAL>=100)
		{
		    portraits->maxscrollSMKNORMAL=0;
		}
	    }
	}
	//the tlkxx.smk portraits
	portdata_tbl=alldattbl.portdata_dat->portdata_tbl[portdata_id+110];
	strcpy(smkfilename,"portrait\\");
	strcat(smkfilename,alldattbl.portdata_tbl->get_TBL_STR(portdata_tbl-1));
	strcat(smkfilename,"X.smk");
	len=strlen(smkfilename)-5;
	strlwr(smkfilename);
	for (i=0;i<MAXTLKSMKFILE;i++)
	{
	    smkfilename[len]=i+'0';
	    typeofsmk=typeofsmk2[i];
/*	    hmpq=GetFilePosInArchive(NULL,smkfilename,&mpqfilenr);
	    if (hmpq)
	    {
		portraits->mpqfilenr[typeofsmk][portraits->max[typeofsmk]]=mpqfilenr;
		portraits->max[typeofsmk]++;
	    }
*/
	    if (GetMPQFileID(NULL,smkfilename,&tempfile)!=-1)
	    {
		portraits->mpqfilenr[typeofsmk][portraits->max[typeofsmk]]=tempfile.mpqfilenr;
		portraits->max[typeofsmk]++;
	    }
	}
    }
    return(portraits);
}
//=====================================================


