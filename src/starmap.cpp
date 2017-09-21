#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "vars.h"
#include "auxil.h"
#include "mpq.h"
#include "objs.h"
#include "mylist.h"
#include "pylon.h"
#include "gener.h"
#include "doodad.h"
#include "stars.h"
#include "load.h"
#include "lists.h"
#include "mytime.h"
#include "tempobj.h"
#include "gamemenu.h"
#include "rand.h"
#include "flingy.h"
#include "stringfiles.h"
#include "debug.h"
#include "triggers.h"
#include "briefing.h"
#include "fog.h"
#include "iscript.h"
#include "starmap.h"

#ifdef WITHSDL
        #include "sdl/grsdl.h"
#endif

#include <grplib/gr8.h>
//=================================================
struct starmap_section
{
  char name[4];
  int  size;
};

//=================================================
#define MAXGROUPS 3000
#define MAXTILES8 100000

char terraintypes[MAXTERRAINTYPES][10] = 
{
	"Badlands",
	"Space",
	"Install",
	"Ashworld",
	"Jungle",
	"Desert",
	"Arctic",
	"Twilight"
};
//=====================================================
void add_unit_stat(struct mapinfo *info,int stattype,int playernr,int SC_Unit)
{
    if (TRIG_ChangeStat)
    {
	if (playernr<MAXPLAYERS&&SC_Unit<MAX_UNITS_ELEM)
        {
	    info->units_stat.stattype[stattype][playernr][SC_Unit]++;
	    info->units_stat.stattype[stattype][playernr][SC_EVERYUNIT]++;
	    if (IsGroupMenFlag(SC_Unit))
		info->units_stat.stattype[stattype][playernr][SC_MENUNIT]++;
	    if (IsGroupBuildFlag(SC_Unit))
		info->units_stat.stattype[stattype][playernr][SC_BUILDUNIT]++;
	    if (IsGroupFactoryFlag(SC_Unit))
		info->units_stat.stattype[stattype][playernr][SC_FACTORYUNIT]++;
	}
	else
	    DEBUGMES("!!!!!!! stattype=%d player=%d unit=%d\n",stattype,playernr,SC_Unit);
    }
}
//=====================================================
void sub_unit_stat(struct mapinfo *info,int stattype,int playernr,int SC_Unit)
{
    if (TRIG_ChangeStat)
    {
        if (playernr<MAXPLAYERS&&SC_Unit<MAX_UNITS_ELEM)
	{
	    info->units_stat.stattype[stattype][playernr][SC_Unit]--;
	    info->units_stat.stattype[stattype][playernr][SC_EVERYUNIT]--;
	    if (IsGroupMenFlag(SC_Unit))
		info->units_stat.stattype[stattype][playernr][SC_MENUNIT]--;
	    if (IsGroupBuildFlag(SC_Unit))
		info->units_stat.stattype[stattype][playernr][SC_BUILDUNIT]--;
	    if (IsGroupFactoryFlag(SC_Unit))
		info->units_stat.stattype[stattype][playernr][SC_FACTORYUNIT]--;
	}
	else
	    DEBUGMES("!!!!!!! stattype=%d player=%d unit=%d\n",stattype,playernr,SC_Unit);
    }
}
//=====================================================
//=====================================================
//=====================================================
//=====================================================
//=====================================================
int load_tileset_palette( HANDLE mpq,char *filename, char *pal )
{
	HANDLE f;
	UINT readed;
	
	int err = wr_SFileOpenFileEx(mpq,filename,SFILE_OPEN_FROM_MPQ,&f);
	if ( !err )
	    return -1;
	
	wr_SFileReadFile(f,pal,1024,&readed,NULL);
	wr_SFileCloseFile(f);
	return 0;
}
//=====================================================
int load_tiles8( HANDLE mpq,char *filename, char *tiles )
{
	HANDLE f;
	int tc = 0;
	UINT readed;
	char *p = tiles;

	int err = wr_SFileOpenFileEx(mpq,filename,SFILE_OPEN_FROM_MPQ,&f);
	if ( !err )
	    return -1;
	
	while( !SFileEOF(f) )
	{
		wr_SFileReadFile(f,p,64,&readed,NULL);
		if( readed==64 && ++tc < MAXTILES8 )
		{
			p += 64;	
		}
	}
	wr_SFileCloseFile(f);
	return 0;
}
//=====================================================
int load_tiles32(HANDLE mpq, char *filename, char *tiles8,
		  char **tilesret, unsigned short *megatiles,int nrmegatiles )
{
	HANDLE f;
	char *p = tiles8;
	UINT sz,readed;
	char *tiles32;
	int i,j,k;
	unsigned short tile8idx;
	char *q;
	int count, x,y;

	*tilesret = 0;
	int curentmegatile=0;

	tiles32 = (char *)wmalloc( nrmegatiles*32*32 );
	for( i=0; i<nrmegatiles; i++ )
	{
		q = tiles32 + i*1024;
		for( j=0; j<4; j++ )
		{
			for( k=0; k<4; k++ )
			{
				tile8idx=megatiles[curentmegatile++];
				p = tiles8 + (tile8idx>>1)*64;
				if( tile8idx & 1 )
				for( x=0; x<8; x++ )
					for( y=0; y<8; y++ )
					{
						q[j*256+k*8 + y*32 + x] = p[ y*8 + 7-x ];
					}
				else
				for( x=0; x<8; x++ )
					for( y=0; y<8; y++ )
					{
						q[j*256+k*8 + y*32 + x] = p[ y*8 + x ];
					}
			}
		}
	}
	*tilesret = tiles32;
	return 0;
}

//=====================================================
unsigned short *loadvx4(HANDLE mpq, char *filename,int *size)
{
    HANDLE f;
    unsigned short *megatiles;
    UINT sz,readed;
    int err = wr_SFileOpenFileEx(mpq,filename,SFILE_OPEN_FROM_MPQ,&f);
    if ( !err )
        return NULL;
    sz = SFileGetFileSize(f,NULL);
    megatiles=(unsigned short  *)wmalloc(sz);
    wr_SFileReadFile(f,megatiles,sz,&readed,NULL);
    wr_SFileCloseFile(f);
    *size=sz/32;
    return(megatiles);
}
//=====================================================
unsigned short *loadvf4(HANDLE mpq, char *filename,int *size)
{
    HANDLE f;
    unsigned short *walktiles;
    UINT sz,readed;
    int err = wr_SFileOpenFileEx(mpq,filename,SFILE_OPEN_FROM_MPQ,&f);
    if ( !err )
        return NULL;
    sz = SFileGetFileSize(f,NULL);
    walktiles=(unsigned short *)wmalloc(sz);
    wr_SFileReadFile(f,walktiles,sz,&readed,NULL);
    wr_SFileCloseFile(f);
    *size=sz/32;
    return(walktiles);
}
//=====================================================
int load_header(HANDLE mpq, char *filename, unsigned short *groups , terrain_cv5 *conf)
{
	HANDLE f;
	int gc = 0;
	unsigned short *p;
	terrain_cv5 *c;
	UINT readed;

	int err = wr_SFileOpenFileEx(mpq,filename,SFILE_OPEN_FROM_MPQ,&f);
	if ( !err )
	    return -1;
	p=groups;
	c=conf;
	while( !SFileEOF(f) )
	{
	    wr_SFileReadFile(f,c,20,&readed,NULL);
	    if(readed==20 && ++gc < MAXGROUPS)
	    {
	        c++;
	        wr_SFileReadFile(f,p,32,&readed,NULL);
	        if( readed==32)
	        {
	    	    p+=16;
		}
	    }
	}
	wr_SFileCloseFile(f);
	return(gc);
}
//=====================================================
/* map smaller than minimap in terms of tiles */
void prepare_minimap_inverse( unsigned short *map, int w, int h, char *minimap,
	char *tiles )
{
	int i,j;
	int xstart, ystart;
	int xend, yend;
	float rate = w>h ? MINIMAPW/(float)w : MINIMAPW/(float)h;
	float ratei = 1/rate;

	xstart = (int)(h>w ? (h-w)*rate/2 : 0);
	ystart = (int)(w>h ? (w-h)*rate/2 : 0);
	xend  = (int)(xstart ? (h+w)*rate/2 : MINIMAPW);
	yend  = (int)(ystart ? (h+w)*rate/2 : MINIMAPW);

	if( rate == 2 ) /* 64x64 */
	for( i=0; i<h; i++ )
	{
		for( j=0; j<w; j++ )
		{
			minimap[i*2*MINIMAPW+j*2] = tiles[ map[i*w+j]*1024 ];
			minimap[i*2*MINIMAPW+j*2+1] = tiles[ map[i*w+j]*1024+32 ];
			minimap[(i*2+1)*MINIMAPW+j*2] = tiles[ map[i*w+j]*1024+512];
			minimap[(i*2+1)*MINIMAPW+j*2+1] = tiles[ map[i*w+j]*1024+512+32 ];
		}
	}
	else	/* 96x */
	for( i=0; i<MINIMAPW; i++ )
	{
		if( i < ystart || i > yend )
			memset( minimap+i*MINIMAPW, 0, MINIMAPW );
		else
		for( j=0; j<MINIMAPW; j++ )
		{
			if( j < xstart || j > xend )
				minimap[i*MINIMAPW+j] = 0;
			else
			minimap[i*MINIMAPW+j] =
					tiles[ map[((int)((i-ystart)*ratei))*w+
					((int)((j-xstart)*ratei))]*1024 ];
		}
	}
}
//=================================================
void prepare_minimap( unsigned short *map, int w, int h, char *minimap,
	char *tiles, int *minx, int *miny, int *maxx, int *maxy )
{
	int i,j, mi,mj;
	int xoffs, xmax, yoffs, ymax;	/* for non-square map */
	float rate;
	
	xoffs = yoffs = 0;
	xmax = MINIMAPW;
	ymax = MINIMAPW;
	rate = w/(float)MINIMAPW;

	if( w > h )
	{
		rate = w/(float)MINIMAPW;
		yoffs = (int)((w-h)/(2*rate));
		ymax =  (int)((w+h)/(2*rate));
	}
	else if( h > w )
	{
		rate = h/(float)MINIMAPW;
		xoffs = (int)((h-w)/(2*rate));
		xmax =  (int)((h+w)/(2*rate));
	}

	*minx = xoffs;
	*miny = yoffs;
	*maxx = xmax;
	*maxy = ymax;

	if( rate < 1 )
		return prepare_minimap_inverse( map, w, h, minimap, tiles );

 	mi = 0;	
	for( i=0; i<h; )
	{
		if( mi > yoffs && (mi-yoffs)*rate > i )
		{
			i++;
			continue;
		}

		mj = 0;

		if( mi < yoffs || mi > ymax )
			memset( minimap + mi*MINIMAPW, 0, MINIMAPW );	// no map;
		else
		{
			for( j=0; j<w; )
			{
				if( mj > xoffs && (mj-xoffs)*rate > j )
				{
					j++;
					continue;
				}
	
				if( mj < xoffs || mj > xmax )
					minimap[mi*MINIMAPW+mj] = 0;	// no map
				else
				{
					minimap[mi*MINIMAPW+mj] = tiles[ map[i*w+j]*1024+16*32+16 ];
					j++;
				}
				mj++;
			}
			i++;
		}
		mi++;
	}
}

//=================================================
int load_terrain_tiles( struct mapinfo *info,
			int terrain, char **tiles, int *tilecount,
			char *outpal, unsigned short *map, walkflags32 **walkflags,
			int w, int h, char *minimap,
			int *minx, int *miny, int *maxx, int *maxy )
{
	int allcreeptiles[50];
	char buf[200];
	int begcreepnr=0,endcreepnr;
	int i,j,k,hconfflags;
	int last_tile,prev_walk;
	unsigned short *tile_ids, *rev_ids;
	HANDLE mpq;
	char *used_tiles;
	int err,vf4elemnr,nrmegatiles;
	char *tiles8;
	unsigned short *header,*megatiles;
	terrain_cv5 *headerconf;
	unsigned short *used_walkf,*used_tile32prop;
	terrain_cv5_doodad *doodadconf;

	header = (unsigned short *)calloc( 16*MAXGROUPS, sizeof(unsigned short) );
	headerconf = (terrain_cv5 *) calloc( 16*MAXGROUPS, sizeof(headerconf) );
	tiles8 = (char*  )calloc( 64*MAXTILES8, sizeof(char) );
	doodadconf=(terrain_cv5_doodad *)(headerconf+1024);

	assert( header );
	assert( tiles8 );

	// read terrain palette (RGBx format)
	sprintf( buf,"%s.wpe", palettedir[terrain] );
	mpq = MpqsFindFile(buf);
	if (!mpq)
	    return -2;
	err = load_tileset_palette(mpq,buf, outpal );
	if (err)
	    return -3;
	
	// read 8x8 tiles
	sprintf( buf, "%s.vr4",palettedir[terrain] );
	mpq = MpqsFindFile(buf);
	if (!mpq)
	    return -4;
	err = load_tiles8(mpq,buf, tiles8 );
	if (err)
	    return -5;
	// read 32x32 tiles
	sprintf( buf, "%s.vx4",palettedir[terrain] );
	mpq = MpqsFindFile(buf);
	if (!mpq)
	    return -6;
	megatiles=loadvx4(mpq,buf,&nrmegatiles);
	err = load_tiles32(mpq,buf, tiles8, tiles, megatiles,nrmegatiles);
	if (err)
	    return -7;
	// read walkable flags
	sprintf( buf, "%s.vf4",palettedir[terrain] );
	mpq = MpqsFindFile(buf);
	if (!mpq)
	    return -8;
	info->vf4mem=loadvf4(mpq,buf,&vf4elemnr);
	// read header
	sprintf( buf, "%s.cv5",palettedir[terrain] );
	mpq = MpqsFindFile(buf);
	if (!mpq)
	    return -8;
	int loadedcv5 = load_header(mpq,buf, header,headerconf );
	if (loadedcv5<0)
	    return -9;
	// replace display_map values with ones taken
	// from header
	tile_ids = (unsigned short*)calloc( 65536,2 );
	rev_ids = (unsigned short*)calloc( 65536,2 );
	assert( tile_ids );
	assert( rev_ids );
	last_tile=0;

	info->buildswalk = (unsigned char*)wmalloc(h*w);
    
	if (loadedcv5>1024)
	{
	    loadedcv5-=1024;
//	    printf("all doodads information=%d\n",loadedcv5);
	    loadedcv5=1024;
	}
//	printf("loaded megatiles vf5=%d\n",loadedcv5);
	for (i=0;i<loadedcv5/16;i++)
	{
	    hconfflags=headerconf[i].flags;
	    if (hconfflags&0x40)		//creep tile
	    {
//	        printf("creep line %03d with 16 megatiles\n",i);
//		printf("creep tiles referience = ");
		for (j=0;j<16;j++)
		{
//		    printf("[%d] ",header[i*16+j]);
		    if (header[i*16+j])
		    {
			if (!begcreepnr)
			    begcreepnr=header[i*16+j];
			allcreeptiles[info->creeptilecnt]=header[i*16+j];
			info->creeptilecnt++;
		    }
		}
//		printf("\n");
	    }
	}
//        printf("creepnrfrom %d with total creep tiles %d\n",begcreepnr,info->creeptilecnt);
	info->creeptiles = (char*)wmalloc(info->creeptilecnt*32*32);
	// copy creep tiles
	for (i=0;i<info->creeptilecnt;i++)
	{
	    memcpy(info->creeptiles+i*32*32,(*tiles)+allcreeptiles[i]*32*32,32*32);
	}
	info->creeptileminimapcolors=createminimapcolorsfrom(info->creeptiles,info->creeptilecnt);

	for( i=0; i<h; i++ )
	{
	    for( j=0; j<w; j++ )
	    {
		k = header[ map[i*w+j] ];
		hconfflags=headerconf[map[i*w+j]/16].flags;
		//set if tile32 is buildable & capable to grow creep
		info->buildswalk[i*w+j]=((hconfflags&0x0080)==0);
		//map[i*w+j] = k;
		//add used tiles to a list.
		if( !tile_ids[k] )
		{
		    tile_ids[k] = last_tile;
		    rev_ids[last_tile] = k;
		    last_tile++;
		}
		map[i*w+j] = tile_ids[k];
	    }
	}
	// leave only used tiles.
	used_tiles = (char*)wmalloc( last_tile*32*32 );
	used_walkf = (unsigned short*) wmalloc( last_tile*sizeof(unsigned short) );
	used_tile32prop = (unsigned short*) wmalloc( last_tile*sizeof(unsigned short)*16 );
	for( i=0; i<last_tile; i++ )
	{
		used_walkf[i]=0;
		memcpy( used_tiles+i*32*32, (*tiles)+rev_ids[i]*32*32, 32*32 );
		prev_walk=rev_ids[i];
    		for (j=0; j<16; j++ )
		{
        	    used_walkf[i] |= (info->vf4mem[prev_walk*16+j]&1)<<j;
		    used_tile32prop[i*16+j] = info->vf4mem[prev_walk*16+j];
		}
	}
	free( tile_ids );
	free( rev_ids );
	wfree( *tiles );
	wfree(info->vf4mem);
	info->vf4mem=used_tile32prop;
	free( header );
	free( tiles8 );
	free(headerconf);   //wfree cv5 file
	wfree(megatiles);   //wfree vx4 file
	*tiles = used_tiles;
	*walkflags = used_walkf;
	*tilecount = last_tile;
	
	prepare_minimap( map, w,h, minimap, *tiles,
		minx, miny, maxx, maxy );
	return 0;
}
//=================================================
#define MAXBLOCK (1024*1024)
//=================================================
mylist map_units,map_doodads;
int read_starmap(HANDLE mpq,char *filename, struct mapinfo *info,int flags )
{
	int i,j,err,testplay,trgfilesize;
	HANDLE f;
	UINT readed;
	struct starmap_section section;
	struct unit_on_map *unit;
	doodad_THG2_starmap tempdoodad;
	doodad_DD2_starmap tempdoodad2;
	doodad_on_map *doodadu;
	struct locations *loc;
        int maxlocations,startloc=0;
	int maxsectionsize=0,readyloc;
	char name[5];
	char *prevbuf=NULL,*buf=NULL;
	char *DD2_section=NULL;
	char *DD2_section2;
	int DD2_sectionsize,DD2_sectionsize2;

	char default_max_upgrade[MAX_UPGRADES_ELEM];
        char default_start_upgrade[MAX_UPGRADES_ELEM];
        char use_upgrdefaults[MAXPLAYERS][MAX_UPGRADES_ELEM];
	
	char default_tech_available[MAX_TECHDATA_ELEM];
	char default_tech_researched[MAX_TECHDATA_ELEM];
	char use_techdefaults[MAXPLAYERS][MAX_TECHDATA_ELEM];


	memset( name, 0, 5 );
	memset( info, 0, sizeof(struct mapinfo) );
	memset(info->pl_startlocplayernr,-1,MAXPLAYERS);

	testplay=flags&STARMAP_LOADFORINFO;

	err = wr_SFileOpenFileEx(NULL,filename,SFILE_OPEN_LOCAL_FILE,&f);
	if ( !err )
	{
	    if (!mpq)
	    {
		//propable we open campaign from install.exe, check if we opened installexe as mpq
		if (!install1mpq)
		{
		    if (!install2mpq)
		    {
			return(-1);
		    }
		    else
		    {
checkmpq2:
			err = wr_SFileOpenFileEx(install2mpq,filename,SFILE_OPEN_FROM_MPQ,&f);
			if ( !err )
			    return -1;
			info->mpqid = install2mpq;
			info->mpqclose = 0;
		    }
		}
		else
		{
		    err = wr_SFileOpenFileEx(install1mpq,filename,SFILE_OPEN_FROM_MPQ,&f);
		    if ( !err )
			goto checkmpq2;
		    info->mpqid = install1mpq;
		    info->mpqclose = 0;
		}
	    }
	    else
	    {
		err = wr_SFileOpenFileEx(mpq,filename,SFILE_OPEN_FROM_MPQ,&f);
		if ( !err )
		    return -1;
		info->mpqid = mpq;
		info->mpqclose = 1;
	    }
	}
	else
	{
    	    info->mpqid = NULL;//localfile
	    info->mpqclose = 0;
	}
	for (i=0;i<MAXPLAYERS;i++)
	    PLAYER[i].colorRACE=-1;
	while( !SFileEOF(f) )
	{
		wr_SFileReadFile(f,&section,sizeof(struct starmap_section),&readed,NULL);
		if (readed != sizeof(struct starmap_section))
		    break;
		strncpy( name, section.name, 4 );
		if( section.size <= 0 )
		    continue;
//		printf( "Section %s size (%d)\n",name, section.size );
		if( section.size > MAXBLOCK )
		{
//			printf( "Section %s too large (%d), skipping\n",
//				name, section.size );
			SFileSetFilePointer(f,section.size,NULL,FILE_CURRENT);
			continue;
		}
		if (section.size > maxsectionsize)
		{
		    maxsectionsize=section.size;
		    if (prevbuf)
			wfree(prevbuf);
		    prevbuf=(char *)wmalloc(section.size);
		}
		buf=prevbuf;
		wr_SFileReadFile(f,buf,section.size,&readed,NULL);
		if (section.size!=readed)
		{
		    return (-1);
		}
		if( !strcmp( name, "TYPE" ) )
		{
		    if (!strncmp(buf,"RAWB",4))
		        info->flags|=STARMAP_FLAG_EXPANSION;
		    else	
			if (strncmp(buf,"RAWS",4))
			    return(-1);
		}
		else if( !strcmp( name, "VER " ) )
		{
			info->format_version[0] = *((unsigned short *)buf);
		}
		else if( !strcmp( name, "IVER" ) )
		{
			info->format_version[1] = *((unsigned short *)buf);
		}
		else if( !strcmp( name, "IVE2" ) )
		{
			info->format_version[2] = *((unsigned short *)buf);
		}
		else if( !strcmp( name, "VCOD" ) )
		{
			/* TODO: fix this */
			info->valid_vcode = 1;
		}
		else if( !strcmp( name, "OWNR" ) )
		{
			memcpy( info->pl_owner, buf, 12 );
		}
		else if( !strcmp( name, "IOWN" ) )
		{
			memcpy( info->pl_iowner, buf, 12 );
		}
		else if( !strcmp( name, "ERA " ) )
		{
//			int t = *((int*)buf)&0xffff;
			int t = *((int*)buf)&0x7;
			info->terrain = (terrain_type)t;
		}
		else if( !strcmp( name, "DIM " ) )
		{
			info->map_width = *((int*)buf) & 65535;
			info->map_height = *((int*)(buf +2) ) & 65535;
//			printf("mapsize=%dx%d\n",info->map_width,info->map_height);
		}
		else if( !strcmp( name, "SIDE" ) )
		{
			memcpy( info->pl_race, buf, 12 );
		}

		else if( !strcmp( name, "FORC" ) )
		{
			memcpy( &info->pl_force, buf, section.size);
		}
		else if( !strcmp( name, "PUNI" ) )
		{
			if (!testplay)
			{
			    memcpy( info->prod_restrictions.player_restrictions, 
					buf, MAXPLAYERS*MAX_UNITS_ELEM );
			    memcpy( info->prod_restrictions.global_restrictions,
					buf+MAXPLAYERS*MAX_UNITS_ELEM , MAX_UNITS_ELEM );
			    memcpy( info->prod_restrictions.use_defaults,
					buf+13*MAX_UNITS_ELEM, MAXPLAYERS*MAX_UNITS_ELEM );
			    //need to block construct expansion units in noexpansion game
			    if (!(info->flags&STARMAP_FLAG_EXPANSION))
			    {
				for (i=0;i<MAX_UNITS_ELEM;i++)
				    if (IsExpansionUnit(i))
				    {
					info->prod_restrictions.global_restrictions[i]=0;
					for (j=0;j<MAXPLAYERS;j++)
					    info->prod_restrictions.player_restrictions[j][i]=0;
				    }
			    }
			}
				    
		}
		else if( !strcmp( name, "UNIS" ) )
		{
			if (!testplay)
			{
		    	    memcpy(&info->unit_settings,buf,section.size);
			    memmove(&info->unit_settings.weaponbonusdamage,
				    ((char *)&info->unit_settings.weaponbasedamage)+2*MAX_STARWEAPONS_ELEM,
				    2*MAX_STARWEAPONS_ELEM);
			}
		}
		else if( !strcmp( name, "UNIx" ) )
		{
			if (!testplay)
			{
			    memcpy(&info->unit_settings,buf,section.size);
			}
		}
		else if( !strcmp( name, "UPGS" ) )
		{
			if (!testplay)
			{
			    memcpy( info->upgr_settings.use_defaults,
			    	    buf+(0)*MAX_STARUPGRADES_ELEM,
				    MAX_STARUPGRADES_ELEM);
			    memcpy( info->upgr_settings.basecostminerals,
				    buf+(1)*MAX_STARUPGRADES_ELEM,
				    2*MAX_STARUPGRADES_ELEM);
			    memcpy( info->upgr_settings.upgradecostminerals,
				    buf+(3)*MAX_STARUPGRADES_ELEM,
				    2*MAX_STARUPGRADES_ELEM);
			    memcpy( info->upgr_settings.basecostgas,
				    buf+(5)*MAX_STARUPGRADES_ELEM,
				    2*MAX_STARUPGRADES_ELEM);
			    memcpy( info->upgr_settings.upgradecostgas,
				    buf+(7)*MAX_STARUPGRADES_ELEM,
				    2*MAX_STARUPGRADES_ELEM);
			    memcpy( info->upgr_settings.basecosttime,
				    buf+(9)*MAX_STARUPGRADES_ELEM,
				    2*MAX_STARUPGRADES_ELEM);
			    memcpy( info->upgr_settings.upgradecosttime,
				    buf+(11)*MAX_STARUPGRADES_ELEM,
				    2*MAX_STARUPGRADES_ELEM);
			}
		}
		else if( !strcmp( name, "UPGx" ) )
		{
			if (!testplay)
			{
			    memcpy(&info->upgr_settings,buf,section.size);
			}
		}
		else if( !strcmp( name, "TECS" ) )
		{
			if (!testplay)
			{
			    memcpy( info->tech_settings.use_defaults,
				    buf+(0)*MAX_STARTECHDATA_ELEM,
				    MAX_STARTECHDATA_ELEM);
			    memcpy( info->tech_settings.costminerals,
				    buf+(1)*MAX_STARTECHDATA_ELEM,
				    2*MAX_STARTECHDATA_ELEM);
			    memcpy( info->tech_settings.costgas,
				    buf+(3)*MAX_STARTECHDATA_ELEM,
				    2*MAX_STARTECHDATA_ELEM);
			    memcpy( info->tech_settings.researchtime,
				    buf+(5)*MAX_STARTECHDATA_ELEM,
				    2*MAX_STARTECHDATA_ELEM);
			    memcpy( info->tech_settings.energycost,
				    buf+(7)*MAX_STARTECHDATA_ELEM,
				    2*MAX_STARTECHDATA_ELEM);
			}
		}
		else if( !strcmp( name, "TECx" ) )
		{
			if (!testplay)
			{
			    memcpy(&info->tech_settings,buf,section.size);
			}
		}
		else if( !strcmp( name, "UPGR" ) )
		{
			if (!testplay)
			{

			    for (i=0;i<MAXPLAYERS;i++)
				memcpy( info->upgr_restrictions.max_upgrade[i],
					buf+(i+0)*MAX_STARUPGRADES_ELEM,MAX_STARUPGRADES_ELEM);
			    for (i=0;i<MAXPLAYERS;i++)
				memcpy( info->upgr_restrictions.start_upgrade[i],
					buf+(i+12)*MAX_STARUPGRADES_ELEM,MAX_STARUPGRADES_ELEM);
			    memcpy( default_max_upgrade,
					buf+24*MAX_STARUPGRADES_ELEM,MAX_STARUPGRADES_ELEM);
			    memcpy( default_start_upgrade,
					buf+25*MAX_STARUPGRADES_ELEM,MAX_STARUPGRADES_ELEM);
		    	    for (i=0;i<MAXPLAYERS;i++)
				memcpy( use_upgrdefaults[i],
					buf+(i+26)*MAX_STARUPGRADES_ELEM,MAX_STARUPGRADES_ELEM);
			    for (i=0;i<MAXPLAYERS;i++)
				for (j=0;j<MAX_STARUPGRADES_ELEM;j++)
				    if (use_upgrdefaults[i][j]==1)
				    {
					info->upgr_restrictions.max_upgrade[i][j]=default_max_upgrade[j];
					info->upgr_restrictions.start_upgrade[i][j]=default_start_upgrade[j];
	    			    }
			}
		}
		else if( !strcmp( name, "PUPx" ) ) /* Brood version */
		{
			if (!testplay)
			{

			    for (i=0;i<MAXPLAYERS;i++)
				memcpy( info->upgr_restrictions.max_upgrade[i],
					buf+(i+0)*MAX_BROODUPGRADES_ELEM,MAX_BROODUPGRADES_ELEM);
			    for (i=0;i<MAXPLAYERS;i++)
				memcpy( info->upgr_restrictions.start_upgrade[i],
					buf+(i+12)*MAX_BROODUPGRADES_ELEM,MAX_BROODUPGRADES_ELEM);
			    memcpy( default_max_upgrade,
					buf+24*MAX_BROODUPGRADES_ELEM,MAX_BROODUPGRADES_ELEM);
			    memcpy( default_start_upgrade,
					buf+25*MAX_BROODUPGRADES_ELEM,MAX_BROODUPGRADES_ELEM);
			    for (i=0;i<MAXPLAYERS;i++)
				memcpy( use_upgrdefaults[i],
					buf+(i+26)*MAX_BROODUPGRADES_ELEM,MAX_BROODUPGRADES_ELEM);
			    for (i=0;i<MAXPLAYERS;i++)
				for (j=0;j<MAX_BROODUPGRADES_ELEM;j++)
				    if (use_upgrdefaults[i][j]==1)
				    {
					info->upgr_restrictions.max_upgrade[i][j]=default_max_upgrade[j];
					info->upgr_restrictions.start_upgrade[i][j]=default_start_upgrade[j];
	    			    }
			}
		}
		else if( !strcmp( name, "PTEC" ) )
		{
			if (!testplay)
			{
		    
			for (i=0;i<MAXPLAYERS;i++)
			    memcpy( info->tech_restrictions.tech_available[i],
				    buf+(i+0)*MAX_STARTECHDATA_ELEM,MAX_STARTECHDATA_ELEM);
			for (i=0;i<MAXPLAYERS;i++)
			    memcpy( info->tech_restrictions.tech_researched[i],
				    buf+(i+12)*MAX_STARTECHDATA_ELEM,MAX_STARTECHDATA_ELEM);
			memcpy( default_tech_available,
				buf+24*MAX_STARTECHDATA_ELEM,MAX_STARTECHDATA_ELEM);
			memcpy( default_tech_researched,
				buf+25*MAX_STARTECHDATA_ELEM,MAX_STARTECHDATA_ELEM);
			for (i=0;i<MAXPLAYERS;i++)
			    memcpy( use_techdefaults[i],
				    buf+(i+26)*MAX_STARTECHDATA_ELEM,MAX_STARTECHDATA_ELEM);
			//copy def to nodef if def flag is set
			for (i=0;i<MAXPLAYERS;i++)
			    for (j=0;j<MAX_STARTECHDATA_ELEM;j++)
				if (use_techdefaults[i][j]==1)
				{
				    info->tech_restrictions.tech_available[i][j]=default_tech_available[j];
				    info->tech_restrictions.tech_researched[i][j]=default_tech_researched[j];
				}
			}
		}
		else if( !strcmp( name, "PTEx" ) ) /* Brood version */
		{
			if (!testplay)
			{
			    for (i=0;i<MAXPLAYERS;i++)
				memcpy( info->tech_restrictions.tech_available[i],
				        buf+(i+0)*MAX_BROODTECHDATA_ELEM,MAX_BROODTECHDATA_ELEM);
    			    for (i=0;i<MAXPLAYERS;i++)
    				memcpy( info->tech_restrictions.tech_researched[i],
				        buf+(i+12)*MAX_BROODTECHDATA_ELEM,MAX_BROODTECHDATA_ELEM);
			    memcpy( default_tech_available,
			    	    buf+24*MAX_BROODTECHDATA_ELEM,MAX_BROODTECHDATA_ELEM);
			    memcpy( default_tech_researched,
				    buf+25*MAX_BROODTECHDATA_ELEM,MAX_BROODTECHDATA_ELEM);
			    for (i=0;i<MAXPLAYERS;i++)
				memcpy( use_techdefaults[i],
					buf+(i+26)*MAX_BROODTECHDATA_ELEM,MAX_BROODTECHDATA_ELEM);
			    for (i=0;i<MAXPLAYERS;i++)
				for (j=0;j<MAX_BROODTECHDATA_ELEM;j++)
				    if (use_techdefaults[i][j]==1)
				    {
					info->tech_restrictions.tech_available[i][j]=default_tech_available[j];
					info->tech_restrictions.tech_researched[i][j]=default_tech_researched[j];
				    }
			}
		}
		else if( !strcmp( name, "MTXM" ) )
		{
			info->display_map = (unsigned short *)
			    wmalloc( info->map_width*info->map_height*2 );
			if( !info->display_map )
			{
//				printf( "Could not allocate display map! exiting.\n" );
				return -1;
			}
			memcpy( info->display_map, buf, 
					info->map_width*info->map_height*2 );
		}
		else if( !strcmp( name, "TILE" ) )
		{
			info->level_tiles = (unsigned short *)
			    wmalloc( info->map_width*info->map_height*2 );
			if( !info->level_tiles )
			{
//				printf( "Could not allocate level's tile! exiting.\n" );
				return -1;
			}
			memcpy( info->level_tiles, buf, 
					info->map_width*info->map_height*2 );
		}
		else if( !strcmp( name, "UPRP" ) )
		{
		    if (!testplay)
		    {
			info->triggerunitsprop=(struct triggerunit *)wmalloc(section.size);
			memcpy(info->triggerunitsprop,buf,section.size);
		    }
		}
		else if( !strcmp( name, "UPUS" ) )
		{
		    if (!testplay)
		    {
//			savebuff("UPUS.txt",buf,section.size);
		    }
		}
		else if( !strcmp( name, "UNIT" ) )
		{
		    if (map_units.GetMaxElements())
		    {
			printf("Error unit lists not empty\n");
			return(-1);
		    }
		    do{
//			if (!testplay)
			{
			    unit = (unit_on_map *) wmalloc(sizeof(struct unit_on_map));
			    memcpy(unit,buf,sizeof(struct unit_on_map));
			    map_units.AddList(unit);
			}
//			else
//			    unit=(unit_on_map *)buf;
			if (unit->unit_type==SC_STARTLOC)
			{
			    info->pl_startlocplayernr[info->pl_startlocs++]=unit->player;
			}
			section.size-=sizeof(struct unit_on_map);
			buf+=sizeof(struct unit_on_map);
		    }while(section.size>0);
		}
		else if( !strcmp( name, "DD2 " ) )
		{
		    if (!testplay)
		    {
			//copy dd2 section for later use
			DD2_section2=DD2_section=(char *)wmalloc(section.size);
			DD2_sectionsize=section.size;
			DD2_sectionsize2=section.size;
			memcpy(DD2_section2,buf,section.size);
		    }
		}
		else if( !strcmp( name, "THG2" ) )
		{
		    if (!testplay)
		    {
			do{
			    memcpy(&tempdoodad,buf,sizeof(struct doodad_THG2_starmap));
			    doodadu = (doodad_on_map *) wmalloc(sizeof(struct doodad_on_map));
			    doodadu->sprites_id=tempdoodad.sprites_id;
			    doodadu->xpos=tempdoodad.xpos;
			    doodadu->ypos=tempdoodad.ypos;
			    doodadu->playernr=tempdoodad.playernr;
			    doodadu->flags=tempdoodad.flags&0xfffe;	//leave bit0 for open/close door(doodad)
			    map_doodads.AddList(doodadu);
			    //search in DD2_section for open/close flag
			    if (DD2_section)
			    {
				do
				{
				    memcpy(&tempdoodad2,DD2_section2,sizeof(struct doodad_DD2_starmap));
				    if (tempdoodad2.xpos==doodadu->xpos&&tempdoodad2.ypos==doodadu->ypos)
				    {
					if (tempdoodad2.state==1)
					    doodadu->flags |= DOODADBOTTOMSTATE;
				    }
				    DD2_section2+=sizeof(struct doodad_DD2_starmap);
				    DD2_sectionsize2-=sizeof(struct doodad_DD2_starmap);
				}while(DD2_sectionsize2>0);
			    }
			    DD2_section2 = DD2_section;
			    DD2_sectionsize2 = DD2_sectionsize;
			    section.size-=sizeof(struct doodad_THG2_starmap);
		    	    buf+=sizeof(struct doodad_THG2_starmap);
			}while(section.size>0);
		    }
		}
		else if( !strcmp( name, "MRGN" ) )
		{
		    if (!testplay)
			if (GAMETYPE == MAP_GAMETYPE_USEMAPSETTINGS)
			{
			    info->maxlocations  = section.size/sizeof(struct locations);
			    info->gamelocations = (struct locations *)wmalloc(info->maxlocations*sizeof(locations));
			    memcpy(info->gamelocations,buf,info->maxlocations*sizeof(locations));
			}
		}
		else if( !strcmp( name, "STR " ) )
		{
		    info->STR=(char *)wmalloc(section.size);
		    memcpy( info->STR, buf, section.size );
//		    for (i=0;i<1024;i++)
//			DEBUGMES("%s %d\n",getmapSTR(info,i),i);
//		    savebuff("STR.txt",info->STR,section.size);
		}
		else if( !strcmp( name, "MBRF" ) )
		{
		    if (GAMETYPE == MAP_GAMETYPE_USEMAPSETTINGS)
		    {
			info->BRIEFS_NR=section.size/sizeof(MAP_TRIGS);
			info->BRIEFS=(MAP_TRIGS *)wmalloc(section.size);
			memcpy(info->BRIEFS,buf,section.size);
//		    	savebuff("BRIEFS.txt",info->BRIEFS,section.size);
		    }
		}
		else if( !strcmp( name, "COLR" ) )
		{
		    for (i=0;i<8;i++)
			PLAYER[i].colorRACE=buf[i];
		}
		else if( !strcmp( name, "TRIG" ) )
		{
		    if (!testplay)
		    {
			if ( GAMETYPE == MAP_GAMETYPE_USEMAPSETTINGS )
			{
		    	    info->TRIGS=(MAP_TRIGS *)wmalloc(section.size);
			    memcpy(info->TRIGS,buf,section.size);
			    info->TRIGS_NR=section.size/sizeof(MAP_TRIGS);
//		    	    savebuff("TRIG.txt",info->TRIGS,section.size);
			}
			else
			{
			    //load presetted triggers from starclone.mpq
			    loadfilefrommpq(allmpq.GetElemNr(0),STAR_TRGMELEEFILE,(char **)&info->TRIGS,&trgfilesize);
			    info->TRIGS_NR = trgfilesize/sizeof(MAP_TRIGS);
			}
		    }
		}
		else if( !strcmp( name, "MASK" ) )
		{
		    if (!testplay)
		    {
			if (GAMETYPE == MAP_GAMETYPE_USEMAPSETTINGS)
			{
			    info->FogMask=(unsigned char *)wmalloc(section.size);
		    	    memcpy(info->FogMask,buf,section.size);
			}
		    }
		}
		else
		{
//		    printf("Unknown Section Name:%s Size=%d\n",name,section.size);
		}
	}
	if (DD2_section)
	    wfree(DD2_section);	
	wfree( prevbuf );
	wr_SFileCloseFile( f );

//	if (checkleak( buf )) DEBUGMES("leak\n");

	/* now that we know the terrain type and 
	 * tile positions, let's generate terrain tiles.
	 * Will replace display_map values with the ones
	 * from loaded header. */
	err=load_terrain_tiles( info,
			info->terrain, &info->tiles, 
			&info->tilecount, info->palette, info->display_map,
			&info->tileswalk, info->map_width, info->map_height, 
			info->minimap,
			&info->minimap_startx, &info->minimap_starty,
			&info->minimap_endx, &info->minimap_endy);
	return(err);
}

char player_owners[8][20] = {
	"(None)",
	"(None)",
	"(None)",
	"Rescuable",
	"(None)",
	"Computer",
	"Human",
	"Neutral"
};

char player_races[MAXPLAYERS][20] = {
	"Zerg",
	"Terran",
	"Protoss",
	"Independent",
	"Neutral",
	"User Select",
	"(None)",
	"Inactive",
	"(None)",
	"(None)",
	"Human",
	"(None)"
};

//=================================================
int get_forces( struct mapinfo *info ,int gametype)
{
    int i,j,forces,realforces,found,nrpl;
    char force_array[PLAYEDPLAYERS];
    switch(gametype)
    {
	case MAP_GAMETYPE_MELEE:
	case MAP_GAMETYPE_FREEFORALL:
	    for (i=0;i<PLAYEDPLAYERS;i++)
		info->pl_force.force_nr[i]=0;
	    for (i=0;i<MAXPLAYERS;i++)
		if (info->pl_iowner[i]==OWNER_RESCUABLE)
		    info->pl_iowner[i]=OWNER_HUMAN;
	    return(1);
	case MAP_GAMETYPE_USEMAPSETTINGS:
	    break;
	default:
	    return(1);
    }
    forces=0;
    realforces=0;
    for (i=0;i<PLAYEDPLAYERS;i++)
    {
	found=0;
	for (j=0;j<forces;j++)
	    if (info->pl_force.force_nr[i]==force_array[j])
	    {
		found=1;
		break;
	    }
	if (!found)
	    force_array[forces++]=info->pl_force.force_nr[i];
    }
    for (j=0;j<MAX_PLFORCES;j++)		//parce all forces slots
    {
	nrpl=0;
	for (i=0;i<PLAYEDPLAYERS;i++)
	{
	    if (info->pl_force.force_nr[i]==j)
		if (IfPlayerHaveStartLocation(info,i)!=-1)
	    	    nrpl++;
	}
	if (nrpl)
	    realforces++;
    }
    return(realforces);
}
//=================================================
void print_mapinfo( struct mapinfo *info )
{
	int i;

	if( !info )
		return;

	printf( "Map info:\n" );
//	printf( "Scenario type: '%s'\n", info->scenario_type );
	printf( "Format version: '%d %d %d'\n", info->format_version[0],
			info->format_version[1], info->format_version[2] );
	printf( "VCODE: '%s'\n", info->valid_vcode? "YES" : "NO" );
	printf( "Player OWNERs: " );
	for( i=0; i<MAXPLAYERS; i++ )
		printf( "[%10s] ", player_owners[ info->pl_owner[i] ] );

	printf( "\nPlayer Races: " );
	for( i=0; i<MAXPLAYERS; i++ )
		printf( "[%10s] ", player_races[ info->pl_race[i] ] );

	printf( "\nPlayer Forces=%d ",force_slots.forces );
	for( i=0; i<PLAYEDPLAYERS; i++ )
		printf( "FORCENR=[%d] ", info->pl_force.force_nr[i] );
	printf("\n");
	for( i=0; i<MAX_PLFORCES; i++ )
		printf( "FORCENAME=[%d][%s]", info->pl_force.force_ID[i],getmapSTR(info,info->pl_force.force_ID[i]-1));
	printf("\n");
	for( i=0; i<MAX_PLFORCES; i++ )
		printf( "FORCEFLG=[%x] ", info->pl_force.force_flag[i] );

	printf( "\nTerrain type: '%s'\n", terraintypes[info->terrain] );
	printf( "Map width: %d\n", info->map_width );
	printf( "Map height: %d\n", info->map_height );
	printf( "Minimap begin x/y width: %d/%d\n",info->minimap_startx, info->minimap_starty);
	printf( "Minimap end x/y width: %d/%d\n",info->minimap_endx, info->minimap_endy );
}
//=================================================
char tempfn[1024];
//=================================================
int load_starmap( const char *mapfile,char *fname, struct mapinfo *info,GAMECONF *conf)
{
    HANDLE mpq;
    int result,i,j,k,ii,jj,tile32,err,pl;
    unsigned char firstlevel,level;
    unsigned short tile;

    _RACE = ZERGRACE;
    MAPDEF = TERRAIN|UNITS;
    MaxObjects = 0;
    info->flags &= ~STARMAP_FLAG_IGNOREMAPMOVE;
    if (fname)
    {
	strcpy(tempfn,fname);
	strcat(tempfn,"\\");
	strcat(tempfn,STAREDITCHK);
	result = read_starmap(NULL,tempfn, info,STARMAP_LOADFORPLAY);
    }
    else
    {
	result = wr_SFileOpenArchive(mapfile,0,MPQOPENPARAM,&mpq);
	if (!result)
    	    return(1);
	result = read_starmap(mpq,STAREDITCHK, info,STARMAP_LOADFORPLAY);
    }
    if (result)
        return(1);
    info->maplevel=(char*) wmalloc(info->map_width*info->map_height);
    info->mapwalk=(char*) wmalloc(info->map_width*4*info->map_height*4);
    info->constrmap=(unsigned char*) wmalloc(info->map_width*info->map_height);
//	info->buildswalk=(unsigned char*) wmalloc(info->map_width*info->map_height);
    info->creepflagplace=(unsigned char *) wmalloc(info->map_width*info->map_height);
    info->creepflagneeded=(unsigned char *) wmalloc(info->map_width*info->map_height);

    info->mapbits.seedetector=(unsigned char *) wmalloc(info->map_width*info->map_height);
    info->mapbits.arbiterfield=(unsigned char *) wmalloc(info->map_width*info->map_height);

    info->mapbits.seedetector2=(unsigned char *) wmalloc(info->map_width*info->map_height);
    info->mapbits.arbiterfield2=(unsigned char *) wmalloc(info->map_width*info->map_height);
    info->mapbits.mageeffect[DSWARMEFFECT]=(unsigned char *) wmalloc(info->map_width*info->map_height);
    info->mapbits.mageeffect[DISRUPTEFFECT]=(unsigned char *) wmalloc(info->map_width*info->map_height);

    memset(info->maplevel,0x0,info->map_width*info->map_height);
    memset(info->mapwalk,0x1,info->map_width*4*info->map_height*4);
    memset(info->constrmap,0x0,info->map_width*info->map_height);
//        memset(info->buildswalk,0x1,info->map_width*info->map_height);
    memset(info->creepflagplace,0x0,info->map_width*info->map_height);
    memset(info->creepflagneeded,0x0,info->map_width*info->map_height);

    memset(info->mapbits.seedetector,0x0,info->map_width*info->map_height);
    memset(info->mapbits.arbiterfield,0x0,info->map_width*info->map_height);

    memset(info->mapbits.seedetector2,0x0,info->map_width*info->map_height);
    memset(info->mapbits.arbiterfield2,0x0,info->map_width*info->map_height);
    memset(info->mapbits.mageeffect[DSWARMEFFECT],0x0,info->map_width*info->map_height);
    memset(info->mapbits.mageeffect[DISRUPTEFFECT],0x0,info->map_width*info->map_height);
	
//	memcpy(info->origpalette,info->palette,256*4);			//copy additional palette info for gamma
    CreateMiniMapPixels(info);	//set my map vars from starmap
    EmptyAllCreeps();
    AddToMinimapNeutralObjs(&map_units,info,STARMAP_LOADFORPLAY);
        
    for (i=0;i<MAXYMAP8;i++)
        for (j=0;j<MAXXMAP8;j++)
        {
	    tile32=info->display_map[i/4*MAXXMAP+j/4];
	    for (ii=0;ii<4;ii++)
	    {
	        for (jj=0;jj<4;jj++)
	        {
	    	    info->mapwalk[i*MAXXMAP8+j]=info->tileswalk[tile32]&&(1<<(ii*4+jj));
		}
	    }
	}

    for (i=0;i<MAXYMAP;i++)
        for (j=0;j<MAXXMAP;j++)
        {
		tile32=info->display_map[i*MAXXMAP+j];
		for (int k=0;k<16;k++)
		{
		    //tile8 16 bits
		    //0   - tilewalkable
		    //1   - mid
		    //2   - high
		    //3   - blocks view
		    //4   - is rampa
		    tile=info->vf4mem[tile32*16+k];
		    level=(tile>>1)&3;
		    if (tile&0x10)//if rampa
		    {
			level=0;
			break;
		    }
		    if (!k)
			firstlevel=level;
		    else
			if (firstlevel!=level)
			{
			    level=0;
			    break;
			}
		}
		info->maplevel[i*MAXXMAP+j]=level;
	}
    if (info->terrain==TERRAIN_SPACEPLATFORM)
    {
	if (loadStars("parallax\\star.spk"))
	{
	    return(3);
	}
    }
//    print_mapinfo(info);
    err=loadpal(info->terrain,info->palette);
    if (err)
    {
        printf("error %d ,can't load palette for %s\n",err,palettedir[info->terrain]);
        return(4);
    }
    err=loadcreep(info->terrain);
    if (err)
    {
        printf("error %d.grp ,can't load creep for %s\n",err,palettedir[info->terrain]);
        return(5);
    }
    if (!NETWORKGAME)
	NUMBGAMER=GetPlayableGameNumber(info);
    randomizelocations(info);
    randomizerace(info,conf);
    randomizecolor();
    CreateDefaultAliance(info);
    resetinfoplayers();
    CreateDefaultVision(info);
    First_Triggers_Prepare(info,info->TRIGS_NR,info->TRIGS);
    observers_prepare(info);		//make vision for observers players

    for (i=0;i<PLAYEDPLAYERS;i++)
    {
	if (IfPlayerHaveStartLocation(info,i) != -1)
	{
	    info->mapbits.whitefog[i]=(unsigned char *) wmalloc(info->map_width*info->map_height);
	    info->mapbits.blackfog[i]=(unsigned char *) wmalloc(info->map_width*info->map_height);
	    info->mapbits.whitefog2[i]=(unsigned char *) wmalloc(info->map_width*info->map_height);
	    info->mapbits.savedcreep[i]=(unsigned char *) wmalloc(info->map_width*info->map_height);
	    info->mapbits.savedtile[i]=(unsigned short int *) wmalloc(info->map_width*info->map_height*sizeof(short int));
    	    info->mapbits.underfog[i]=(UNITUNDERFOG **) wmalloc(info->map_width*info->map_height*sizeof(UNITUNDERFOG *));

	    memset(info->mapbits.whitefog[i],0x0,info->map_width*info->map_height);
	    memset(info->mapbits.blackfog[i],0x0,info->map_width*info->map_height);
	    memset(info->mapbits.whitefog2[i],0x0,info->map_width*info->map_height);
	    memset(info->mapbits.savedcreep[i],0x00,info->map_width*info->map_height);
	    memset(info->mapbits.savedtile[i],0xff,info->map_width*info->map_height*sizeof(short int));
	    memset(info->mapbits.underfog[i],0x0,info->map_width*info->map_height*sizeof(UNITUNDERFOG *));
	}
    }

    mytimer.SetMyTimerFunc(&gametimer,NULL);
    BlockSoundToPlay();
    TRIG_ChangeStat=0;
    createallobj(info);
    TRIG_ChangeStat=1;
    UnBlockSoundToPlay();

    for (i=0;i<PLAYEDPLAYERS;i++)
    {
	if (IfPlayerHaveStartLocation(info,i) != -1)
	    ApplyFogMask(info,i);
    }
    saveoldtileandcreep();

    if (info->FogMask)
    {
	wfree(info->FogMask);
	info->FogMask = NULL;
    }

    _RACE = getplayerrace(&map,NUMBGAMER);
    if (_RACE == RACE_OBSERVER)
	_RACE = RACE_TERRAN;

    if (loadbuttons(_RACE))
    {
        printf("!!!not load all console/icons grp files!!!\n");
	return(7);
    }
    if (loadramka(_RACE))
    {
        printf("!!!not load all console/icons grp files!!!\n");
	return(8);
    }
    if (loadtilegrp(_RACE))
    {
        printf("error loading tile.grp table\n");
        exit(-1);
    }

    wclrscr(0);
    
    palchange(info->palette,gameconf.videoconf.gamma,gameconf.videoconf.saturate);
    if (!gameconf.grmode.emulationmode)
	wscreenon();
    if (loadstaticsmk())
    {
//        gameend("Problem with loading basic smk files");
    }
    for (i=0;i<PLAYEDPLAYERS;i++)
	info->clearfog[i]=1;
//    info->iscriptmem = LoadIScriptBin();
    return(0);
}
//=================================================
void unload_starmapallocated(struct mapinfo *info )
{
    Remove_Briefing(info);
    Remove_Triggers(info);
    if (info->mpqclose)
    {
        wr_SFileCloseArchive(info->mpqid);
        info->mpqid=NULL;
	info->mpqclose=0;
    }
    if (info->STR)
    {
	wfree(info->STR);
	info->STR=NULL;
    }
    if (info->display_map)
    {
	wfree(info->display_map);
	info->display_map=NULL;
    }
    if (info->level_tiles)
    {
	wfree(info->level_tiles);
	info->level_tiles=NULL;
    }
    if (info->buildswalk)
    {
	wfree(info->buildswalk);
	info->buildswalk=NULL;
    }
    if (info->creeptiles)
    {
	wfree(info->creeptiles);
	info->creeptiles=NULL;
    }
    if (info->vf4mem)
    {
	wfree(info->vf4mem);
	info->vf4mem=NULL;
    }
    if (info->tiles)
    {
	wfree(info->tiles);
	info->tiles=NULL;
    }
    if (info->tileswalk)
    {
	wfree(info->tileswalk);
	info->tileswalk=NULL;
    }
    if (info->creeptileminimapcolors)
    {
	wfree(info->creeptileminimapcolors);
	info->creeptileminimapcolors=NULL;
    }
    RemoveUnitsFromLists(&map_units);
}
//=================================================
void unload_starmap(struct mapinfo *info )
{
    int i,j,k;
    
//    UnloadIScriptBin(info->iscriptmem);
//    info->iscriptmem = NULL;
    
    unloadstaticsmk();
    unloadtilegrp();
    FreePylonArea(info);
    unloadramka();		//race ramka unload
    unloadbuttons();		//race buttons unload

    freeOBJS();//remove all objects

    weaponflingy.FreeAndEmptyAll();

    mytimer.ClearMyTimerFunc();
//    UnloadDoodadList();	//unload list with all doodads


    unloadpal();	//remove all palettes(low,pal,transp,etc...)
    unloadcreep();	//unload creep decorations from tileset.grp

    unload_starmapallocated(info);
    
    for (i=0;i<MAXLEADERBOARDS;i++)
	if (info->leaderboards[i])
	{
	    if (info->leaderboards[i]->txtstr)
		wfree(info->leaderboards[i]->txtstr);
	    wfree(info->leaderboards[i]);
	    info->leaderboards[i]=NULL;
	}
    if (info->terrain==TERRAIN_SPACEPLATFORM)
    {
	unloadStars();
    }
    if (info->triggerunitsprop)
    {
	wfree(info->triggerunitsprop);
	info->triggerunitsprop=NULL;
    }
    if (info->gamelocations)
    {
	wfree(info->gamelocations);
	info->gamelocations=NULL;
    }
    if (info->creepflagneeded)
    {
	wfree(info->creepflagneeded);
	info->creepflagneeded=NULL;
    }
    if (info->creepflagplace)
    {
	wfree(info->creepflagplace);
	info->creepflagplace=NULL;
    }
    if (info->constrmap)
    {
	wfree(info->constrmap);
	info->constrmap=NULL;
    }
    if (info->mapwalk)
    {
	wfree(info->mapwalk);
	info->mapwalk=NULL;
    }
    if (info->maplevel)
    {
	wfree(info->maplevel);
	info->maplevel=NULL;
    }
    if (info->creeptiles)
    {
	wfree(info->creeptiles);
	info->creeptiles=NULL;
    }
    if (info->mapbits.seedetector)
    {
	wfree(info->mapbits.seedetector);
	info->mapbits.seedetector=NULL;
    }
    if (info->mapbits.arbiterfield)
    {
	wfree(info->mapbits.arbiterfield);
	info->mapbits.arbiterfield=NULL;
    }
    if (info->mapbits.seedetector2)
    {
	wfree(info->mapbits.seedetector2);
	info->mapbits.seedetector2=NULL;
    }
    if (info->mapbits.arbiterfield2)
    {
	wfree(info->mapbits.arbiterfield2);
	info->mapbits.arbiterfield2=NULL;
    }
    if (info->mapbits.mageeffect[DSWARMEFFECT])
    {
	wfree(info->mapbits.mageeffect[DSWARMEFFECT]);
	info->mapbits.mageeffect[DSWARMEFFECT]=NULL;
    }
    if (info->mapbits.mageeffect[DISRUPTEFFECT])
    {
	wfree(info->mapbits.mageeffect[DISRUPTEFFECT]);
	info->mapbits.mageeffect[DISRUPTEFFECT]=NULL;
    }
    for (i=0;i<MAXPLAYERS;i++)
    {
	if (IfPlayerHaveStartLocation(info,i)!=-1)
	{
		if (info->mapbits.blackfog[i])
		{
		    wfree(info->mapbits.blackfog[i]);
		    info->mapbits.blackfog[i]=NULL;
		}
		if (info->mapbits.whitefog[i])
		{
		    wfree(info->mapbits.whitefog[i]);
		    info->mapbits.whitefog[i]=NULL;
		}
		if (info->mapbits.whitefog2[i])
		{
		    wfree(info->mapbits.whitefog2[i]);
		    info->mapbits.whitefog2[i]=NULL;
		}
		if (info->mapbits.savedcreep[i])
		{
		    wfree(info->mapbits.savedcreep[i]);
		    info->mapbits.savedcreep[i]=NULL;
		}
		if (info->mapbits.savedtile[i])
		{
		    wfree(info->mapbits.savedtile[i]);
		    info->mapbits.savedtile[i]=NULL;
		}
		if (info->mapbits.underfog[i])
		{
		    for (j=0;j<MAXYMAP;j++)
			for (k=0;k<MAXXMAP;k++)
			    if (info->mapbits.underfog[i][j*MAXXMAP+k])
				wfree(info->mapbits.underfog[i][j*MAXXMAP+k]);
    	    	    wfree(info->mapbits.underfog[i]);
    	    	    info->mapbits.underfog[i]=NULL;
		}
	}
    }
    info->valid_vcode = 0;
//    RemoveUnitsFromLists(&map_units);
    MaxObjsRevealMap=0;
}
//=================================================
void CreateAliance(struct mapinfo *info,int player1,int player2,int flag)
{
//    DEBUGMESSCR("set alince status pl1=%d pl2=%d flag=%d\n",player1,player2,flag);
    info->pl_allied[player1][player2]=flag;
//	info->pl_allied[player2][player1]=flag;
}
//=================================================
void CreateDefaultAliance(struct mapinfo *info)
{
    int i,j;
    for (i=0;i<MAXPLAYERS;i++)
	for (j=0;j<MAXPLAYERS;j++)
    	    if (i>=PLAYEDPLAYERS||j>=PLAYEDPLAYERS)
                info->pl_allied[i][j]=NEUTRALOBJ;
	    else
		if (i==j)
            	    info->pl_allied[i][j]=MYOBJ;
        	else
            	    info->pl_allied[i][j]=ENEMYOBJ;
}
//=================================================
int CreateBitsVision(struct mapinfo *info,int player)
{
    int i,pl,allbits=0;
    for (i=0;i<PLAYEDPLAYERS;i++)
    {
	if (player_vision(player,i)==PLAYER_VISION)
	    allbits|=(1<<i);
    }
    return(allbits);

}
//=================================================
//player see player2
//=================================================
void CreateVision(struct mapinfo *info,int player,int seeplayer2,int flag)
{
    unsigned char bits=0;
    info->pl_vision[player][seeplayer2]=flag;
    info->pl_visionbits[player]=CreateBitsVision(info,player);
}
//=================================================
void CreateDefaultVision(struct mapinfo *info)
{
    int i,ii,j,k,pl,nrpl,forces;
    signed char sharedvision[MAXPLAYERS];
    signed char allies[MAXPLAYERS];
    for (i=0;i<MAXPLAYERS;i++)
    {
	if (IfPlayerHaveStartLocation(info,i)!=-1)
	    CreateVision(info,i,i,PLAYER_VISION);
    }
    forces=get_forces(info,GAMETYPE);
    //create vision and allies for forces
    switch(GAMETYPE)
    {
	case MAP_GAMETYPE_MELEE:
	    memset(sharedvision,-1,MAXPLAYERS);
	    memset(allies,-1,MAXPLAYERS);
	    k=0;
	    for (i=0;i<PLAYEDPLAYERS;i++)
	        if (gameconf.pl_owner[i]==OWNER_COMPUTER)
	        {
	    	    sharedvision[k]=i;
	    	    allies[k]=i;
	    	    k++;
	    	}
	    for (i=0;i<k;i++)
		for (ii=i+1;ii<k;ii++)
		{
		    CreateVision(info,sharedvision[i],sharedvision[ii],PLAYER_VISION);
		    CreateVision(info,sharedvision[ii],sharedvision[i],PLAYER_VISION);
		}
	    for (i=0;i<k;i++)
	        for (ii=i+1;ii<k;ii++)
		{
		    CreateAliance(info,allies[i],allies[ii],NEUTRALOBJ);
		    CreateAliance(info,allies[ii],allies[i],NEUTRALOBJ);
		}
	    break;//aliances & visions between computers
	case MAP_GAMETYPE_FREEFORALL:
	    break;//no aliances no visions
	case MAP_GAMETYPE_USEMAPSETTINGS:
            if (forces>1)
            {
			for (j=0;j<MAX_PLFORCES;j++)
			{
				nrpl=GetPlayersInForceNr(info,j);
				if (nrpl>1)
				{
				    if (info->pl_force.force_flag[j]&FORCEFLAG_SHAREDVISION)
				    {
					k=0;
					memset(sharedvision,-1,MAXPLAYERS);
					for (i=0;i<PLAYEDPLAYERS;i++)
					    if (j==info->pl_force.force_nr[i])
						sharedvision[k++]=i;
					for (i=0;i<k;i++)
					    for (ii=i+1;ii<k;ii++)
						{
						    CreateVision(info,sharedvision[i],sharedvision[ii],PLAYER_VISION);
						    CreateVision(info,sharedvision[ii],sharedvision[i],PLAYER_VISION);
						}
				    }
				    if (info->pl_force.force_flag[j]&FORCEFLAG_ALLIES)
				    {
					k=0;
					memset(allies,-1,MAXPLAYERS);
					for (i=0;i<PLAYEDPLAYERS;i++)
					    if (j==info->pl_force.force_nr[i])
						allies[k++]=i;
					for (i=0;i<k;i++)
					    for (ii=i+1;ii<k;ii++)
						{
						    CreateAliance(info,allies[i],allies[ii],ALLIANCEOBJ);
						    CreateAliance(info,allies[ii],allies[i],ALLIANCEOBJ);
						}
				    }
				}
			}
            }
            k=0;
	    memset(sharedvision,-1,MAXPLAYERS);
            for (i=0;i<MAXPLAYERS;i++)
            {
        	if (info->pl_iowner[i] == OWNER_NEUTRAL || info->pl_iowner[i] == OWNER_RESCUABLE)
        	{
        	    for (j=0;j<MAXPLAYERS;j++)
        	    {
			CreateAliance(info,i,j,NEUTRALOBJ);
			CreateAliance(info,j,i,NEUTRALOBJ);
		    }
		}
	    }
            for (i=0;i<MAXPLAYERS;i++)
            {
        	if (info->pl_iowner[i] == OWNER_COMPUTER)
        	    for (j=0;j<MAXPLAYERS;j++)
        		if (i != j && info->pl_iowner[j] == OWNER_COMPUTER)
        		{
			    CreateAliance(info,i,j,NEUTRALOBJ);
			    CreateAliance(info,j,i,NEUTRALOBJ);
			}
	    }
	    break;
    }//switch
}
//=================================================
int TILE8POSINTILE32[4]={0*32+0,0*32+16,16*32+0,16*32+16};
char createcolors8x8(char *bytes32x32,int pos)
{
    int tempcolors[256],maxcolor=0,i,j;
    memset(tempcolors,0,256*sizeof(int));
    for (i=0;i<8;i++)
    {
	for (j=0;j<8;j++)
	{
	    tempcolors[bytes32x32[TILE8POSINTILE32[pos]+i*32+j]]++;
	}
    }
    for (i=0;i<256;i++)
    {
	if (tempcolors[i]>maxcolor)
	    maxcolor=tempcolors[i];
    }
    return(maxcolor);
}
//=================================================
int *createminimapcolorsfrom(char *tiles,int tilecnt)
{
    int i,j;
    int *minimapcolors;
    minimapcolors=(int *)wmalloc(tilecnt*sizeof(int));
    memset(minimapcolors,0,tilecnt*sizeof(int));
    for (i=0;i<tilecnt;i++)
    {
	for (j=0;j<4;j++)
	{
	    minimapcolors[i]|=createcolors8x8(tiles+i*32*32,j)<<(j*8);
	}
    }
    return(minimapcolors);
}
//=================================================
int starmap_info( char *pathname,const char *fname, mapinfo *info)
{
	HANDLE mpq;
	int result;
	if (!pathname)
	{
	    result = wr_SFileOpenArchive(fname,0,MPQOPENPARAM,&mpq);
	    if (!result)
    		return(-1);
	    strcpy(tempfn,STAREDITCHK);
	}
	else
	{
	    mpq=NULL;
	    strcpy(tempfn,pathname);
	    strcat(tempfn,"\\");
	    strcat(tempfn,STAREDITCHK);
	}
	result = read_starmap(mpq, tempfn, info,STARMAP_LOADFORINFO);
	if (result)
	    return(1);
	CreateMiniMapPixels(info);	//set my map vars from starmap
	AddToMinimapNeutralObjs(&map_units,info,STARMAP_LOADFORINFO);
	return(0);
}
//=================================================
void randomizerace(struct mapinfo *info,GAMECONF *conf)
{
    if (GAMETYPE == MAP_GAMETYPE_USEMAPSETTINGS)
	for (int i=0;i<MAXPLAYERS;i++)
	{
	    if (info->pl_race[i] == RACE_INDEPENDENT)
		info->pl_race[i] = myrand(3);
	}
    else
    {
        memcpy(info->pl_race,conf->pl_race,sizeof(conf->pl_race));
	memcpy(info->pl_owner,conf->pl_owner,sizeof(conf->pl_owner));
    }
}
//=====================================
void randomizecolor(void)
{
    int i,j,rndcolor,found;
    if (GAMETYPE != MAP_GAMETYPE_USEMAPSETTINGS)
    {
	    for (i=0;i<PLAYEDPLAYERS;i++)
	    {
		PLAYER[i].colorRACE=-1;
		do{
		    found=0;
		    rndcolor=myrand(PLAYEDPLAYERS);
		    for (j=0;j<i;j++)
			if (PLAYER[j].colorRACE==rndcolor)
			{
			    found=1;
			    break;
	    		}
		}while(found);
		PLAYER[i].colorRACE=rndcolor;
	    }
    }
    for (i=0;i<MAXPLAYERS;i++)
    {
        if (PLAYER[i].colorRACE==-1)
    	    PLAYER[i].colorRACE=i;
    }
}
//=====================================
void randomizelocations(struct mapinfo *tempmap)
{
    int i,j,force,loc,locpl,startlocs,locnr=0,forceplrs,offsetslot=0;
    signed char locs[MAXPLAYERS];
    signed char pl_location[MAXPLAYERS];
    startlocs = GetTotalStartLocations(tempmap);
    memset(pl_location,-1,MAXPLAYERS);
    memset(tempmap->pl_location,-1,MAXPLAYERS);
    for (force=0;force<MAX_PLFORCES;force++)
    {
	forceplrs=GetPlayersInForceNr(tempmap,force);
	j=0;
	if (forceplrs)
	{
	    memset(locs,-1,MAXPLAYERS);
    	    for (i=0;i<startlocs;i++)
	    {
		locpl=tempmap->pl_startlocplayernr[i];
		if (tempmap->pl_force.force_nr[locpl] == force)// || force_slots.forces == 1 )
		{
    		    if (tempmap->pl_owner[locpl]==OWNER_COMPUTER||
			tempmap->pl_owner[locpl]==OWNER_HUMAN||
			tempmap->pl_owner[locpl]==OWNER_NEUTRAL||
			tempmap->pl_race[locpl]==RACE_OBSERVER)
		    {
			locs[j++]=locpl;
		    }
    		}
    	    }
	}
	if ( (tempmap->pl_force.force_flag[force] & FORCEFLAG_RANDOMSTARTLOCATION ) && (j > 1) )
	{
    	    for (i=0;i<j;i++)
    	    {
//		printf("SEED=%u\n",getseed());
    		do{
		    loc = myrand(j);
    		}while(pl_location[locs[loc]]!=-1);
		pl_location[locs[loc]] = locnr;
		locnr++;
	    }
        }
        else
        {
    	    for (i=0;i<j;i++)
    		pl_location[locs[i]] = locs[i];
        }
    }
    for (i=0,j=0;i<MAXPLAYERS;i++,j++)
    {
    	if (gameconf.pl_owner[j]==OWNER_FORCENAME)
    	    j++;
	tempmap->pl_location[j]=pl_location[i];
    }
}
//=====================================
void starmap_forceslots( mapinfo *info,FORCE_SLOTS *fslots,int gametype)
{
    int i,j,nrpl,slot=0;
    fslots->Clear();
    memcpy(fslots->force_nr,info->pl_force.force_nr,PLAYEDPLAYERS);
    fslots->forces=get_forces(info,gametype);
    if (fslots->forces==1)
    {
	for (i=0;i<PLAYEDPLAYERS;i++)
	{
	    if (info->pl_owner[i]==OWNER_COMPUTER||info->pl_owner[i]==OWNER_HUMAN||info->pl_race[i]==RACE_OBSERVER)
		fslots->openedslots++;
	    fslots->usedslots1[i]=info->pl_owner[i];
	    fslots->usedslots2[i]=info->pl_race[i];
	}
    }
    else
    {
	for (j=0;j<MAX_PLFORCES;j++)
	{
	    nrpl=GetPlayersInForceNr(info,j);
	    if (nrpl)
	    {
		fslots->usedslots1[slot]=OWNER_FORCENAME;
		fslots->usedslots2[slot]=j;
		fslots->AddForceName(j,getmapSTR(info,info->pl_force.force_ID[j]-1));
		slot++;
		fslots->openedslots++;
		for (i=0;i<nrpl;i++)
		{
		    fslots->usedslots1[slot]=info->pl_owner[i];
		    fslots->usedslots2[slot]=info->pl_race[i];
		    slot++;
		    fslots->openedslots++;
		}
	    }
	}
    }
}
//=================================================
int GetForceFirstEmptySlot(FORCE_SLOTS *fslots)
{
    int i;
    for (i=0;i<fslots->openedslots;i++)
    {
	if (fslots->usedslots1[i]!=OWNER_FORCENAME)
	    return(i);
    }
    return(0);
}
//=================================================
int GetPlayersInForceNr(mapinfo *info,int forcenr)
{
    int i,nrpl=0;
    for (i=0;i<PLAYEDPLAYERS;i++)
    {
	if (info->pl_force.force_nr[i]==forcenr || force_slots.forces == 1)
	    if (info->pl_owner[i]==OWNER_COMPUTER || info->pl_owner[i]==OWNER_HUMAN || info->pl_owner[i]==OWNER_NEUTRAL ||
		info->pl_race[i]==RACE_OBSERVER||info->pl_iowner[i]==OWNER_RESCUABLE)
		nrpl++;
    }
    return(nrpl);
}
//=================================================
void observers_prepare(mapinfo *info)		//make vision for objserver players
{
    int i,j;
    for (i=0;i<PLAYEDPLAYERS;i++)
    {
	if (info->pl_race[i]==RACE_OBSERVER)
	{
	    for (j=0;j<PLAYEDPLAYERS;j++)
		if (info->pl_owner[j] == OWNER_COMPUTER || info->pl_owner[j] == OWNER_HUMAN)
		    CreateVision(info,i,j,PLAYER_VISION);
	}
    }
    
}
//=================================================
int GetPlayableGameNumber(mapinfo *info)
{
    int i=-1;
    for (i=0;i<PLAYEDPLAYERS;i++)
    {
	if (info->pl_owner[i]==OWNER_HUMAN)
	    break;
    }
    return(i);
}
//=================================================
void free_missionobjectives(void)
{
    if (missionobj)
    {
	wfree(missionobj);
	missionobj=NULL;
    }	
}
//=================================================
void fill_missionobjectives(char *missiontxt)
{
    free_missionobjectives();
    missionobj=(char *)wmalloc(strlen(missiontxt)+1);
    strcpy(missionobj,missiontxt);
}
//=================================================
LEADERBOARD *AddLeaderBoard(int boardnr,int leaderboardtype,struct mapinfo *info,int actiononplayers,int unittype,int nrofunits,int stringID)
{
    int i;
    char *strtxt;
    strtxt=getmapSTR(info,stringID);
    //commandsymb+emptycubesymb+cubecolor+space+number(max 10 symbols)+zero termstring
    if (!info->leaderboards[boardnr])
    {
	info->leaderboards[boardnr]=(LEADERBOARD *)wmalloc(sizeof(LEADERBOARD));
	info->leaderboards[boardnr]->txtstr=(char *)wmalloc(strlen(strtxt)+5+10);
    }
    info->leaderboards[boardnr]->leaderboardtype=leaderboardtype;
    info->leaderboards[boardnr]->unittype=unittype;
    info->leaderboards[boardnr]->nrofunits=nrofunits;
    info->leaderboards[boardnr]->stringID=stringID;
    if (!TRIG_leaderboardcomputerplayers)
    {
	//exclude computerplayers from calculations;
	for (i=0;i<MAXPLAYERS;i++)
	{
	    if (info->pl_iowner[i]!=OWNER_HUMAN)
	    {
		actiononplayers&=~(1<<i);
	    }
	}
    }
    info->leaderboards[boardnr]->actiononplayers=actiononplayers;
    return(info->leaderboards[boardnr]);
}
//=================================================
void CalcLeaderBoards(struct mapinfo *info)
{
    int i,j,mask,nrunits,ore,gas;
    for (i=0;i<MAXLEADERBOARDS;i++)
    {
	if (info->leaderboards[i])
	{
	    switch (info->leaderboards[i]->leaderboardtype)
	    {
		case TRG_ACTIONTYPE_LEADERBOARDCONTROL://17
		    nrunits=CheckForUnit(NULL, info->leaderboards[i]->actiononplayers,info->leaderboards[i]->unittype,0,NULL,NULL);
		    sprintf(info->leaderboards[i]->txtstr,COMMANDSYMBCHAR EMPTYCUBE14X14CHAR "%c %d %s",
				    PLAYER[NUMBGAMER].colorRACE+1,nrunits,getmapSTR(info,info->leaderboards[i]->stringID));
		    info->leaderboards[i]->calcready=1;
		    break;
		case TRG_ACTIONTYPE_LEADERBOARDRESOURCES://19
		    ore=0;
		    gas=0;
		    switch(info->leaderboards[i]->unittype)
		    {
			case TRG_RESTYPE_MINERALS:
			    for (j=0,mask=1;j<MAXPLAYERS;j++,mask<<=1)
				if (info->leaderboards[i]->actiononplayers&mask)
				    ore+=PLAYER[j].minerals>>8;
			    sprintf(info->leaderboards[i]->txtstr,COMMANDSYMBCHAR MINERALSYMBCHAR " %d %s",
					    ore,getmapSTR(info,info->leaderboards[i]->stringID));
			    break;
			case TRG_RESTYPE_GAS:
			    for (j=0,mask=1;j<MAXPLAYERS;j++,mask<<=1)
				if (info->leaderboards[i]->actiononplayers&mask)
				    gas+=PLAYER[NUMBGAMER].gas>>8;
			    sprintf(info->leaderboards[i]->txtstr,COMMANDSYMBCHAR GASSYMBCHAR " %d %s",
					    gas,getmapSTR(info,info->leaderboards[i]->stringID));
			    break;
			case TRG_RESTYPE_MINERALSANDGAS:
			    for (j=0,mask=1;j<MAXPLAYERS;j++,mask<<=1)
				if (info->leaderboards[i]->actiononplayers&mask)
				{
				    ore=PLAYER[NUMBGAMER].minerals>>8;
				    gas=PLAYER[NUMBGAMER].gas>>8;
				}
			    sprintf(info->leaderboards[i]->txtstr,COMMANDSYMBCHAR MINERALSYMBCHAR " %d " COMMANDSYMBCHAR GASSYMBCHAR " %d %s",
					    ore,gas,getmapSTR(info,info->leaderboards[i]->stringID));
			    break;
		    }
		    info->leaderboards[i]->calcready=1;
		    break;
		case TRG_ACTIONTYPE_LEADERBOARDPOINTS://21
		    nrunits=0;
		    for (j=0,mask=1;j<MAXPLAYERS;j++,mask<<=1)
			if (info->leaderboards[i]->actiononplayers&mask)
			    nrunits += info->units_stat.score[j][TRG_SCORETYPE_CUSTOM];
		    sprintf(info->leaderboards[i]->txtstr,COMMANDSYMBCHAR EMPTYCUBE14X14CHAR "%c %d %s",
				    PLAYER[NUMBGAMER].colorRACE+1,nrunits,getmapSTR(info,info->leaderboards[i]->stringID));
		    info->leaderboards[i]->calcready=1;
		    break;
		case TRG_ACTIONTYPE_LEADERBOARDGOALKILLS://36
		    nrunits=0;
		    for (j=0,mask=1;j<MAXPLAYERS;j++,mask<<=1)
			if (!(info->leaderboards[i]->actiononplayers&mask))
			    nrunits += info->units_stat.stattype[UNITSTAT_DEATH][j][info->leaderboards[i]->unittype];
		    nrunits=info->leaderboards[i]->nrofunits-nrunits;
		    sprintf(info->leaderboards[i]->txtstr,COMMANDSYMBCHAR EMPTYCUBE14X14CHAR "%c %d %s",
				    PLAYER[NUMBGAMER].colorRACE+1,nrunits,getmapSTR(info,info->leaderboards[i]->stringID));
		    info->leaderboards[i]->calcready=1;
		    break;
		case TRG_ACTIONTYPE_LEADERBOARDGOALPOINTS://37
		    nrunits=0;
		    for (j=0,mask=1;j<MAXPLAYERS;j++,mask<<=1)
			if (info->leaderboards[i]->actiononplayers&mask)
			    nrunits += info->units_stat.score[j][TRG_SCORETYPE_CUSTOM];
		    nrunits=info->leaderboards[i]->nrofunits-nrunits;
		    sprintf(info->leaderboards[i]->txtstr,COMMANDSYMBCHAR EMPTYCUBE14X14CHAR "%c %d %s",
				    PLAYER[NUMBGAMER].colorRACE+1,nrunits,getmapSTR(info,info->leaderboards[i]->stringID));
		    info->leaderboards[i]->calcready=1;
		    break;
		default:
		    break;
	    }
	}
    }    
}
//=================================================
void ShowLeaderBoards(struct mapinfo *info,int x,int y)
{
    int i,rowsize,fontnr;
    fontnr=IDFONT12;
    getmaxsymbolsize(fontnr,NULL,&rowsize);
    for (i=0;i<MAXLEADERBOARDS;i++)
    {
	if (info->leaderboards[i]&&info->leaderboards[i]->calcready)
	{
	    putmessage(x,y,fontnr,info->leaderboards[i]->txtstr,GGREENCOLORFONT,tfontgamp,gamedlggrp);
	    y+=rowsize+5;
	}
    }
}
//=================================================
void ShowCountDownTimer(struct mapinfo *info,int x,int y)
{
    char timer[10];
    int i,rowsize,fontnr;
    if (info->flags&STARMAP_FLAG_HAVECOUNTDOWN)
    {
	fontnr=IDFONT16;
	getmaxsymbolsize(fontnr,NULL,&rowsize);
	sprintf(timer,"%02d:%02d",info->CountDownTimer/MAXGAMECYCLESPERSECOND/60,(info->CountDownTimer/MAXGAMECYCLESPERSECOND)%60);
	putmessage(x,y,fontnr,timer,GWHITECOLORFONT,tfontgamp,gamedlggrp);
    }
}

//=================================================


