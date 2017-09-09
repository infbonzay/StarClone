//============================================
//============================================
int IScriptCmd_playfram(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)	//0
{
    img->framenr = *((unsigned short *)&buf[0]);
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_playframtile(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)	//1
{
    img->framenr = *((unsigned short *)&buf[0]) + map.terrain;
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_sethorpos(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)	//2
{
    img->xdelta = (signed char )buf[0];
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_setvertpos(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)	//3
{
    img->ydelta = (signed char )buf[0];
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_setpos(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)		//4
{
    img->xdelta = (signed char )buf[0];
    img->ydelta = (signed char )buf[1];
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_wait(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)		//5
{
    img->waitticks = buf[0];
    img->offsetcmdinbuf += cmdsize;
    return(1);
}
//============================================
int IScriptCmd_waitrand(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)	//6
{
    if (!myrand(2))
        img->waitticks = buf[0];
    else
        img->waitticks = buf[1];
    img->offsetcmdinbuf += cmdsize;
    return(1);
}
//============================================
int IScriptCmd_goto(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)		//7
{
    img->offsetcmdinbuf = *((unsigned short *)&buf[0]);
    return(0);
}
//============================================
unsigned char BUNKERFIRELO[16]={0,1,1,2,2,3,3,4,4,4,5,5,6,6,7,7};
//============================================
int IScriptCmd_imgol(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)		//8
{
    OBJ *a,*tr,*subunit;
    OVERLAY_IMG *newimg;
    unsigned short image_id,flags,imagelo_id;
    unsigned char subunitnr;
    image_id = *((unsigned short *)&buf[0]);
    flags = img->flags;
    flags &= ~SC_IMAGE_FLAG_IMGOBJMAIN;
    if (img->flags & SC_IMAGE_FLAG_NEEDTOCREATESUBUNIT)		//need to create subunit
    {
	img->flags &= ~SC_IMAGE_FLAG_NEEDTOCREATESUBUNIT;
	a = img->parentimg->creator.objcreator.obj;
	CreateImageAndAddToList(a->subunit,GetOBJx256(a),GetOBJy256(a),3,NOLOIMAGE);
	a->subunit->mainimage->elevationlevel++;
    }
    else
    {
	if (img->parentimg->whocreate == SC_IMAGE_OBJ_CREATOR)
	{
	    a = img->parentimg->creator.objcreator.obj;
	    if (a)
	    {
		if (a->finalOBJ)
		{
		    tr = a->in_transport;
		    if (a->SC_Unit == SC_FIREBATOBJ && tr)
		    {
			flags &= ~SC_IMAGE_FLAG_DISABLEDRAW;
		    }
		}
	    }
	}
        newimg = new OVERLAY_IMG(img->parentimg,image_id,buf[2],buf[3],img->elevationlevel+1,flags | SC_IMAGE_FLAG_IMGOVER,ISCRIPTNR_INIT);
	iscriptinfo.ExecuteScript(newimg);
    }
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_imgul(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)		//9
{
    OVERLAY_IMG *newimg;
    unsigned short image_id,flags;
    flags = img->flags;
    flags &= ~SC_IMAGE_FLAG_IMGOBJMAIN;
    image_id = *((unsigned short *)&buf[0]);
    newimg = new OVERLAY_IMG(img->parentimg,image_id,buf[2],buf[3],img->elevationlevel-1,flags | SC_IMAGE_FLAG_IMGUNDER,ISCRIPTNR_INIT);
    iscriptinfo.ExecuteScript(newimg);
    
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_imgolorig(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)	//0a
{
    LOFILE *lo;
    signed char *adrxyoffs,xlo,ylo;
    unsigned char side;
    unsigned short image_id;//,imagelo_id;
    image_id = *((unsigned short *)&buf[0]);

    GetLoadedImage( alldattbl.images_dat->Special_Overlay[img->imageid],(void **)&lo);
    if (img->parentimg->side & 0x80)
    {
	side = (256 - img->parentimg->side) / 8;
	adrxyoffs = GetLoXY(lo,side,0);
	xlo = -adrxyoffs[0];
    }
    else
    {
        side = img->parentimg->side/8;
	adrxyoffs = GetLoXY(lo,side,0);
	xlo = adrxyoffs[0];
    }
    ylo = adrxyoffs[1];

    new OVERLAY_IMG(img->parentimg,image_id,xlo,ylo,img->elevationlevel+1,img->flags | SC_IMAGE_FLAG_IMGOVER,ISCRIPTNR_INIT);

    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_imgoluselo(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)	//0d
{
    OBJ *a,*subunit;
    unsigned short image_id,imagelo_id;
    unsigned char subunitnr;
    image_id = *((unsigned short *)&buf[0]);
    imagelo_id = GetIDFromOverlayLayer(img->imageid,buf[2]);
    if (img->flags & SC_IMAGE_FLAG_NEEDTOCREATESUBUNIT)		//need to create image for subunit
    {
	img->flags &= ~SC_IMAGE_FLAG_NEEDTOCREATESUBUNIT;
	a = img->parentimg->creator.objcreator.obj;
	CreateImageAndAddToList(a->subunit,GetOBJx256(a),GetOBJy256(a),3,imagelo_id);
	a->subunit->mainimage->side = TANKSIEGESIDE;
	a->subunit->mainimage->neededside = TANKSIEGESIDE;
	a->subunit->mainimage->elevationlevel++;
    }
    else
    {
	if (imagelo_id)
	    new OVERLAY_IMG(img->parentimg,image_id,imagelo_id,img->elevationlevel+1,img->flags | SC_IMAGE_FLAG_IMGOVER,ISCRIPTNR_INIT);
	else
	    new OVERLAY_IMG(img->parentimg,image_id,buf[2],buf[3],img->elevationlevel+1,img->flags | SC_IMAGE_FLAG_IMGOVER,ISCRIPTNR_INIT);
    }
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_sprol(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)		//0x0f
{
    MAIN_IMG *newimg;

    unsigned short image_id,sprite_id;
    unsigned short flags = SC_IMAGE_FLAG_IMGOBJMAIN;
    sprite_id = *((unsigned short *)&buf[0]);
    image_id = alldattbl.sprites_dat->images_id[sprite_id];

    if (img->flags & SC_IMAGE_FLAG_SAVEINFOGTABLE)
	flags |= SC_IMAGE_FLAG_SAVEINFOGTABLE;
    if (img->flags & SC_IMAGE_FLAG_AIRIMG)
	flags |= SC_IMAGE_FLAG_AIRIMG;

    newimg = new MAIN_IMG(  image_id,img->parentimg->xpos,img->parentimg->ypos,
			    img->elevationlevel + 1,
			    buf[2],buf[3],0,img->parentimg->imageusercolor,flags,ISCRIPTNR_INIT);
    newimg->whocreate = SC_IMAGE_SELF_CREATOR;
    mainimageslist.AddElem(newimg);
    
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_lowsprul(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)	//0x11
{
    MAIN_IMG *newimg;
    unsigned short image_id,sprite_id;
    unsigned short flags = SC_IMAGE_FLAG_IMGOBJMAIN;
    sprite_id = *((unsigned short *)&buf[0]);
    image_id = alldattbl.sprites_dat->images_id[sprite_id];

    if (img->flags & SC_IMAGE_FLAG_SAVEINFOGTABLE)
	flags |= SC_IMAGE_FLAG_SAVEINFOGTABLE;
    newimg = new MAIN_IMG(  image_id,img->parentimg->xpos,img->parentimg->ypos,
			    img->elevationlevel - 1,
			    buf[2],buf[3],0,img->parentimg->imageusercolor,flags,ISCRIPTNR_INIT);
    newimg->whocreate = SC_IMAGE_SELF_CREATOR;
    mainimageslist.AddElem(newimg);
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_spruluselo(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)	//14
{
    unsigned short image_id,sprite_id;
    unsigned short flags = SC_IMAGE_FLAG_IMGOBJMAIN;
    MAIN_IMG *newimg;
    sprite_id = *((unsigned short *)&buf[0]);
    image_id = alldattbl.sprites_dat->images_id[sprite_id];

    newimg = new MAIN_IMG(  image_id,img->parentimg->xpos,img->parentimg->ypos,
			    img->elevationlevel + 1,
			    buf[2],buf[3],img->parentimg->side,img->parentimg->imageusercolor,flags,ISCRIPTNR_INIT);
    mainimageslist.AddElem(newimg);		
    newimg->whocreate = SC_IMAGE_SELF_CREATOR;
    newimg->newgrpmethod = img->parentimg->newgrpmethod;	//copy grp method from parent

    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_sproluselo(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)	//15
{
    LOFILE *lo;
    unsigned short image_id,sprite_id;
    unsigned short flags = SC_IMAGE_FLAG_IMGOBJMAIN;
    unsigned char side;
    signed char *adrxyoffs,xlo,ylo;
    MAIN_IMG *newimg;
    sprite_id = *((unsigned short *)&buf[0]);
    image_id = alldattbl.sprites_dat->images_id[sprite_id];

    GetLoadedImage( alldattbl.images_dat->Attack_Overlay[img->imageid],(void **)&lo);
    if (img->parentimg->side & 0x80)
    {
	side = (256 - img->parentimg->side) / 8;
	adrxyoffs = GetLoXY(lo,side,0);
	xlo = -adrxyoffs[0];
    }
    else
    {
        side = img->parentimg->side/8;
	adrxyoffs = GetLoXY(lo,side,0);
	xlo = adrxyoffs[0];
    }
    ylo = adrxyoffs[1];
    newimg = new MAIN_IMG(  image_id,img->parentimg->xpos,img->parentimg->ypos,
			    img->elevationlevel + 1,
			    xlo,ylo,img->parentimg->side,img->parentimg->imageusercolor,flags,ISCRIPTNR_INIT);
    mainimageslist.AddElem(newimg);		
    newimg->whocreate = SC_IMAGE_SELF_CREATOR;

    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_end(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)		//16
{
    if (img->parentimg == img)
    {
	switch(img->parentimg->whocreate)
	{
	    case SC_IMAGE_OBJ_CREATOR:
		if (img->parentimg->creator.objcreator.obj)
		{
		    MarkForDestroy(img->parentimg->creator.objcreator.obj);
		    img->parentimg->whocreate = SC_IMAGE_UNKNOWN_CREATOR;
		    img->parentimg->creator.objcreator.obj = NULL;
    		}
    		else
		    DEBUGMESSCR("bad news END IMAGE OBJ we dont know who create the image :(\n");
		img->parentimg->DeleteMainImg(&mainimageslist);
		break;
	    case SC_IMAGE_FLINGY_CREATOR:
		if (img->parentimg->creator.flingycreator.flingy)
		{
		    img->parentimg->creator.flingycreator.flingy->DelFlingy();
		    img->parentimg->DeleteMainImg(&mainimageslist);
		    img->parentimg->creator.flingycreator.flingy=NULL;
		    img->parentimg->whocreate=SC_IMAGE_UNKNOWN_CREATOR;
		    return(1);
		}
		else
		    DEBUGMESSCR("flingy creator is null :(\n");
		img->parentimg->DeleteMainImg(&mainimageslist);
		break;
	    case SC_IMAGE_MINIMAP_CREATOR:
		img->parentimg->DeleteMainImg(&imageslist_minimap);
		break;
	    case SC_IMAGE_DOODAD_CREATOR:
		img->parentimg->DeleteMainImg(&imageslist_doodads);
		break;
	    case SC_IMAGE_SELF_CREATOR:
	    case SC_IMAGE_OVERLAYOBJ_CREATOR:
		img->parentimg->DeleteMainImg(&mainimageslist);
		break;
	    default:
		DEBUGMESSCR("bad news END IMAGE flingy creator unknown\n");
		break;
	}
    }
    else
    {
	img->DeleteChildImg();
    }

    img->offsetcmdinbuf += cmdsize;
    return(1);
}
//============================================
int IScriptCmd_setflipstate(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    if (buf[0])
    {
	img->flags |= SC_IMAGE_FLAG_MIRRORIMAGE;
    }
    else
    {
	img->flags &= ~SC_IMAGE_FLAG_MIRRORIMAGE;
    }
    
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_playsnd(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    unsigned short sndid = *((unsigned short *)&buf[0]);
//    if (!(img->flags & SC_IMAGE_FLAG_DISABLEDRAW))
    {
	Play_sfxdata(img->parentimg->xpos>>8,img->parentimg->ypos>>8,sndid,2);
    }
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_playsndrand(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    //cmdsize is 0x80
    unsigned char nrofrandomsnds,sndnr;
    unsigned short sndid;
//    if (!(img->flags & SC_IMAGE_FLAG_DISABLEDRAW))
    {
	nrofrandomsnds = buf[0];
	sndnr = myrand(nrofrandomsnds);
	sndid = *((unsigned short *)&buf[1+sndnr*2]);
	Play_sfxdata(img->parentimg->xpos>>8,img->parentimg->ypos>>8,sndid,2);
    }
    img->offsetcmdinbuf += nrofrandomsnds * 2+1;
    return(0);
}
//============================================
int IScriptCmd_playsndbtwn(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    unsigned short sndid1,sndid2;
//    if (!(img->flags & SC_IMAGE_FLAG_DISABLEDRAW))
    {
	sndid1 = *((unsigned short *)&buf[0]);
	sndid2 = *((unsigned short *)&buf[2]);
	Play_sfxdata(img->parentimg->xpos>>8,img->parentimg->ypos>>8,sndid1+myrand(sndid2-sndid1+1),2);
    }
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_domissiledmg(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    SC_FLINGY *flingy;
    switch(img->parentimg->whocreate)
    {
	case SC_IMAGE_FLINGY_CREATOR:
	    //flingy weapon creator
	    flingy = img->parentimg->creator.flingycreator.flingy;
	    if (flingy)
	    {
		WeaponDoDamage( flingy->atacker,flingy->destobj,
				img->parentimg->xpos,img->parentimg->ypos,
				flingy->SC_Unit,
			        flingy->playernr,
				flingy->weapon_id,
				flingy->frommodemove,
				flingy->nroftargets,NULL);
	    }
	    break;
	default:
	    DEBUGMESSCR("unknown creator\n");
	    break;
    }

    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_attackmelee(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    OBJ *a;
    unsigned char nrofrandomsnds,sndnr;
    unsigned short sndid;
    
    nrofrandomsnds = buf[0];
    sndnr = myrand(nrofrandomsnds);
    sndid = *((unsigned short *)&buf[1+sndnr*2]);
    Play_sfxdata(img->parentimg->xpos>>8,img->parentimg->ypos>>8,sndid,2);
    
    if (img->parentimg->whocreate == SC_IMAGE_OBJ_CREATOR)
    {
	a = img->parentimg->creator.objcreator.obj;
    	if (a && a->finalOBJ)
	    CreateWeaponID( a,a->finalOBJ,img->parentimg->xpos,img->parentimg->ypos,alldattbl.units_dat->GroundWeapon[a->SC_Unit],0);
    }
    img->offsetcmdinbuf += nrofrandomsnds * 2+1;
    return(0);
}
//============================================
int IScriptCmd_followmaingraphic(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    if (img->parentimg)
    {
	img->framenr = img->parentimg->framenr;
    }

    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_randcondjmp(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    if (img->parentimg->invisiblecolors == MININVISIBLECOLOR)	//prevent to 'dance' if cloaked
    {
	if (myrand(256)<buf[0])
	{
	    img->offsetcmdinbuf = *((unsigned short *)&buf[1]);
	    return(0);
	}
    }
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_turnccwise(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    img->parentimg->side -= buf[0]*8;
    img->parentimg->neededside = img->parentimg->side;

    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_turncwise(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    img->parentimg->side += buf[0]*8;
    img->parentimg->neededside = img->parentimg->side;

    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_turn1cwise(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    img->parentimg->side += 8;
    img->parentimg->neededside = img->parentimg->side;

    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_turnrand(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    if (myrand(2))
	return(IScriptCmd_turncwise(img,buf,cmdsize));
    else
	return(IScriptCmd_turnccwise(img,buf,cmdsize));
}
//============================================
int IScriptCmd_setspawnframe(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    OVERLAY_IMG *tempimg;
    img->parentimg->UnitNeededDirection256(buf[0]*8);

    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_sigorder(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    int retvalue;

    img->offsetcmdinbuf += cmdsize;
    switch(img->parentimg->whocreate)
    {
	case SC_IMAGE_OBJ_CREATOR:
	case SC_IMAGE_OVERLAYOBJ_CREATOR:
	    if (img->parentimg->creator.objcreator.obj)
	    {
		if (img->parentimg->creator.objcreator.obj->OrderFunc)
		{
		    retvalue = (*img->parentimg->creator.objcreator.obj->OrderFunc)(img->parentimg->creator.objcreator.obj);
		    if (retvalue)
			return(1);
		}
		else
		    DEBUGMESSCR("imgid=%d OrderFunc is NULL\n",img->imageid);
	    }
	    else
		DEBUGMESSCR("objcreator is NULL\n");
	    break;
	case SC_IMAGE_FLINGY_CREATOR:
	    //DEBUGMESSCR("sigorder from flingy under developing\n");
	    //????
	    break;
	default:
	    DEBUGMESSCR("sigorder from non OBJ or flingy\n");
	    break;
    }
    return(0);
}
//============================================
int IScriptCmd_attackwith(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    unsigned char subunit;
    signed char *adrxyoffs;
    int retval;
    OBJ *a,*tr;
    LOFILE *lo;
    MAIN_IMG *newimg;
    unsigned int weapon_id;
    if (img->parentimg->whocreate == SC_IMAGE_OBJ_CREATOR)
    {
	a = img->parentimg->creator.objcreator.obj;
	if (a)
	{
	    switch(buf[0])
	    {
		case 1:
		    weapon_id = alldattbl.units_dat->GroundWeapon[a->SC_Unit];
		    break;
		case 2:
		    weapon_id = alldattbl.units_dat->AirWeapon[a->SC_Unit];
		    break;
		default:
		    weapon_id = 0;
		    DEBUGMESSCR("SC_Unit = %d attack with != 1 or 2 \n",a->SC_Unit);
		    break;
	    }
	    if (a->finalOBJ)
	    {
		//show fire bunker if marine inside
		tr = a->in_transport;
		if (a->SC_Unit == SC_MARINEOBJ && tr)
		{
		    unsigned char side = CalcDirection1(GetOBJx256(tr),GetOBJy256(tr),GetOBJx256(a->finalOBJ),GetOBJy256(a->finalOBJ));
		    tr->mainimage->AllUnitDirection256(side);
		    
		    GetLoadedImage( alldattbl.images_dat->Attack_Overlay[tr->mainimage->imageid],(void **)&lo);
		    adrxyoffs = GetLoXY(lo,0,BUNKERFIRELO[tr->mainimage->side/16]);
		
		    newimg = new MAIN_IMG(IMAGEID_BUNKERFIREOVERLAY,GetOBJx256(tr),GetOBJy256(tr),
					    tr->mainimage->elevationlevel + 1,
					    adrxyoffs[0],adrxyoffs[1],side,tr->mainimage->imageusercolor,
					    tr->mainimage->flags | SC_IMAGE_FLAG_IMGOVER,ISCRIPTNR_INIT);
		    newimg->whocreate = SC_IMAGE_SELF_CREATOR;
		    mainimageslist.AddElem(newimg);

		}
		retval = CreateWeaponID( a,a->finalOBJ,GetOBJx256(a->finalOBJ),GetOBJy256(a->finalOBJ),weapon_id,1);
		if (retval)
		    return(1);
	    }
	}
    }

    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_attack(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    int retval;
    OBJ *a;
    unsigned char weapon_id;
    if (img->parentimg->whocreate == SC_IMAGE_OBJ_CREATOR)
    {
	a = img->parentimg->creator.objcreator.obj;
	if (a && a->finalOBJ)			//atacking air or ground depend on destination unit
	{
	    if (!IsOnSkyOBJ(a->finalOBJ))
		weapon_id = alldattbl.units_dat->GroundWeapon[a->SC_Unit];
	    else
		weapon_id = alldattbl.units_dat->AirWeapon[a->SC_Unit];
	    retval = CreateWeaponID( a,a->finalOBJ,GetOBJx256(a->finalOBJ),GetOBJy256(a->finalOBJ),weapon_id,1);
	    if (retval)
	        return(1);
	}
    }

    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_castspell(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    switch(img->parentimg->whocreate)
    {
	case SC_IMAGE_OBJ_CREATOR:
	case SC_IMAGE_OVERLAYOBJ_CREATOR:
	    if (img->parentimg->creator.objcreator.obj)
	    {
		CastSpell(img->parentimg->creator.objcreator.obj);
	    }
	    break;
	default:
	    DEBUGMESSCR("cast spell from non OBJ\n");
	    break;
    }

    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_useweapon(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    OBJ *a;
    if (img->parentimg->whocreate == SC_IMAGE_OBJ_CREATOR)
    {
	a = img->parentimg->creator.objcreator.obj;
    	if (a)
	    CreateWeaponID( a,a->finalOBJ,img->parentimg->xpos,img->parentimg->ypos,buf[0],1);
    }
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_move(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    unsigned char previscriptnr;
    int status;
//    printf("deltamove=%d\n",buf[0]);
    previscriptnr = img->iscriptnr;
    status = moveaction(img->parentimg,buf[0]<<8);
    switch(status)
    {
        case 2://if changes iscripts
    	    if (previscriptnr == img->iscriptnr)
		img->offsetcmdinbuf += cmdsize;
    	    return(1);
	case 0:
	    //if no moveaction(path finding tell us about no path or rotation needed) we do not go to the next script command
	    //but next cycle we parce this script again
	    img->offsetcmdinbuf--;
	    return(1);
    }
    //here we moved correctly
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_gotorepeatattk(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    OBJ *a;
    a = img->parentimg->creator.objcreator.obj;
    a->atackcooldowntime = GetAtackCoolDown(a,a->usedweapon_id);
    if (img->parentimg->whocreate == SC_IMAGE_OBJ_CREATOR)
    {
	a = img->parentimg->creator.objcreator.obj;
    	if (a)
    	    if (ApplyNextModeMove(a));
    }

    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_engframe(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    img->framenr = buf[0];
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_engset(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    img->framenr = buf[0] * 17;
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_hideimg(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    img->HideImgFlag();
    img->DisableExecScript();

    img->offsetcmdinbuf = 0;	//waits for every iscriptnr
//    img->offsetcmdinbuf += cmdsize;
    return(1);
}
//============================================
int IScriptCmd_nobrkcodestart(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    img->flags |= SC_IMAGE_FLAG_CANTBREAKCODE;
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_nobrkcodeend(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    OBJ *a;
    img->flags &= ~SC_IMAGE_FLAG_CANTBREAKCODE;
/*    if (img->parentimg->whocreate == SC_IMAGE_OBJ_CREATOR)
    {
	a = img->parentimg->creator.objcreator.obj;
    	if (a)
    	    if (ApplyNextModeMove(a));
    }
*/    
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_ignorerest(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    img->flags &= ~SC_IMAGE_FLAG_CANTBREAKCODE;

    img->offsetcmdinbuf--;
    return(1);
}
//============================================
//============================================
int IScriptCmd_attkshiftproj(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)		//attkshiftproj(signed char distfromimg)
{
    OBJ *a;
    signed char weaponxoffs,weaponyoffs;
    CalcXYOffsets(img->parentimg->side,buf[0],0,&weaponxoffs,&weaponyoffs);

    if (img->parentimg->whocreate == SC_IMAGE_OBJ_CREATOR)
    {
	a = img->parentimg->creator.objcreator.obj;
    	if (a && a->finalOBJ)
	    CreateWeaponID( a,a->finalOBJ,img->parentimg->xpos+(weaponxoffs<<8),img->parentimg->ypos+(weaponyoffs<<8),
			    alldattbl.units_dat->GroundWeapon[a->SC_Unit],1);
    }

    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_tmprmgraphicstart(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    img->HideImgFlag();

    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_tmprmgraphicend(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    img->ShowImgFlag();

    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_setfldirect(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    img->parentimg->side = buf[0] * 8;
    img->parentimg->neededside = img->parentimg->side;

    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_call(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    img->returnaddress = img->offsetcmdinbuf + cmdsize;
    img->offsetcmdinbuf = *((unsigned short *)&buf[0]);
    return(0);
}
//============================================
int IScriptCmd_return(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    img->offsetcmdinbuf = img->returnaddress;
    return(0);
}
//============================================
int IScriptCmd_setflspeed(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    int status;
    unsigned short distance;
    distance = *((unsigned short *)&buf[0]);
    if (distance)
    {
        status = moveaction(img->parentimg,distance);
        switch(status)
        {
            case 2://changes iscripts
	        return(1);
	    case 0:
		//if no moveaction(path finding tell us about no path or rotation needed) we do not go to the next script command
		//but next cycle we parce this script again
	        img->offsetcmdinbuf--;
		return(1);
	}
    }
    //here we moved correctly
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_creategasoverlays(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    OVERLAY_IMG *newimg;
    LOFILE *lo;
    unsigned short overlayimage_id,newflags,imagelo_tbl;
    signed char *adrxyoffs,xlo,ylo;

    unsigned char overlaynr = buf[0];
    GetLoadedImage( alldattbl.images_dat->Special_Overlay[img->imageid],(void **)&lo);
    adrxyoffs = GetLoXY(lo,img->framenr,overlaynr);
    xlo = adrxyoffs[0];
    ylo = adrxyoffs[1];
//    newflags = img->flags;
    newflags = SC_IMAGE_FLAG_SAVEINFOGTABLE;
    newflags |= SC_IMAGE_FLAG_IMGOVER;
    if (IfGeyserIsDepleted(img->parentimg->creator.objcreator.obj))
	overlayimage_id =  IMAGEID_DEPLETEDGEYSERSMOKE1;
    else
	overlayimage_id =  IMAGEID_GEYSERSMOKE1;
    newimg = new OVERLAY_IMG(img->parentimg,overlayimage_id+overlaynr,xlo,ylo,img->elevationlevel+1,newflags,ISCRIPTNR_INIT);
    
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_pwrupcondjmp(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    OBJ *a = img->parentimg->creator.objcreator.obj;
    if (IsPickupUnit(a->SC_Unit) && a->carryobj)
    {
	img->offsetcmdinbuf = *((unsigned short *)&buf[0]);
	return(0);
    }
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_trgtrangecondjmp(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    OBJ *a,*destobj;
    switch(img->parentimg->whocreate)
    {
	case SC_IMAGE_OBJ_CREATOR:
	    a = img->parentimg->creator.objcreator.obj;
	    destobj = a->finalOBJ;
    	    if (a && destobj)
    	    {
		if (GetDistances(GetOBJx(a),GetOBJy(a),GetOBJx(destobj),GetOBJy(destobj)) <= buf[0] )
		{
		    img->offsetcmdinbuf = *((unsigned short *)&buf[1]);
		    return(0);
		}
	    }
	    break;
	case SC_IMAGE_FLINGY_CREATOR:
	    a = img->parentimg->creator.flingycreator.flingy->atacker;
	    destobj = img->parentimg->creator.flingycreator.flingy->destobj;
    	    if (a && destobj)
    	    {
		if (GetDistances(GetOBJx(a),GetOBJy(a),GetOBJx(destobj),GetOBJy(destobj)) <= buf[0] )
		{
		    img->offsetcmdinbuf = *((unsigned short *)&buf[1]);
		    return(0);
		}
	    }
	    break;
	default:
	    DEBUGMESSCR("wrong trgtrangecondjmp not from OBJ_CREATOR or OBJ_FLINGY\n");
	    break;	    
    }

    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_trgtarccondjmp(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    OBJ *a,*destobj;
    switch(img->parentimg->whocreate)
    {
	case SC_IMAGE_OBJ_CREATOR:
	    a = img->parentimg->creator.objcreator.obj;
	    destobj = a->finalOBJ;
    	    if (a && destobj)
    	    {
		if (CalcDirection1(GetOBJx256(a),GetOBJy256(a),GetOBJx256(destobj),GetOBJy256(destobj)) <= buf[0] )
		{
		    img->offsetcmdinbuf = *((unsigned short *)&buf[2]);
		    return(0);
		}
	    }
	    break;
	default:
	    DEBUGMESSCR("wrong trgtrangecondjmp not from OBJ_CREATOR\n");
	    break;	    
    }

    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_curdirectcondjmp(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    if (img->parentimg->side == buf[0])
    {
	img->offsetcmdinbuf = *((unsigned short *)&buf[2]);
	return(0);
    }
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_imgulnextid(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    new OVERLAY_IMG(img->parentimg,img->imageid+1,buf[0],buf[1],img->elevationlevel-1,img->flags | SC_IMAGE_FLAG_IMGUNDER,ISCRIPTNR_INIT);

    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_liftoffcondjmp(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    if (img->parentimg->whocreate == SC_IMAGE_OBJ_CREATOR)
    {
	if (img->parentimg->creator.objcreator.obj)
	{
	    if (IsOnSkyOBJ(img->parentimg->creator.objcreator.obj))
	    {
		img->offsetcmdinbuf = *((unsigned short *)&buf[0]);
		return(0);
	    }
	}
	else
	    DEBUGMESSCR("objcreator is NULL\n");
    }
    else
	DEBUGMESSCR("no objcreator\n");
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_warpoverlay(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    img->grpmethod = WARPTEXTURE;
    img->protossbuildwhite = buf[0];
    img->framenr = 0;

    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_orderdone(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    if (img->parentimg->whocreate == SC_IMAGE_OBJ_CREATOR)
    {
	if (img->parentimg->creator.objcreator.obj)
	{
	    SetOrder(img->parentimg->creator.objcreator.obj,buf[0],NULL);		//remove order
	}
    }
    
    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_dogrddamage(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    SC_FLINGY *flingy;
    switch(img->parentimg->whocreate)
    {
	case SC_IMAGE_FLINGY_CREATOR:
	    //flingy weapon creator
	    flingy = img->parentimg->creator.flingycreator.flingy;
	    if (flingy)
	    {
		WeaponDoDamage( flingy->atacker,flingy->destobj,
				img->parentimg->xpos,img->parentimg->ypos,
				flingy->SC_Unit,
				flingy->playernr,
				flingy->weapon_id,
				flingy->frommodemove,
				flingy->nroftargets,
				flingy->listdamaged);
	    }
	    break;
	default:
	    DEBUGMESSCR("unknown creator\n");
	    break;
    }

    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int IScriptCmd_errorcmd(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    unsigned char cmd = (int ) (*(buf-1));
    img->offsetcmdinbuf--;
    printf("img[%d] cmd[0x%02x][%s]-special error, offset cmd buffer is [0x%04x]\n",img->imageid,cmd,iscriptcmdstr[cmd],img->offsetcmdinbuf);
    return(1);
}
//============================================
int IScriptCmd_DEVELOPING(OVERLAY_IMG *img,unsigned char *buf,int cmdsize)
{
    unsigned char unknowncmd = (int ) (*(buf-1));
    printf("img[%d] unknown cmd[%02x][%s]\n",img->imageid,unknowncmd,iscriptcmdstr[unknowncmd]);

    img->offsetcmdinbuf += cmdsize;
    return(0);
}
//============================================
int (*IScriptCmd[ISCRIPTCMD_MAXVALUE])(OVERLAY_IMG *img,unsigned char *buf,int cmdsize) = 
{
		&IScriptCmd_playfram,				//0x00
		&IScriptCmd_playframtile,                       //0x01
		&IScriptCmd_sethorpos,                          //0x02
		&IScriptCmd_setvertpos,                         //0x03
		&IScriptCmd_setpos,                             //0x04
		&IScriptCmd_wait,                               //0x05
		&IScriptCmd_waitrand,                           //0x06
		&IScriptCmd_goto,                               //0x07
		&IScriptCmd_imgol,                              //0x08
		&IScriptCmd_imgul,                              //0x09
		&IScriptCmd_imgolorig,                          //0x0a
		&IScriptCmd_DEVELOPING,                         //0x0b
		&IScriptCmd_DEVELOPING,                         //0x0c
		&IScriptCmd_imgoluselo,                         //0x0d
		&IScriptCmd_DEVELOPING,                         //0x0e
		&IScriptCmd_sprol,                              //0x0f
		&IScriptCmd_DEVELOPING,                         //0x10
		&IScriptCmd_lowsprul,                           //0x11
		&IScriptCmd_DEVELOPING,                         //0x12
		&IScriptCmd_spruluselo,                         //0x13
		&IScriptCmd_DEVELOPING,                         //0x14
		&IScriptCmd_sproluselo,                         //0x15
		&IScriptCmd_end,	                        //0x16
                &IScriptCmd_setflipstate,                       //0x17
                &IScriptCmd_playsnd,                            //0x18
                &IScriptCmd_playsndrand,                        //0x19
                &IScriptCmd_playsndbtwn,                        //0x1a
                &IScriptCmd_domissiledmg,                       //0x1b
                &IScriptCmd_attackmelee,                        //0x1c
                &IScriptCmd_followmaingraphic,                  //0x1d
                &IScriptCmd_randcondjmp,                        //0x1e
                &IScriptCmd_turnccwise,                         //0x1f
                &IScriptCmd_turncwise,                          //0x20
                &IScriptCmd_turn1cwise,                         //0x21
                &IScriptCmd_turnrand,                         	//0x22
                &IScriptCmd_setspawnframe,                      //0x23
                &IScriptCmd_sigorder,                           //0x24
                &IScriptCmd_attackwith,                         //0x25
                &IScriptCmd_attack,                             //0x26
                &IScriptCmd_castspell,                          //0x27
                &IScriptCmd_useweapon,                          //0x28
                &IScriptCmd_move,	                        //0x29
                &IScriptCmd_gotorepeatattk,                     //0x2a
                &IScriptCmd_engframe,                           //0x2b
                &IScriptCmd_engset,                         	//0x2c
                &IScriptCmd_hideimg,                            //0x2d
                &IScriptCmd_nobrkcodestart,                     //0x2e
                &IScriptCmd_nobrkcodeend,                       //0x2f
                &IScriptCmd_ignorerest,                         //0x30
                &IScriptCmd_attkshiftproj,                      //0x31
                &IScriptCmd_tmprmgraphicstart,                  //0x32
                &IScriptCmd_tmprmgraphicend,                    //0x33
                &IScriptCmd_setfldirect,                        //0x34
                &IScriptCmd_call,                               //0x35
                &IScriptCmd_return,                             //0x36
                &IScriptCmd_setflspeed,                         //0x37
                &IScriptCmd_creategasoverlays,                  //0x38
                &IScriptCmd_pwrupcondjmp,                       //0x39
                &IScriptCmd_trgtrangecondjmp,                   //0x3a
                &IScriptCmd_trgtarccondjmp,                     //0x3b
                &IScriptCmd_curdirectcondjmp,                   //0x3c
                &IScriptCmd_imgulnextid,                        //0x3d
                &IScriptCmd_DEVELOPING,                         //0x3e
                &IScriptCmd_liftoffcondjmp,                     //0x3f
                &IScriptCmd_warpoverlay,                        //0x40
                &IScriptCmd_orderdone,                          //0x41
                &IScriptCmd_sprol,                         	//0x42
                &IScriptCmd_DEVELOPING,                         //0x43
                &IScriptCmd_dogrddamage,                        //0x44
                &IScriptCmd_errorcmd,                           //0x45
};
//============================================
