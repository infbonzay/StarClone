/*
    grp-lib v. 0.4.20
    Created by Botnary Vasya 
    Copyright 2018
    
    wrapper function for different grp functions
        
*/
#include "grplib/grp.h"
#include "grplib/usegrp.h"
#include "grplib/gr8.h"

//=======================================
//void [i]putgrpspr(int x,int y,GRPFILE *adr,int format,
//               int maxcolor,int grcolor,char *tablgrd,int nrpicture)
//
//=======================================
void putgrpspr(int x,int y,GRPFILE *adr,int format,
               int maxcolor,int grcolor,char *tablgrd,int nrpicture)
{
     if (nrpicture>=adr->CountPictures)		//if non-existed picture 
        return;					//do not put nothing
    //put sprite depend on format
    switch(format)
    {
	case NORMAL:
    	    putgrp(x,y,adr,nrpicture,grcolor<<8);
            break;
        case TRANSPARENT:
    	    if (!maxcolor)
            {
                   if (!tablgrd)
                        putgrp(x,y,adr,nrpicture,grcolor<<8);
                   else
                        putgrpgrd(x,y,adr,nrpicture,tablgrd);
            }
            else
            {
                    GRP_gr_maxcolordesen=maxcolor;
                    putgrptransp(x,y,adr,nrpicture,grcolor<<8);
            }
            break;
        case DISTORTION:
    	    if (!maxcolor)
            {
                    if (!tablgrd)
                        putgrp(x,y,adr,nrpicture,grcolor<<8);
            	    else
                	putgrpgrd(x,y,adr,nrpicture,tablgrd);
            }
            else
            {
            	    GRP_gr_maxcolordesen=maxcolor;
            	    putgrpnonvis(x,y,adr,nrpicture);
            }
            break;
	case SHADOW:
    	    putgrpshadow(x,y,adr,nrpicture,tablgrd,maxcolor<<8);
            break;
        case TOTABLE:
	    putgrptotable(x,y,adr,nrpicture,tablgrd,maxcolor<<8,grcolor<<8);
            break;
        case GLOW:
            if (tablgrd)
	        putgrpgrd(x,y,adr,nrpicture,tablgrd);
	    else
        	putgrp(x,y,adr,nrpicture,grcolor<<8);
            break;
    }
}
//=======================================
//inversion function mirror y-axis
//put inversion sprite same as 'putgrpspr' but sprite as mirror
void iputgrpspr(int x,int y,GRPFILE *adr,int format,
               int maxcolor,int grcolor,char *tablgrd,int nrpicture)
{
     if (nrpicture>=adr->CountPictures)
        return;
//     if (adr->SizeX>255)			//not show unpacked grp
//        return;
    switch(format)
    {
           case NORMAL:
                iputgrp(x,y,adr,nrpicture,grcolor<<8);
                break;
           case TRANSPARENT:
                if (!maxcolor)
                {
                   if (!tablgrd)
                      iputgrp(x,y,adr,nrpicture,grcolor<<8);
                   else
                      iputgrpgrd(x,y,adr,nrpicture,tablgrd);
                }
                else
                {
                   GRP_gr_maxcolordesen=maxcolor;
                   iputgrptransp(x,y,adr,nrpicture,grcolor<<8);
                }
                break;
           case DISTORTION:
                if (!maxcolor)
                {
            	    if (!tablgrd)
                       iputgrp(x,y,adr,nrpicture,grcolor<<8);
                    else
                       iputgrpgrd(x,y,adr,nrpicture,tablgrd);
                }
                else
                {
                    GRP_gr_maxcolordesen=maxcolor;
                    iputgrpnonvis(x,y,adr,nrpicture);
                }
                break;
           case SHADOW:
                iputgrpshadow(x,y,adr,nrpicture,tablgrd,maxcolor<<8);
                break;
           case TOTABLE:
		iputgrptotable(x,y,adr,nrpicture,tablgrd,maxcolor<<8,grcolor<<8);
                break;
           case GLOW:
                if (tablgrd)
		    iputgrpgrd(x,y,adr,nrpicture,tablgrd);
		else
                    iputgrp(x,y,adr,nrpicture,grcolor<<8);
                break;
    }
}
//=======================================


