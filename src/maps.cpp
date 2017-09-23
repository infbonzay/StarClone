#include <grplib/gr8.h>
#include <grplib/usegrp.h>
#include "auxil.h"
#include "man.h"
#include "doodad.h"
#include "gr.h"
#include "load.h"
#include "mage.h"
#include "vars.h"
#include "diap.h"
#include "gr.h"
#include "objs.h"
#include "mouse.h"
#include "tempobj.h"
#include "starmap.h"
#include "creep.h"
#include "action.h"
#include "objinfo.h"
#include "vision.h"
#include "fog.h"
#include "gener.h"
#include "market.h"
#include "images.h"
#include "maps.h"

#ifdef WITHSDL
       #include "sdl/grsdl.h"
#endif
#ifdef UNDERDOS
       #include "dos/gr8dos.h"
       #include "dos/handlers.h"
#endif

struct mapinfo map;
int Xkartbeg,Ykartbeg,Xkartend,Ykartend;
float factorx,factory;
int sizemaprectx,sizemaprecty;
//==============================
void CreateMiniMapPixels(struct mapinfo *map)
{
    MAXXMAP = map->map_width;
    MAXYMAP = map->map_height;
    MAXXMAP8 = map->map_width*4;
    MAXYMAP8 = map->map_height*4;
    Xkartbeg = map->minimap_startx;
    Ykartbeg = map->minimap_starty;
    Xkartend = map->minimap_endx;
    Ykartend = map->minimap_endy;
    mouser[MOUSEMODE4].x1 = Xkart+Xkartbeg;
    mouser[MOUSEMODE4].y1 = Ykart+Ykartbeg;
    mouser[MOUSEMODE4].x2 = Xkart+Xkartend;
    mouser[MOUSEMODE4].y2 = Ykart+Ykartend;
    factorx=(float)((Xkartend-Xkartbeg))/MAXXMAP;
    factory=(float)((Ykartend-Ykartbeg))/MAXYMAP;
    sizemaprectx=(int)(widthkart*factorx);
    sizemaprecty=(int)(hightkart*factory);
}
//==============================
//x32,y32 - coords in map 128x128 for example,
//sizex,sizey - size of object 1 - is 32 pixels
//==============================
void GetMiniMapCoord(int x32,int y32,
		     int sizex,int sizey,
		     int *xminimap,int *yminimap,
		     int *sizexinpixels,int *sizeyinpixels)
{
    *xminimap = Xkart+Xkartbeg+(int)(x32*factorx);
    *yminimap = Ykart+Ykartbeg+(int)(y32*factory);
    *sizexinpixels=(int)(sizex*factorx+0.5);
    *sizeyinpixels=(int)(sizey*factory+0.5);
}
//==============================
void minimap_showobj(OBJ *a)
{
    int sx,sy;
    if (a->SC_Unit == SC_MAPREVEALEROBJ)
	return;
    if (!OBJ_VAR_CHK(a,obj_notdetect,NUMBGAMER) || (IsDoodadState(a->SC_Unit) && GetDoodadState(a) == DOODAD_TOP_STATE))
    {
	sx=GetUnitBoxWidth(a->SC_Unit);
	sy=GetUnitBoxHeight(a->SC_Unit);
	ObjOnMiniMap(a->xkart-sx/64,a->ykart-sy/64,sx,sy,tableforminimap[a->color],Minimap);
    }
}
//==============================================
void getminimapcoord(struct mapinfo *info,int x,int y,int *xm,int *ym)
{
    *xm = (int) ((x*factorx/SIZESPRLANSHX)+Xkartbeg)+Xkart;
    *ym = (int) ((y*factory/SIZESPRLANSHY)+Ykartbeg)+Ykart;
}
//==============================================
void ObjOnMiniMap(int xkart,int ykart,int sizex,int sizey,char color,char *minimapadr)
{
    int i,j,minix,miniy,pixx,pixy;
    pixx=(int)((sizex+16)/32*factorx+0.5);
    pixy=(int)((sizey+16)/32*factory+0.5);
    minix = (int) ((xkart*factorx)+Xkartbeg);
    for (i=0;i<pixy;i++)
    {
	miniy = (int)((ykart*factory)+Ykartbeg+i)*MAXIMSIZEMINIMAP;
	for (j=0;j<pixx;j++)
	{
	    minimapadr[miniy+minix+j] = color;
	}
    }
}
//==============================
void drawMINIMAP(void)
{
    int xbeg,ybeg,xend,yend;
    if (MAPDEF)
    {
	CPutPartVisibleSpr( Xkart,Ykart,128,128,Minimap);
	GetMiniMapCoord(map.MAPX,map.MAPY,widthkart,hightkart,&xbeg,&ybeg,&xend,&yend);
	xend+=xbeg;
	yend+=ybeg;
	wrectangle(WHITECOLOR,xbeg,ybeg,xend,yend);
	if (Ykartbeg>0)
	    horizline(WHITECOLOR,Xkartbeg+Xkart-1,Xkartend+Xkart+1,Ykartbeg+Ykart);
	if (Ykartend<128)
	    horizline(WHITECOLOR,Xkartbeg+Xkart-1,Xkartend+Xkart+1,Ykartend+Ykart);
	if (Xkartbeg>0)
	    vertline(WHITECOLOR,Xkartbeg+Xkart,Ykartbeg+Ykart-1,Ykartend+Ykart+1);
	if (Xkartend<128)
	    vertline(WHITECOLOR,Xkartend+Xkart,Ykartbeg+Ykart-1,Ykartend+Ykart+1);
    }
}
//==================================
int MAPAROUND[8][2]={
			{-1,-1},{ 0,-1},{ 1,-1},
			{-1, 0},        { 1, 0},
			{-1, 1},{ 0, 1},{ 1, 1}
			};

//==================================
//return 0 if around not open map
int checkaroundtile(int x,int y)
{
    int i,newx,newy,ret=0;
    for (i=0;i<8;i++)
    {
	newx=x+MAPAROUND[i][0];
	newy=y+MAPAROUND[i][1];
	if (newx<0||newy<0||newx>=MAXXMAP||newy>=MAXYMAP)
	    continue;
	if (mapSEE(newx,newy)>0||mapOPEN(newx,newy)>0)
	    ret+=1;
    }
    return(ret);
}

//==================================
#define SHADOWFOROPENEDCELLMAP		0x10
#define SHADOWFORCLOSEDCELLMAP		0x08	//work only if visiblemap is set
//==================================

void saveoldtileandcreep(void)
{
    int i,j,nosave,indextile32;
    unsigned char wf,bf;
    signed char creepnr;
    for (i=0;i<MAXYMAP;i++)
	for (j=0;j<MAXYMAP;j++)
	{
	    nosave=0;
	    wf=mapSEE(j,i);
	    bf=mapOPEN(j,i);
	    if (wf>1||bf>1)//if opened map
	    {
    		indextile32=map.display_map[i*MAXXMAP+j];
		creepnr = GetCreepAroundWithFog(j,i,indextile32);
		if (wf<=0)//if we see it before,but now fog we put saved pictures
		{
		    if (map.mapbits.savedtile[NUMBGAMER])
			if (map.mapbits.savedtile[NUMBGAMER][i*MAXXMAP+j]!=0xffff)
			    indextile32=map.mapbits.savedtile[NUMBGAMER][i*MAXXMAP+j];
		    nosave=1;
		}
		if (creepnr==-1)
		{
		    if (!nosave)
    			savemaptileadr(j,i,indextile32);
		}
		if (!nosave)
		    savemapcreepadr(j,i,map.creepflagplace[i*MAXXMAP+j]);
	    }
	}
}
//==================================
void drawMAP(int ignorefirstwaiting)
{
    unsigned char wf,bf;
    static int firstmap;
    signed char creepnr,creepoffset;
    int i,j,indextile32,nrobj,method,open,see;
    int POSINMAP;
    int bh,eh,bw,ew,ii,jj,xk,yk,xkk,ykk;
    char minimapcolor;
    if (!CODEFORSCREEN)
	wclrscr(0);
    else
    {
	if (ignorefirstwaiting)
	{
	    for (i=0;i<PLAYEDPLAYERS;i++)
	    {
    		if (IfPlayerHaveStartLocation(&map,i)!=-1)
    		{
    		    memset(map.mapbits.whitefog[i],0x4,map.map_width*map.map_height);
    		    memset(map.mapbits.whitefog2[i],0x4,map.map_width*map.map_height);
    		    memset(map.mapbits.blackfog[i],0x4,map.map_width*map.map_height);
    		}
	    }
	}
    }
    bh=map.MAPY;
    eh=bh+hightkart+1;
    if (eh > MAXYMAP)
        eh = MAXYMAP;
    bw=map.MAPX;
    ew=bw+widthkart+1;
    if (ew > MAXXMAP)
	ew = MAXXMAP;
    xk=map.MAPXGLOBAL;
    yk=map.MAPYGLOBAL;
    xkk = xk % SIZESPRLANSHX;
    ykk = yk % SIZESPRLANSHY;
    memset(screenmapused,0,(hightkart+1)*(widthkart+1));
    for (ii=0,i=bh;i<eh;i++,ii++)
    {
	for (jj=0,j=bw;j<ew;j++,jj++)
	{
	    wf=mapSEE(j,i);
	    bf=mapOPEN(j,i);
	    if (wf>1||bf>1)//if opened map
	    {
    		indextile32=map.display_map[i*MAXXMAP+j];
                putlansh(jj*SIZESPRLANSHX-xkk,ii*SIZESPRLANSHY-ykk,j,i,indextile32,wf,bf);
		screenmapused[ii*(widthkart+1)+jj]=1;
	    }
	    else
	        if (gameconf.videoconf.visiblemap||checkaroundtile(j,i))//if around not open we don't show tile
	    	    {
    			indextile32=map.display_map[i*MAXXMAP+j];
            		putlansh(jj*SIZESPRLANSHX-xkk,ii*SIZESPRLANSHY-ykk,j,i,indextile32,wf,bf);
			screenmapused[ii*(widthkart+1)+jj]=1;
            	    }
	    if (SHOWCELLS)
	    {
		if (mapEFFECT(j,i,DSWARMEFFECT))
		    CSetImage8x(jj*SIZESPRLANSHX-xkk,ii*SIZESPRLANSHY-ykk,32,32,200);
		if (mapEFFECT(j,i,DISRUPTEFFECT))
		    CSetImage8x(jj*SIZESPRLANSHX-xkk,ii*SIZESPRLANSHY-ykk,32,32,200);
	    }
	}
    }
    //if time to regeniration minimap and need to put terrain
    if (MAPREGENERATIONBIT&&(MAPDEF&(TERRAIN|UNITS)))
	setmemd(Minimap,MAXIMSIZEMINIMAP*MAXIMSIZEMINIMAP/4,BLACKCOLOR);
    //drawminimap terrain with creep
    if ((MAPREGENERATIONBIT&&(MAPDEF&TERRAIN))||ignorefirstwaiting)
    {
    //if time to regeniration minimap
	for (i=Ykartbeg;i<Ykartend;i++)
	{
	    ii = (int)((i-Ykartbeg)/factory);
	    for (j=Xkartbeg;j<Xkartend;j++)
	    {
		jj = (int) ((j-Xkartbeg)/factorx);
		if (mapSEE(jj,ii)>1)
		    creepnr=map.creepflagplace[ii*MAXXMAP+jj];
		else
		    if (map.mapbits.savedcreep[NUMBGAMER])
			creepnr=map.mapbits.savedcreep[NUMBGAMER][ii*MAXXMAP+jj];
		    else
			creepnr=0;
		if (map.mapbits.underfog[NUMBGAMER]&&
		    map.mapbits.underfog[NUMBGAMER][ii*MAXXMAP+jj]&&
		    (map.mapbits.underfog[NUMBGAMER][ii*MAXXMAP+jj]->nrofslots>0))
		{
			if (map.mapbits.underfog[NUMBGAMER][ii*MAXXMAP+jj]->pictureslot[0].flags & UNDERFOG_MAINIMAGE)
			    minimapcolor=map.mapbits.underfog[NUMBGAMER][ii*MAXXMAP+jj]->pictureslot[0].grcolor;	//putobjonminimap
			//previous color
		}
		else
		    if (creepnr==0)
		    {
			minimapcolor = map.minimap[i*MAXIMSIZEMINIMAP+j];
		    }
		    else
		    {
//		    	if (creepnr>=MAXCREEPNR)
//			    minimapcolor = (map.creeptileminimapcolors[CREEPCOLOR-MAXCREEPNR]>>creepoffset)&0xff;
//		    	else
			    minimapcolor = CREEPCOLOR;
        	    }
            	if (mapSEE(jj,ii)>1)
            	    if (minimapcolor)
            		Minimap[i*MAXIMSIZEMINIMAP+j] = minimapcolor;
            	else
            	    if (mapOPEN(jj,ii)>1)
            		Minimap[i*MAXIMSIZEMINIMAP+j] = _toblack_[minimapcolor+256*SHADOWFOROPENEDCELLMAP];
            	    else
            		if (gameconf.videoconf.visiblemap)
            		    Minimap[i*MAXIMSIZEMINIMAP+j] = _toblack_[minimapcolor+256*SHADOWFORCLOSEDCELLMAP];
	    }
	}
    }
    //put objects under fog if any
    for (ii=0,i=bh-2;i<=eh;i++,ii++)
    {
	if (i<0)
	    continue;
	if (i>=MAXYMAP)
	    break;
	for (jj=0,j=bw-2;j<=ew;j++,jj++)
	{
	    if (j<0)
		continue;
	    if (j>=MAXXMAP)
		break;

	    see=mapSEE(j,i);
	    open=mapOPEN(j,i);
	    if (see<=1&&open>1)
		method=0;
	    else
		if (see<=1 && open<1)
	    	    if (gameconf.videoconf.visiblemap)
			method=2;
		    else
			method=1;
		else
		    method=1;
	    switch(method)
	    {
		case 0:
		case 2:
		    if (map.mapbits.underfog[NUMBGAMER])
			if (map.mapbits.underfog[NUMBGAMER][i*MAXXMAP+j])
		    	    if (map.mapbits.underfog[NUMBGAMER][i*MAXXMAP+j]->nrofslots)
				loadandputimage(i*MAXXMAP+j);
		    break;
		case 1:
		    if (map.mapbits.underfog[NUMBGAMER])
			if (map.mapbits.underfog[NUMBGAMER][i*MAXXMAP+j])
			{
			    map.mapbits.underfog[NUMBGAMER][i*MAXXMAP+j]->nrofslots=0;
//			    wfree(map.mapbits.underfog[NUMBGAMER][i*MAXXMAP+j]);
//			    map.mapbits.underfog[NUMBGAMER][i*MAXXMAP+j]=NULL;
			}
		    break;
	    }
	}
    }
    
/*    FILE *f=fopen("screenmapused.txt","w");
    if (f)
    {
	for (i=0;i<hightkart+1;i++)
	{
	    for (j=0;j<widthkart+1;j++)
	    {
		fprintf(f,"%02d ",screenmapused[i*(widthkart+1)+j]);
	    }
	    fprintf(f,"\n");
	}
	fclose(f);
    }
*/
}
//==================================
#define MAPMOVETIMES		4
void AutoMoveMap(void)
{
    if (map.flags & STARMAP_FLAG_AUTOMOVE)
    {
	int mapmovedone=0;
	map.flags |= STARMAP_FLAG_MAPMOVE;
	map.totaldeltax -= map.deltax;
	map.totaldeltay -= map.deltay;
//	if (!mapmovedone)
	    if (map.deltax < 0)
	    {
		if (map.totaldeltax >= 0)
		    mapmovedone = 1;
	    }
	    else 
		if (map.deltax > 0)
		{
		    if (map.totaldeltax <= 0)
			mapmovedone = 1;
		}
	if (!mapmovedone)
	    if (map.deltay < 0)
	    {
		if (map.totaldeltay >= 0)
		    mapmovedone = 1;
	    }
	    else 
		if (map.deltay > 0)
		{
		    if (map.totaldeltay <= 0)
			mapmovedone = 1;
		}
	if (mapmovedone)
	{
	    //finish automove
	    map.flags &= ~STARMAP_FLAG_AUTOMOVE;
	    map.MAPX = map.newx / SIZESPRLANSHX;
	    map.MAPY = map.newy / SIZESPRLANSHY;
	    map.MAPXGLOBAL = map.MAPX * SIZESPRLANSHX;
	    map.MAPYGLOBAL = map.MAPY * SIZESPRLANSHY;
	}
	else
	{
	    map.MAPXGLOBAL += map.deltax;
	    map.MAPYGLOBAL += map.deltay;
	    map.MAPX = map.MAPXGLOBAL / SIZESPRLANSHX;
	    map.MAPY = map.MAPYGLOBAL / SIZESPRLANSHY;
	}
    }
}
//==================================
void MoveVisualMapPosition(int x,int y)
{
    int deltax,deltay;
    if (x<0)
	x=0;
    if (y<0) 
        y=0;
    if (x>SIZESPRLANSHX*(MAXXMAP-widthkart))
        x=SIZESPRLANSHX*(MAXXMAP-widthkart);
    if (y>SIZESPRLANSHY*(MAXYMAP-hightkart))
        y=SIZESPRLANSHY*(MAXYMAP-hightkart);
    map.newx = x;
    map.newy = y;
    map.totaldeltax = x - map.MAPXGLOBAL;
    map.totaldeltay = y - map.MAPYGLOBAL;
    map.deltax = map.totaldeltax / MAPMOVETIMES;
    map.deltay = map.totaldeltay / MAPMOVETIMES;
    if (map.totaldeltax || map.totaldeltay)
    {
	map.flags |= STARMAP_FLAG_AUTOMOVE;
    }
}
//==================================
void MoveVisualMapPositionCenter(int x,int y)
{
    MoveVisualMapPosition(x-widthkart*SIZESPRLANSHX/2,y-hightkart*SIZESPRLANSHY/2-SIZESPRLANSHY);
}
//==================================
int SetVisualMapPosition(int x,int y)//x,y-0..MAX?MAP*32
{
    int retval=0;
    if (map.flags & STARMAP_FLAG_AUTOMOVE)
	return(0);
    if (x<0) 
	x=0;
    if (y<0) 
        y=0;
    if (x>SIZESPRLANSHX*(MAXXMAP-widthkart))
        x=SIZESPRLANSHX*(MAXXMAP-widthkart);
    if (y>SIZESPRLANSHY*(MAXYMAP-hightkart))
        y=SIZESPRLANSHY*(MAXYMAP-hightkart);
    map.MAPXGLOBAL = x;
    map.MAPYGLOBAL = y;
    if (map.MAPX != x/SIZESPRLANSHX || map.MAPY != y/SIZESPRLANSHX)
    {
	map.flags |= STARMAP_FLAG_MAPMOVE;
	retval=1;
	map.MAPX = x/SIZESPRLANSHX;
	map.MAPY = y/SIZESPRLANSHY;
	map.newx = x;
	map.newy = y;
    }
    return(retval);
}
//==================================
void SetVisualMapPositionCenter(int x,int y)
{
    SetVisualMapPosition(x-widthkart*SIZESPRLANSHX/2,y-hightkart*SIZESPRLANSHY/2-SIZESPRLANSHY);
}
//==================================
void putfullcreep(int xglob,int yglob,int x,int y,int creepnrspr)
{
    char *adrspr=map.creeptiles+creepnrspr*32*32;
    putspr32x32(xglob,yglob,adrspr);
}
//==================================
//save tile for later use
void savemaptileadr(int x,int y,int indextile32)
{
    if (map.mapbits.savedtile[NUMBGAMER])
	map.mapbits.savedtile[NUMBGAMER][y*MAXXMAP+x]=indextile32;
}
//==================================
//save creepnr for later use
void savemapcreepadr(int x,int y,int creepnr)
{
    if (map.mapbits.savedcreep[NUMBGAMER])
	map.mapbits.savedcreep[NUMBGAMER][y*MAXXMAP+x]=creepnr;
}
//==================================
//risovati lanshaft
void putlansh(int xglob,int yglob,int x,int y,int indextile32,char wfog,char bfog)
{
    signed char creepnr;
//    if (indextile32==1)
//	return;
    char *adrspr=&map.tiles[indextile32*32*32];
    int nosave=0;
    creepnr = GetCreepAroundWithFog(x,y,indextile32);	    //get creep picture with fog and without fog
    if (wfog<=0)//if we see it before,but now fog we put saved pictures
    {
	if (map.mapbits.savedtile[NUMBGAMER])
	    if (map.mapbits.savedtile[NUMBGAMER][y*MAXXMAP+x]!=0xffff)
		indextile32=map.mapbits.savedtile[NUMBGAMER][y*MAXXMAP+x];//get previous tile
	nosave=1;
    }
    if (creepnr==-1)
    {
        putspr32x32(xglob,yglob,adrspr);
	if (!nosave)
    	    savemaptileadr(x,y,indextile32);
    }
    else
    {
        if (creepnr<MAXCREEPNR)
        {
    	    putspr32x32(xglob,yglob,adrspr);
	    GRP_gr_gamernr = 0;
	    putgrp(xglob,yglob,creepgrp,creepnr);
	}
	else
	{
	    putfullcreep(xglob,yglob,x,y,creepnr-MAXCREEPNR);
	}
    }
    if (!nosave)
	savemapcreepadr(x,y,map.creepflagplace[y*MAXXMAP+x]);
}
//==================================
void clearopenseeKarta(void)
{
    int i;
    memcpy(map.mapbits.seedetector,map.mapbits.seedetector2,MAXXMAP*MAXYMAP);
    memcpy(map.mapbits.arbiterfield,map.mapbits.arbiterfield2,MAXXMAP*MAXYMAP);

    
    for (i=0;i<PLAYEDPLAYERS;i++)
    {
    	if (IfPlayerHaveStartLocation(&map,i)!=-1)
	{
	    memcpy(map.mapbits.whitefog[i],map.mapbits.whitefog2[i],MAXXMAP*MAXYMAP);
	}
    }
    memset(map.mapbits.seedetector2,0,MAXXMAP*MAXYMAP);
    memset(map.mapbits.arbiterfield2,0,MAXXMAP*MAXYMAP);
    if (CODEFORSCREEN)
    {
    }
    else
    {
	for (i=0;i<PLAYEDPLAYERS;i++)
	{
    	    if (IfPlayerHaveStartLocation(&map,i)!=-1)
	    {
		memset(map.mapbits.whitefog2[i],0,MAXXMAP*MAXYMAP);
	    }
	}
    }
}
//=====================================
//objects who moved and need to reveal map for them
//=====================================
void AddObjsRevealMap(void)
{
    OBJ *a;
    OBJstruct *b;
    int i;
//    printf("%d\n",MaxObjsRevealMap);
    for (i=0;i<MaxObjsRevealMap;i++)
    {
        a=ObjsRevealMap[i];
    	b=loadobj(a->SC_Unit);
    	makeoneobjseeopen(a,b);		//open map
    }
    MaxObjsRevealMap=0;
}
//=====================================
void makeopenseeKarta(int beginobj,int endobj)
{
    OBJ *a;
    OBJstruct *b;
    int i;
    if (!MaxObjects)
	return;
    if (beginobj==-1||endobj==-1)
	return;
    for (i=beginobj;i<=endobj;i++)
    {
        a=objects[i];
    	b=loadobj(a->SC_Unit);
        if (GetUnitSightRange(a,b)>0)
	{
    	    makeoneobjseeopen(a,b);
	    arbitermakewarpfield(a,b);
	}
    }//for
    for (i=beginobj;i<=endobj;i++)
    {
	a=objects[i];
//	if (IsPermanentCloak(a->SC_Unit))
//	    continue;
        if (!IsBuild(a->SC_Unit))
               if (a->SC_Unit!=SC_ARBITEROBJ&&a->SC_Unit!=SC_HERO_DANIMOTHOBJ)
               {
		  if (a->playernr<PLAYEDPLAYERS)
                  if (map.mapbits.arbiterfield[a->ykart*MAXXMAP+a->xkart]&(1<<a->playernr))
                  {
                        if (GetMageAtr(&a->atrobj,ATRINVISIBLE)<=MAXATRMAGE)
			{
                    	    if (GetMageAtr(&a->atrobj,ATRINVISIBLE)>0)
                                //esli cloakwrite pod arbiterom
                    		addmage(a,ATRINVISIBLE,WRAITHEPODARBITEROM);
                            else
                                //esli drug iuniti pod arbiterom
                    		addmage(a,ATRINVISIBLE,NEWRAITHEPODARBITEROM);
			}
                  }
                  else
                         if (GetMageAtr(&a->atrobj,ATRINVISIBLE)==NEWRAITHEPODARBITEROM)
                             addmage(a,ATRINVISIBLE,0);//prostoi obj pod arbit
                         else
                             if (GetMageAtr(&a->atrobj,ATRINVISIBLE)==WRAITHEPODARBITEROM)
                                 addmage(a,ATRINVISIBLE,a->mana);//writhe clockanii
                             else
                                if (GetMageAtr(&a->atrobj,ATRINVISIBLE)>MAXATRMAGE&&
                                    GetMageAtr(&a->atrobj,ATRINVISIBLE)<ATRMAGEINFINITE)
                                    addmage(a,ATRINVISIBLE,0);//others obj
               }
    }//for
}
//=================================
char xypos[2][4]={{0,1,0,1},{0,0,1,1}};
//=================================
void arbitermakewarpfield(OBJ *a,OBJstruct *b)
{
    int x,y,i,j,mybits,xcenter,ycenter,see,objxysize;
    if (a->SC_Unit!=SC_ARBITEROBJ&&a->SC_Unit!=SC_HERO_DANIMOTHOBJ)
	return;
    if (a->prop&VARNOTWORK)
	return;
    if (GetMageAtr(&a->atrobj,ATRHALLUCINATION)!=0)
	return;
    if (PLAYER[a->playernr].isobserverflag)
	return;
    see=GetCloakRange(a,b);
    objxysize = GetRangeObjSize(a->SC_Unit,NULL,NULL);

    xcenter=a->xkart-xypos[0][objxysize];
    ycenter=a->ykart-xypos[1][objxysize];
//    xcenter=a->xkart;
//    ycenter=a->ykart;
    mybits=1<<a->playernr;

    for (i=0;i<MAXVISY;i++)
    {
	y=ycenter+i-MIDY;
	if (y<0)
	    continue;
	if (y>=MAXYMAP)
	    break;
	for (j=0;j<MAXVISX;j++)
	{
	    if (MAPvision[objxysize][i][j]<=see)
	    {
		x=xcenter+j-MIDX;
		if (x<0)
		    continue;
		if (x>=MAXXMAP)
		    break;
		map.mapbits.arbiterfield[y*MAXXMAP+x]|=mybits;
		map.mapbits.arbiterfield2[y*MAXXMAP+x]|=mybits;
	    }
	}
    }
}
//=================================
void makeoneobjseeopen(OBJ *a,OBJstruct *b)
{
    int x,y,opendelta,mapchanges=0,objxysize,pl;
    int i,j,k,pp,see,prst=0,opn,nrofuplevel,objsee,onsky=0;
    int xcenter,ycenter,level;

    int nosave,indextile32;
    signed char creepnr;

    if (IsSubUnit(a->SC_Unit))
	return;
    if (a->prop & VARNOTHERE)
	return;
    if (a->prop&VARKARTCHANGES)
	a->prop&=~VARKARTCHANGES;
    else
	return;
    if (a->in_transport)//in buncker/transport
	return;
    if (IsSubUnit(a->SC_Unit))
	return;		//subunit not reveal the map
    if (a->playernr>=PLAYEDPLAYERS)		//this is neutral unit
	return;
    if (!IsOBJUnderConstruct(a))
    {
        prst=GetMageAtr(&a->atrobj,ATRPARASITEFROM);
        see=GetUnitSightRange(a,b)+2;
    }
    else
	see=SEERANGEIFNOTREADY;
    if (IsOnSkyOBJ(a))
	onsky=1;

    objxysize = GetRangeObjSize(a->SC_Unit,NULL,NULL);

    xcenter=a->xkart-xypos[0][objxysize];
    ycenter=a->ykart-xypos[1][objxysize];

//    level=map.maplevel[ycenter*MAXXMAP+xcenter];
    level=map.maplevel[a->ykart*MAXXMAP+a->xkart];
    opn=GetVisionBitsPlayer(a->playernr);
    if (!opn && !prst)
	return;

    for (i=0;i<MAXVISY;i++)
    {
	y=ycenter+i-MIDY;
	if (y<0)
	    continue;
	if (y>=MAXYMAP)
	    break;
	for (j=0;j<MAXVISX;j++)
	{
	    opendelta=see-MAPvision[objxysize][i][j]+1;
	    if (opendelta<1)
		continue;
	    if (opendelta>4)
		opendelta=4;
	    x=xcenter+j-MIDX;
	    if (x<0)
		continue;
	    if (x>=MAXXMAP)
		break;
	    if (map.maplevel[y*MAXXMAP+x]<=level)
	    {
	        objsee=1;
	    }
	    else
	        objsee=0;
	    if (objsee||onsky)
	    {
        	    if (!CODEFORSCREEN)
        	    {
//			pp=force_slots.playernr[a->playernr];
			pp=a->playernr;
            		if (opn&(1<<pp))
            		{
				if (mapSEE(x,y,pp)<opendelta)
				{
				    map.mapbits.whitefog[pp][y*MAXXMAP+x]=opendelta;
				    mapchanges=1;
				}
				if (mapSEE2(x,y,pp)<opendelta)
				{
				    map.mapbits.whitefog2[pp][y*MAXXMAP+x]=opendelta;
				    mapchanges=1;
				}
			}
			if (prst)
			{
			    for (pp=0;pp<PLAYEDPLAYERS;pp++)
			    {
				if (prst&(1<<pp))
				{
				    if (mapSEE(x,y,pp)<opendelta)
				    {
					map.mapbits.whitefog[pp][y*MAXXMAP+x]=opendelta;
					mapchanges=1;
				    }
				    if (mapSEE2(x,y,pp)<opendelta)
				    {
					map.mapbits.whitefog2[pp][y*MAXXMAP+x]=opendelta;
					mapchanges=1;
				    }
				}
			    }
			}
			if (opendelta==1)
			    continue;
                    	a->visibleby=opn;
			if (opn)
			{
            		}
        	    }
        	    if ((!(a->prop&VARNOTWORK))&&!IsOBJUnderConstruct(a)&&(a->prop|VARPOWEROFF))//esli dvigaetsea
                	if (IsDetector(a->SC_Unit)&&GetMageAtr(&a->atrobj,ATRBLIND)==0)
			{
                        	if  (opn)//esli alienseobj|parasite
				{
				    map.mapbits.seedetector2[y*MAXXMAP+x]|=opn;
				    map.mapbits.seedetector[y*MAXXMAP+x]|=opn;
				}
			}
    		    nosave=0;
    		    if (mapchanges && a->playernr == NUMBGAMER)
    		    {
    			indextile32=map.display_map[y*MAXXMAP+x];
			creepnr = GetCreepAroundWithFog(x,y,indextile32);
			if (creepnr==-1)
			{
    			    savemaptileadr(x,y,indextile32);
			}
			savemapcreepadr(x,y,map.creepflagplace[y*MAXXMAP+x]);
		    }

	    }
	}
    }
    if (mapchanges)
	map.clearfog[a->playernr]=1;
}
//=================================
void opentempmap(int playernr,int xkart,int ykart,int sizex,int sizey)
{
    int i,j,fromx,fromy,tox,toy;
    if (playernr>=PLAYEDPLAYERS || !map.mapbits.whitefog )
	return;
    fromx = xkart - sizex/2;
    if (fromx<0)
	fromx=0;
    fromy = ykart - sizey/2;
    if (fromy<0)
	fromy=0;
    tox = xkart + sizex/2;
    if (tox>=MAXXMAP)
	tox=MAXXMAP-1;
    toy = ykart + sizey/2;
    if (toy>=MAXYMAP)
	toy=MAXYMAP-1;
    for (i=fromy;i<=toy;i++)
	for (j=fromx;j<=tox;j++)
	{
	    map.mapbits.whitefog[playernr][i*MAXXMAP+j]=4;
    	    map.mapbits.whitefog2[playernr][i*MAXXMAP+j]=4;
    	}
}
//=================================
void calcfullinvandsee(void)
{
    for (int i=0;i<MaxObjects;i++)
    {
        calcfullinvandseeobj(objects[i]);
    }
}
//=================================
void calcfullinvandseeobj(OBJ *a)
{
    int i,decloaked,cloaked,notdetect=0;
    unsigned char visbits,plmask;
    decloaked = existatrdecloak(a);
    if (IsCloaked(a) || IsOBJBurrowed(a))
	cloaked=1;
    else
	cloaked=0;
    for (i=0,plmask=1;i<PLAYEDPLAYERS;i++,plmask<<=1)
    {
	if (cloaked)
    	    OBJ_VAR_MASK_SET(a,obj_notdetect,plmask);
    	else
    	    OBJ_VAR_MASK_CLR(a,obj_notdetect,plmask);
        if (map.pl_vision[i][a->playernr]			||
    	    player_aliance(i,a->playernr) == ALLIANCEOBJ	||
    	    existparasitebit(a,i))
        {
    	    OBJ_VAR_MASK_SET(a,obj_invsee,plmask);
    	    OBJ_VAR_MASK_SET(a,obj_see,plmask);
    	    OBJ_VAR_MASK_CLR(a,obj_notdetect,plmask);
        }
        else
        {
    	    OBJ_VAR_MASK_CLR(a,obj_invsee,plmask);
    	    OBJ_VAR_MASK_CLR(a,obj_see,plmask);
	    if (mapSEE(a->xkart,a->ykart,i)>1)
    		OBJ_VAR_MASK_SET(a,obj_see,plmask);
	    visbits = GetVisionBitsPlayer(i);
    	    if (map.mapbits.seedetector[a->ykart*MAXXMAP+a->xkart] & visbits)
    	    {//esli vidim detectorom
    		OBJ_VAR_MASK_SET(a,obj_invsee,plmask);
    		OBJ_VAR_MASK_CLR(a,obj_notdetect,plmask);
    	    }
	}
    	if (OBJ_VAR_MASK_CHK(a,obj_notdetect,plmask))
        {
	    if (decloaked)
    		OBJ_VAR_MASK_CLR(a,obj_notdetect,plmask);
    	}
    }//for
    if (cloaked)
	ClearFinalOBJ(a);
}
//==================================
void ClearFinalOBJ(OBJ *a0)
{
    OBJ *a;
    int i=0;
    for (i=0;i<MaxObjects;i++)
    {
	a = objects[i];
	if (a0 == a->finalOBJ)
	{
    	    if (OBJ_VAR_CHK(a0,obj_notdetect,a->playernr))
		moveobj(a,NULL,MODESTOP,0,0,NOSHOWERROR,0);
	}
    }
}
//==================================
void _putcells(void)
{
    int i,j,xkk,ykk;
    xkk=map.MAPXGLOBAL%SIZESPRLANSHX;
    ykk=map.MAPYGLOBAL%SIZESPRLANSHY;
    for (i=0;i<hightkart+1;i++)
    {
	horizline(245,GRP_wminx,GRP_wmaxx,i*SIZESPRLANSHY-ykk);
    }
    for (j=0;j<widthkart+1;j++)
    {
	vertline(245,j*SIZESPRLANSHX-xkk,GRP_wminy,GRP_wmaxy);
    }
}
//==================================
int getplayerrace(struct mapinfo *map,int playernr)
{
    int ret,i;
    ret = map->pl_race[playernr];
    for (i=0;i<3;i++)
    {
	xicon[i] = iconxcoord[ret][i];
	yicon[i] = iconycoord[ret][i];
    }
    return ret;
}
//=============================================
int mapSEE(OBJ *a,int player)
{
    if (!map.mapbits.whitefog[player])
	return(0);
    return(map.mapbits.whitefog[player][a->ykart*MAXXMAP+a->xkart]);
}
//=============================================
int mapSEE(int xkart,int ykart,int player)
{
    if (!map.mapbits.whitefog[player])
	return 0;
    if (ykart<0||xkart<0||ykart>=MAXYMAP||xkart>=MAXXMAP)
	return 0;
    return(map.mapbits.whitefog[player][ykart*MAXXMAP+xkart]);
}
//=============================================
int mapSEE2(int xkart,int ykart,int player)
{
    if (!map.mapbits.whitefog2[player])
	return 0;
    if (ykart<0||xkart<0||ykart>=MAXYMAP||xkart>=MAXXMAP)
	return 0;
    return(map.mapbits.whitefog2[player][ykart*MAXXMAP+xkart]);
}
//=============================================
int mapOPEN(OBJ *a,int player)
{
    if (!map.mapbits.blackfog[player])
	return 0;
    return(map.mapbits.blackfog[player][a->ykart*MAXXMAP+a->xkart]);
}
//=============================================
int mapOPEN(int xkart,int ykart,int player)
{
    if (!map.mapbits.blackfog[player])
	return 0;
    if (ykart<0||xkart<0||ykart>=MAXYMAP||xkart>=MAXXMAP)
	return 0;
    return(map.mapbits.blackfog[player][ykart*MAXXMAP+xkart]);
}
//=============================================
int mapSEE(int xkart,int ykart)
{
    int i;
    unsigned char fogvalue=0;
    if (ykart<0||xkart<0||ykart>=MAXYMAP||xkart>=MAXXMAP)
	return 0;
    for (i=0;i<PLAYEDPLAYERS;i++)
    {
    	if (IfPlayerHaveStartLocation(&map,i)!=-1)
	{
	    if (player_vision(NUMBGAMER,i))
		if (fogvalue<map.mapbits.whitefog[i][ykart*MAXXMAP+xkart])
		    fogvalue=map.mapbits.whitefog[i][ykart*MAXXMAP+xkart];
	}
    }
    return(fogvalue);
}
//=============================================
int mapOPEN(int xkart,int ykart)
{
    int i;
    unsigned char fogvalue=0;
    if (ykart<0||xkart<0||ykart>=MAXYMAP||xkart>=MAXXMAP)
	return 0;
    for (i=0;i<PLAYEDPLAYERS;i++)
    {
	if (IfPlayerHaveStartLocation(&map,i)!=-1)
	{
	    if (player_vision(NUMBGAMER,i))
		if (fogvalue<map.mapbits.blackfog[i][ykart*MAXXMAP+xkart])
		    fogvalue=map.mapbits.blackfog[i][ykart*MAXXMAP+xkart];
	}
    }
    return(fogvalue);
}
//=============================================
int mapEFFECT(int xkart,int ykart,int effectmage)
{
    return(map.mapbits.mageeffect[effectmage][ykart*MAXXMAP+xkart]);
}
//=============================================
int CreateAlianceBitsPlayer(int playernr)
{
    int i,allbits=0;
    for (i=0;i<PLAYEDPLAYERS;i++)
    {
	if (IfPlayerHaveStartLocation(&map,i)!=-1)
	{
	    if (player_aliance(playernr,i)==PLAYER_VISION)
		allbits|=(1<<i);
	}
    }
    return(allbits);
}
//================================================
int GetVisionBitsPlayer(int playernr)
{
    return (map.pl_visionbits[playernr]);
}
//================================================
int player_aliance(int whopl,int pl)
{
    return (map.pl_allied[whopl][pl]);
	//0-enemy,1-neutral,2-alience,3-my
}
//==================================
int player_vision(int whopl,int pl)
{
    return (map.pl_vision[whopl][pl]);
}
//==================================
int GetMapFog(int x,int y)
{
    if (mapSEE(x/32,y/32)>1)
	return(2);
    else
	if (mapOPEN(x/32,y/32)>1)
	    return(1);
        else
	    return(0);
}
//==============================================
