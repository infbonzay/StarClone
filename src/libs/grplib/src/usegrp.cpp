/*
    grp-lib v. 0.4.21
    Created by Botnary Vasya 
    Copyright 2018
    
    wrapper function for different grp functions
        
*/
#include "grplib/grp.h"
#include "grplib/usegrp.h"
#include "grplib/gr8.h"

//=======================================
//void [i]GRP_PutGrp(int x,int y,GRPFILE *adr,int format,
//               int maxcolor,int grcolor,char *tablgrd,int nrpicture)
//
//=======================================
void GRP_PutGrp(int x,int y,GRPFILE *adr,int format,
               int maxcolor,int grcolor,char *tablgrd,int nrpicture)
{
     if (nrpicture >= adr->CountPictures)		//if non-existed picture 
        return;					//do not put nothing
    //put sprite depend on format
    switch(format)
    {
	case NORMAL:
		GRP_Put(x,y,adr,nrpicture,grcolor << 8);
        break;
    case TRANSPARENT:
    	if (!maxcolor)
        {
            if (!tablgrd)
                GRP_Put(x,y,adr,nrpicture,grcolor << 8);
            else
                GRP_PutGrd(x,y,adr,nrpicture,tablgrd);
        }
        else
        {
            GRP_gr_maxcolordesen = maxcolor;
            GRP_PutTransp(x,y,adr,nrpicture,grcolor<<8);
        }
        break;
    case DISTORTION:
		if (!maxcolor)
        {
            if (!tablgrd)
                GRP_Put(x,y,adr,nrpicture,grcolor << 8);
			else
             	GRP_PutGrd(x,y,adr,nrpicture,tablgrd);
        }
        else
        {
			GRP_gr_maxcolordesen = maxcolor;
            GRP_PutInv(x,y,adr,nrpicture);
        }
        break;
	case SHADOW:
    	GRP_PutShadow(x,y,adr,nrpicture,tablgrd,maxcolor <<8 );
        break;
    case TOTABLE:
	    GRP_PutByTable(x,y,adr,nrpicture,tablgrd,maxcolor << 8,grcolor << 8);
        break;
    case GLOW:
        if (tablgrd)
			GRP_PutGrd(x,y,adr,nrpicture,tablgrd);
	    else
        	GRP_Put(x,y,adr,nrpicture,grcolor << 8);
        break;
    }
}
//=======================================
//inversion function mirror y-axis
//put inversion sprite same as 'GRP_PutGrp' but sprite as mirror
void GRP_IPutGrp(int x,int y,GRPFILE *adr,int format,
               int maxcolor,int grcolor,char *tablgrd,int nrpicture)
{
    if (nrpicture >= adr->CountPictures)
        return;
    switch(format)
    {
    case NORMAL:
        GRP_IPut(x,y,adr,nrpicture,grcolor << 8);
        break;
    case TRANSPARENT:
        if (!maxcolor)
        {
			if (!tablgrd)
                GRP_IPut(x,y,adr,nrpicture,grcolor << 8);
			else
				GRP_IPutGrd(x,y,adr,nrpicture,tablgrd);
        }
        else
        {
            GRP_gr_maxcolordesen=maxcolor;
            GRP_IPutTransp(x,y,adr,nrpicture,grcolor << 8);
        }
        break;
    case DISTORTION:
        if (!maxcolor)
        {
       	    if (!tablgrd)
                GRP_IPut(x,y,adr,nrpicture,grcolor << 8);
            else
                GRP_IPutGrd(x,y,adr,nrpicture,tablgrd);
        }
        else
        {
            GRP_gr_maxcolordesen = maxcolor;
            GRP_IPutInv(x,y,adr,nrpicture);
        }
        break;
    case SHADOW:
        GRP_IPutShadow(x,y,adr,nrpicture,tablgrd,maxcolor << 8);
        break;
    case TOTABLE:
		GRP_IPutByTable(x,y,adr,nrpicture,tablgrd,maxcolor << 8,grcolor << 8);
        break;
    case GLOW:
        if (tablgrd)
			GRP_IPutGrd(x,y,adr,nrpicture,tablgrd);
		else
            GRP_IPut(x,y,adr,nrpicture,grcolor << 8);
        break;
    }
}
//=======================================


