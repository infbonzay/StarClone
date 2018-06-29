
#include <stdlib.h>
#include <string.h>
#include "defsunits.h"

#include "vars.h"
#include "wmem.h"
#include "mpq.h"
#include "fonts.h"
#include "gamemenu.h"
#include "mytime.h"
#include "tbl.h"
#include "dialogbin.h"

//#define SHOWMENUINFO
//==========================================
int DialogBin_GetFontNr(int flags)
{
    int fontnr=-1;
    if (flags&DIALOGBIN_FLAGS_FONT10)
        fontnr=IDFONT10;
    else
        if (flags&DIALOGBIN_FLAGS_FONT14)
    	    fontnr=IDFONT14;
        else
	    if (flags&DIALOGBIN_FLAGS_FONT16)
		fontnr=IDFONT16;
	    else
	        if (flags&DIALOGBIN_FLAGS_FONT16X)
	    	    fontnr=IDFONT16X;
		else
		    if (flags&DIALOGBIN_FLAGS_FONT50)
		        fontnr=IDFONT50;
    return(fontnr);
}
//==========================================
int GetDialogElements(char *dialogdata,int *groupofbuttons)
{
    int maxelem=0,lastelem=0,samebutton=0;
    int type,flags,ymin,yminprev=-1;
    DIALOGBIN *tempdialog;
    tempdialog=(DIALOGBIN *)(dialogdata+sizeof(DIALOGBIN));
    do
    {
	if (!tempdialog->NextEntryOffset)
	    lastelem=1;
	if (groupofbuttons)
	{
	    ymin=tempdialog->TopEdge;
	    type=tempdialog->ControlType;
	    flags=tempdialog->Flags;
	    if ((flags&DIALOGBIN_FLAGS_ITEMVISIBLED)&&(type==DIALOGBIN_BUTTON||type==DIALOGBIN_DEFAULTBUTTON))
	    {
		if (yminprev==ymin)
		    samebutton++;
		else
		    yminprev=ymin;
	    }
	}
	tempdialog=(DIALOGBIN *)(dialogdata+tempdialog->NextEntryOffset);
	maxelem++;
    }while(!lastelem);
    if (groupofbuttons)
    {
	if (samebutton)
	    samebutton++;
	*groupofbuttons=samebutton;
    }
    return(maxelem);
}
//==========================================
void UnloadDialogBin(MENUSTR *menu)
{
    removeallmenuinfo(menu);
}
//==========================================
void ShowMenuEdge(MENUSTR *menu)
{
    if (menu->mainmenuflags&DIALOGBIN_FLAGS_ITEMVISIBLED)
	puttransptilebox(menu->x,menu->y,menu->xsize,menu->ysize,gametilegrp);
}
//==========================================
MENUSTR *LoadDialogBin(char *dialogbinfilename,int dialog_opt,int defaultfontnr)
{
    return(LoadDialogBin(dialogbinfilename,dialog_opt,0,defaultfontnr));
}
//==========================================
MENUSTR *LoadDialogBin(char *dialogbinfilename,int dialog_opt,int addforreserv,int defaultfontnr)
{
    int i,buttoncolor,textcolor,radiocolor,checkboxcolor,horizbarcolor,listcolor,decorselecteditem;
    int lastelem=0,elemnr=0,parcedbuttons=0,firstradioelem=-1,firsttextitem=-1,firstlistitem=-1;
    int maxelem,type,flags,textxoffset,textyoffset,maxsymbx,maxsymby,groupbuttons,buttontype,unknown;
    int xmin,ymin,xsize,ysize,xwin,ywin,xwinsize,ywinsize,strxlen;
    int fontnr;

    SMK_ARRAY smkinfo[10]; 
    int nrofsmks=0,smkf[10],smktextx,smktexty;
    DIALOGBIN_SMK *tempsmk[10],*smkarray;

    
    char zerostring=0;
    char *dialogdata,*text;
    unsigned char hotkey,transparentcolor,translucencycolor;
    DIALOGBIN *tempdialog,*tempdialog2;
    MENUSTR *menu;
    if (mpqloadfile(dialogbinfilename,(char **)&dialogdata))
    {
	printf("error in %s\n",dialogbinfilename);
	return NULL;
    }
    maxelem=GetDialogElements(dialogdata,&groupbuttons);
#ifdef SHOWMENUINFO
    printf("dialog %s - elements=%d groupbuttons=%d\n",dialogbinfilename,maxelem,groupbuttons);
#endif
//first item is menu box
    tempdialog=(DIALOGBIN *)dialogdata;
    text=dialogdata+tempdialog->MainStringOffset;
    xwin=tempdialog->LeftEdge+DELTASCREENX;
    ywin=tempdialog->TopEdge+DELTASCREENY2;
//    xwin=tempdialog->LeftEdge;
//    ywin=tempdialog->TopEdge;
    type=tempdialog->ControlType;
    flags=tempdialog->Flags;
    xwinsize=tempdialog->Width;
    ywinsize=tempdialog->Height;
#ifdef SHOWMENUINFO
    printf("elem %d - (%dx%d) - (%dx%d) type=%d flags=%08x text= [%s]\n",elemnr,xwin,ywin,xwinsize,ywinsize,type,flags,text);
#endif
//    if (dialog_opt!=DIALOGBIN_TEST)
//    {
	if (type==DIALOGBIN_MAIN)
	{
	    menu=createmenuinfo(xwin,ywin,xwinsize,ywinsize,maxelem+addforreserv,dialog_opt&DIALOGBIN_DONOTSAVEUNDERMENU);
	    menu->mainmenuflags=flags;
	}
	if (dialog_opt&DIALOGBIN_MAINDIALOGS)
	    addsoundstomenu(menu,SFXDATA_MOUSEDOWN,SFXDATA_MOUSEOVER);
	if (dialog_opt&DIALOGBIN_GAMEDIALOGS)
	    addsoundstomenu(menu,SFXDATA_SOUNDSELECT,-1);
//    }
    tempdialog=(DIALOGBIN *)(dialogdata+sizeof(DIALOGBIN));
    //next items is controls
    do
    {
	for (i=0;i<10;i++)
	{
	    smkinfo[i].smkfile=NULL;
	}
	if (!tempdialog->NextEntryOffset)
	    lastelem=1;
	text=dialogdata+tempdialog->MainStringOffset;
	xmin=tempdialog->LeftEdge;
	ymin=tempdialog->TopEdge;
	xsize=tempdialog->Width;
	ysize=tempdialog->Height;
	type=tempdialog->ControlType;
	flags=tempdialog->Flags;
	fontnr=DialogBin_GetFontNr(flags);
//	if (dialog_opt!=DIALOGBIN_TEST)
	    if (fontnr==-1)
		fontnr=defaultfontnr;
	if (flags&(DIALOGBIN_FLAGS_HOTKEYFIRSTCHARACTER|DIALOGBIN_FLAGS_VIRTHOTKEY))
	{
	    hotkey=text[0];
	    text++;
	}
	else
	    hotkey=255;

	tempdialog2=tempdialog;
	smktextx=0;
	smktexty=0;
	i=0;
	int smkeventxsize=tempdialog->RespondAreaWidth;
	if (!smkeventxsize)
	    smkeventxsize=xsize;
	int smkeventysize=tempdialog->RespondAreaHeight;
	if (!smkeventysize)
	    smkeventysize=ysize;
	if (tempdialog->SMK_Offset)
	{
	    smktextx=xwin+tempdialog->TextOffsetX;
	    smktexty=ywin+tempdialog->TextOffsetY;
	    smkarray=(DIALOGBIN_SMK *)(dialogdata+tempdialog->SMK_Offset);
	    do{
		tempsmk[i]=smkarray;
		smkinfo[i].smkfile=(char *)(dialogdata+tempsmk[i]->SMK_StringOffset);
		smkinfo[i].posx_smk=xmin+tempsmk[i]->SMK_OverlayXPos;
    		smkinfo[i].posy_smk=ymin+tempsmk[i]->SMK_OverlayYPos;
    		smkf[i]=tempsmk[i]->SMK_Flags;
    		smkarray=(DIALOGBIN_SMK *)(dialogdata+tempsmk[i]->SMK_Overlay_Offset);
	    }while(tempsmk[i++]->SMK_Overlay_Offset);
	}
	nrofsmks=i;
	tempdialog=tempdialog2;
    unknown=0;
//    if (dialog_opt!=DIALOGBIN_TEST)
//    {
	getmaxsymbolsize(fontnr,&maxsymbx,&maxsymby);
	textyoffset=(ysize-maxsymby)/2;
	getmessagelen(fontnr,text,&strxlen,NULL);
	textxoffset=(xsize-strxlen)/2;
	if (groupbuttons)
	{
	    if (parcedbuttons==0)
		buttontype=BUTTONTYPELEFT;
	    else
		if (groupbuttons==2)
		    buttontype=BUTTONTYPERIGHT;
		else
		    if (parcedbuttons!=groupbuttons-1)
			buttontype=BUTTONTYPEMIDDLE;
		    else
			buttontype=BUTTONTYPERIGHT;
	}
	else
	    buttontype=BUTTONTYPEFULL;
	
	if (dialog_opt&DIALOGBIN_MAINDIALOGS)
	{
		buttoncolor=MBUTTONCOLOR1;
		textcolor=MTEXTCOLOR1;
		radiocolor=MRADIOCOLOR1;
		checkboxcolor=0x00000000;
		horizbarcolor=0x00000000;
		listcolor=MLISTCOLOR1;
		decorselecteditem=NODECORSELECTEDLISTITEM;
	}
	if (dialog_opt&DIALOGBIN_GAMEDIALOGS)
	{
		buttoncolor=GBUTTONCOLOR2;
		textcolor=GTEXTCOLORBLUE;
		radiocolor=GRADIOBUTTONCOLOR1;
		checkboxcolor=GCHECKBOXCOLOR1;
		horizbarcolor=GHORIZCOLOR1;
		listcolor=GLISTCOLOR1;
		decorselecteditem=DECORSELECTEDLISTITEM;
		textxoffset=0;
	}
	switch(type)
	{
	    case DIALOGBIN_MAIN:
		break;
	    case DIALOGBIN_DEFAULTBUTTON:
		addbuttonitem(menu,elemnr,xmin,ymin,xsize,ysize,textyoffset,
			      hotkey,fontnr,text,buttoncolor,xsize/8,buttontype);
		setdefaultbutton(menu,elemnr);
		//if menu in game we need to draw button with text
		//in other menus we have predefined pictures with drawed buttons
		if (dialog_opt&DIALOGBIN_GAMEDIALOGS)
		    selectdecorobj(menu,elemnr,GDECORBUTTON1);
		parcedbuttons++;
		break;
	    case DIALOGBIN_BUTTON:
		addbuttonitem(menu,elemnr,xmin,ymin,xsize,ysize,textyoffset,
			      hotkey,fontnr,text,buttoncolor,xsize/8,buttontype);
		if (flags&DIALOGBIN_FLAGS_DEFAULTBUTTON)
		    setdefaultbutton(menu,elemnr);
		//if menu in game we need to draw button with text
		//in other menus we have predefined pictures with drawed buttons
		if (dialog_opt&DIALOGBIN_GAMEDIALOGS)
		    selectdecorobj(menu,elemnr,GDECORBUTTON1);
		parcedbuttons++;
		break;
	    case DIALOGBIN_LABELLEFT:
		addtextitem(menu,elemnr,ISLABELLEFT,xmin,ymin,xsize+1,ysize,0,0,text,fontnr,maxsymby,textcolor);
		break;
	    case DIALOGBIN_LABELCENTER:
		addtextitem(menu,elemnr,ISLABELCENTER,xmin,ymin,xsize+1,ysize,0,0,text,fontnr,maxsymby,textcolor);
		break;
	    case DIALOGBIN_LABELRIGHT:
		addtextitem(menu,elemnr,ISLABELRIGHT,xmin,ymin,xsize+1,ysize,0,0,text,fontnr,maxsymby,textcolor);
		break;
	    case DIALOGBIN_OPTIONBUTTON:
		if (firstradioelem==-1)
		    firstradioelem=elemnr;
		addradiobuttonitem(menu,elemnr,xmin,ymin,xsize,ysize,textxoffset,textyoffset,hotkey,text,fontnr,radiocolor,firstradioelem);
		break;
	    case DIALOGBIN_CHECKBOX:
	        addcheckboxitem(menu,elemnr,xmin,ymin,xsize,ysize,22,textyoffset,hotkey,text,fontnr,checkboxcolor,0);
		break;
	    case DIALOGBIN_HORZSCROLLBAR:
		addhorizbutton(menu,elemnr,xmin,ymin+2,xsize,ysize,horizbarcolor,0,0,NULL);//no start param
		break;
	    case DIALOGBIN_COMBOBOX:
		addexpanditem(menu,elemnr,xmin,ymin,xsize,ysize,0,fontnr,ysize,listcolor);
		//selected item 0
		//total items 0
		//by default
		break;
	    case DIALOGBIN_LISTBOX:
		if (firstlistitem==-1)
		    firstlistitem=elemnr;
		addlistboxitem(menu,elemnr,xmin,ymin,xsize,ysize,2,0,fontnr,listcolor,decorselecteditem);
		if (dialog_opt&DIALOGBIN_GAMEDIALOGS)
		    selectdecorobj(menu,elemnr,GDECORLIST1);
		break;
	    case DIALOGBIN_TEXTBOX:
		if (firsttextitem==-1)
		    firsttextitem=elemnr;
		addeditboxitem(menu,elemnr,xmin,ymin,xsize,ysize,2,0,fontnr,"",0,textcolor);
		if (dialog_opt&DIALOGBIN_GAMEDIALOGS)
		    selectdecorobj(menu,elemnr,GDECOREDIT1);
		menu->menu[elemnr].xtextpos = menu->menu[elemnr].hotdeltax+menu->menu[elemnr].xtextdelta;
		menu->menu[elemnr].ytextpos = menu->menu[elemnr].hotdeltay+menu->menu[elemnr].ytextdelta;
		if (firsttextitem==-1)
		    firsttextitem=elemnr;
		break;
	    case DIALOGBIN_IMAGE:
	    	addimageitem(menu,elemnr,xmin,ymin,xsize,ysize,text);
	    	transparentcolor=255;
	    	translucencycolor=255;
	    	if (flags&DIALOGBIN_FLAGS_TRANSPARENTEFFECT)
	    	{
	    	    transparentcolor=0;
	    	}
	    	if (flags&DIALOGBIN_FLAGS_TRANSLUCENCYEFFECT)
	    	{
	    	    transparentcolor=0;
	    	    translucencycolor=254;
		}
		setimgtransparentcolors(menu,elemnr,transparentcolor,translucencycolor);
		break;
	    case DIALOGBIN_SMKBUTTON:
		for (i=0;i<nrofsmks;i++)
		{
		    smkf[i]|=DIALOGBIN_SMKFLAGS_ADDED_UNPACKALLFRAMES;
		    setsmkflags(menu,elemnr,i,smkf[i]);
		}
		addsmkvideo(menu,elemnr,xmin,ymin,xsize,ysize,nrofsmks,smkinfo);
		addbuttontosmk(menu,elemnr,smktextx,smktexty,smkeventxsize,smkeventysize,flags,hotkey,fontnr,text,buttoncolor,0,-1,BF_TEXTGLOWED);
		if (flags&DIALOGBIN_FLAGS_DEFAULTBUTTON)
		    setdefaultbutton(menu,elemnr);
		break;
	    default:
		unknown=1;
		break;
	}
	setitemflags(menu,elemnr,flags);

	
//    }//dialog test
#ifdef SHOWMENUINFO
	printf("elem %d - (%dx%d) - (%dx%d) type=%d flags=%08x text= [%s] txo=%d tyo=%d rax=%d ray=%d\n",
		elemnr,xmin,ymin,xsize,ysize,type,flags,text,
		tempdialog->TextOffsetX,tempdialog->TextOffsetY,
		tempdialog->RespondAreaWidth,tempdialog->RespondAreaHeight);
	for (i=0;i<nrofsmks;i++)
	{
	    printf("SMK%i=[%s] SMKFLAGS=%08x xpos=%d ypos=%d\n",i,smkinfo[i].smkfile,
		    tempsmk[i]->SMK_Flags,tempsmk[i]->SMK_OverlayXPos,tempsmk[i]->SMK_OverlayYPos);
	}
#endif
	tempdialog=(DIALOGBIN *)(dialogdata+tempdialog->NextEntryOffset);
	elemnr++;
    }while(!lastelem);
    menu->defaultlistitem=firstlistitem;
    wfree(dialogdata);
    return(menu);
}
//==========================================
DIALOGBIN_INFO *DialogBin_GetItemCoords(char *dialogbinfilename)
{
    int maxelem,i,lastelem=0,elemnr=0,len;
    int sizex,sizey;
    DIALOGBIN_INFO *alldialogcoords;
    char *dialogdata,*text;
    DIALOGBIN *tempdialog;
    if (mpqloadfile(dialogbinfilename,(char **)&dialogdata))
    {
	printf("error in %s\n",dialogbinfilename);
	return NULL;
    }
    maxelem=GetDialogElements(dialogdata,NULL)+1;
    alldialogcoords=(DIALOGBIN_INFO *)wmalloc(maxelem*sizeof(DIALOGBIN_ITEMINFO)+sizeof(DIALOGBIN_INFO));
    alldialogcoords->nrofitems=maxelem;

    tempdialog=(DIALOGBIN *)dialogdata;
    text=dialogdata+tempdialog->MainStringOffset;
    alldialogcoords->iteminfo[elemnr].xpos=tempdialog->LeftEdge+DELTASCREENX;
    alldialogcoords->iteminfo[elemnr].ypos=tempdialog->TopEdge+DELTASCREENY;
    alldialogcoords->iteminfo[elemnr].xsize=tempdialog->Width;
    alldialogcoords->iteminfo[elemnr].ysize=tempdialog->Height;
    alldialogcoords->iteminfo[elemnr].Flags=tempdialog->Flags;
    alldialogcoords->iteminfo[elemnr].text=NULL;
#ifdef SHOWMENUINFO
	printf("elem %d x=%d,y=%d xs=%d ys=%d [%s] \n",elemnr,
	    alldialogcoords->iteminfo[elemnr].xpos,
	    alldialogcoords->iteminfo[elemnr].ypos,
	    alldialogcoords->iteminfo[elemnr].xsize,
	    alldialogcoords->iteminfo[elemnr].ysize,
	    text);
#endif
    elemnr++;
    tempdialog=(DIALOGBIN *)(dialogdata+sizeof(DIALOGBIN));
    do
    {
	text=dialogdata+tempdialog->MainStringOffset;
	if (!tempdialog->NextEntryOffset)
	    lastelem=1;
	alldialogcoords->iteminfo[elemnr].xpos=tempdialog->LeftEdge;
	alldialogcoords->iteminfo[elemnr].ypos=tempdialog->TopEdge;
	alldialogcoords->iteminfo[elemnr].xsize=tempdialog->Width;
	alldialogcoords->iteminfo[elemnr].ysize=tempdialog->Height;
	alldialogcoords->iteminfo[elemnr].Flags=tempdialog->Flags;
	alldialogcoords->iteminfo[elemnr].fontnr=DialogBin_GetFontNr(alldialogcoords->iteminfo[elemnr].Flags);
	if (alldialogcoords->iteminfo[elemnr].fontnr==-1)
	    alldialogcoords->iteminfo[elemnr].fontnr=GetFontNrFromYSize(tempdialog->Height);
	if (tempdialog->Flags&(DIALOGBIN_FLAGS_HOTKEYFIRSTCHARACTER|DIALOGBIN_FLAGS_VIRTHOTKEY))
	{
	    alldialogcoords->iteminfo[elemnr].hotkey = text[0];
	    text++;
	}
	else
	    alldialogcoords->iteminfo[elemnr].hotkey = 255;
	len=strlen(text);
	if (len)
	{
	    alldialogcoords->iteminfo[elemnr].text=(char*) wmalloc(strlen(text)+1);
	    strcpy(alldialogcoords->iteminfo[elemnr].text,text);
	}
	else
	    alldialogcoords->iteminfo[elemnr].text=NULL;
	getmessagelen(alldialogcoords->iteminfo[elemnr].fontnr,text,&sizex,&sizey);
	alldialogcoords->iteminfo[elemnr].xtextpos=alldialogcoords->iteminfo[elemnr].xpos+(alldialogcoords->iteminfo[elemnr].xsize-sizex)/2;
	alldialogcoords->iteminfo[elemnr].ytextpos=alldialogcoords->iteminfo[elemnr].ypos+(alldialogcoords->iteminfo[elemnr].ysize-sizey)/2;
#ifdef SHOWMENUINFO
	printf("elem %d x=%d,y=%d xs=%d ys=%d [%s] \n",elemnr-1,
	    alldialogcoords->iteminfo[elemnr].xpos,
	    alldialogcoords->iteminfo[elemnr].ypos,
	    alldialogcoords->iteminfo[elemnr].xsize,
	    alldialogcoords->iteminfo[elemnr].ysize,
	    text);
#endif
	elemnr++;
	tempdialog=(DIALOGBIN *)(dialogdata+tempdialog->NextEntryOffset);
    }while(!lastelem);
    wfree(dialogdata);
    return(alldialogcoords);
}
//==========================================
void DialogBin_FreeItemCoords(DIALOGBIN_INFO *allcoords)
{
    int i;
    if (allcoords)
    {
	for (i=0;i<allcoords->nrofitems;i++)
	{
	    if (allcoords->iteminfo[i].text)
		wfree(allcoords->iteminfo[i].text);
	}
	wfree(allcoords);
    }
}
//==========================================
void DialogBin_Save(MENUSTR *menu,char *label,char *filename)
{
    int i;
    int array[100];
    for (i=0;i<100;i++)
	array[i]=i;
    DialogBin_Save(menu,label,filename,array);
}
//==========================================
void DialogBin_Save(MENUSTR *menu,char *label,char *filename,int *sortedarray)
{
    int arrayelemnr,elemnr=0,menuelemnr=0,incrmenuelem,flags,alltext,previoustext,textdataoffset,sizedialog;
    int xwin,ywin,xwinsize,ywinsize,i,len,totalsmk,dialogsmksize,havetxt;
    char *dialogdata,*textdata,*text;
    DIALOGBIN *tempdialog,*newdialog;
    DIALOGBIN_SMK *tempsmk,*oldsmk;
    dialogdata=(char *) wmalloc(50000);
    memset(dialogdata,0,50000);
    textdata=(char *) wmalloc(50000);
    memset(textdata,0,50000);
    textdataoffset=0;
    
    tempdialog=(DIALOGBIN *)dialogdata;
    
    xwin=menu->x;
    ywin=menu->y;
    
    xwinsize=menu->xsize;
    ywinsize=menu->ysize;
    
//    tempdialog->TextOffsetX=0;
//    tempdialog->TextOffsetY=0;
    tempdialog->LeftEdge=xwin;
    tempdialog->TopEdge=menu->y;
    tempdialog->RightEdge=xwin+xwinsize-1;
    tempdialog->BottomEdge=ywin+ywinsize-1;
    tempdialog->Width=xwinsize;
    tempdialog->Height=ywinsize;
    tempdialog->ControlId=elemnr;
    tempdialog->ControlType=DIALOGBIN_MAIN;
    tempdialog->Flags=DIALOGBIN_FLAGS_ITEMVISIBLED;
    tempdialog->SMK_Offset=sizeof(DIALOGBIN);
    tempdialog->MainStringOffset=textdataoffset;
    strcpy(textdata+textdataoffset,label);
    textdataoffset+=strlen(label)+1;
    
    tempdialog=(DIALOGBIN *)(dialogdata+sizeof(DIALOGBIN));
    elemnr++;
    menuelemnr=0;
    previoustext=0;
    do{
	arrayelemnr=sortedarray[menuelemnr];
	flags=menu->menu[arrayelemnr].dialogbin_flags;
	incrmenuelem=1;
        tempdialog->LeftEdge=menu->menu[arrayelemnr].hotdeltax-xwin;
        tempdialog->TopEdge=menu->menu[arrayelemnr].hotdeltay-ywin;
        tempdialog->Width=menu->menu[arrayelemnr].hotxsize;
        tempdialog->Height=menu->menu[arrayelemnr].hotysize;
        tempdialog->RightEdge=tempdialog->LeftEdge+tempdialog->Width-1;
        tempdialog->BottomEdge=tempdialog->TopEdge+tempdialog->Height-1;
	tempdialog->ControlId=elemnr;
        switch(menu->menu[arrayelemnr].itemtype)
        {
	    case ISBUTTON:
    		if (menu->defaultbutton==elemnr-1)
    		{
    		    tempdialog->ControlType=DIALOGBIN_DEFAULTBUTTON;
    		    flags|=DIALOGBIN_FLAGS_DEFAULTBUTTON;
    		}
    		else
    		{
    		    tempdialog->ControlType=DIALOGBIN_BUTTON;
    		}
    		flags|=DIALOGBIN_FLAGS_KEYMOUSERESPONDEVENTS;
    		text=menu->menu[arrayelemnr].item.button->menustr.text[ITEMSHOW_NOFOCUS];
    		break;
	    case ISLISTBOX:
    		    tempdialog->ControlType=DIALOGBIN_LISTBOX;
    		    text=NULL;
    		    flags|=DIALOGBIN_FLAGS_KEYMOUSERESPONDEVENTS;
    		    incrmenuelem+=3;
    		    break;
	    case ISEDITBOX:
    		    tempdialog->ControlType=DIALOGBIN_TEXTBOX;
    		    text=geteditboxtext(menu,arrayelemnr);
    		    flags|=DIALOGBIN_FLAGS_KEYMOUSERESPONDEVENTS;
    		    break;
	    case ISLABELLEFT:
    		    tempdialog->ControlType=DIALOGBIN_LABELLEFT;
    		    text=menu->menu[arrayelemnr].item.textitem->textstr;
    		    tempdialog->Width-=1;
		    incrmenuelem=1;
		    break;
	    case ISLABELCENTER:
    		    tempdialog->ControlType=DIALOGBIN_LABELCENTER;
    		    text=menu->menu[arrayelemnr].item.textitem->textstr;
    		    tempdialog->Width-=1;
		    incrmenuelem=1;
		    break;
	    case ISLABELRIGHT:
    	    	    tempdialog->ControlType=DIALOGBIN_LABELRIGHT;
    		    text=menu->menu[arrayelemnr].item.textitem->textstr;
    		    tempdialog->Width-=1;
		    incrmenuelem=1;
		    break;
	    case ISEXPANDBOX:
    		    tempdialog->ControlType=DIALOGBIN_COMBOBOX;
    		    text=NULL;
    		    flags|=DIALOGBIN_FLAGS_KEYMOUSERESPONDEVENTS;
		    break;
	    case ISCHECKBOX:
    		    tempdialog->ControlType=DIALOGBIN_CHECKBOX;
    		    text=menu->menu[arrayelemnr].item.checkbox->textstr.text[ITEMSHOW_NOFOCUS];
    		    flags|=DIALOGBIN_FLAGS_KEYMOUSERESPONDEVENTS;
		    break;
	    case ISRADIOBUTTON:
    		    tempdialog->ControlType=DIALOGBIN_OPTIONBUTTON;
    		    text=menu->menu[arrayelemnr].item.radiobutton->textstr.text[ITEMSHOW_NOFOCUS];
    		    flags|=DIALOGBIN_FLAGS_KEYMOUSERESPONDEVENTS;
		    break;
	    case ISHORIZBUTTON:
    		    tempdialog->ControlType=DIALOGBIN_HORZSCROLLBAR;
    		    text=NULL;
    		    flags|=DIALOGBIN_FLAGS_KEYMOUSERESPONDEVENTS;
		    break;
	    case ISSMKVIDEO:
		    tempdialog->ControlType=DIALOGBIN_SMKBUTTON;
		    text=menu->menu[arrayelemnr].item.smkvideo->smkbutton->menustr.text[ITEMSHOW_NOFOCUS];
    		    tempdialog->TextOffsetX=menu->menu[arrayelemnr].item.smkvideo->smkbuttonxpos;//-xwin;
    		    tempdialog->TextOffsetY=menu->menu[arrayelemnr].item.smkvideo->smkbuttonypos;//-ywin;
    		    flags|=DIALOGBIN_FLAGS_KEYMOUSERESPONDEVENTS;
    		    flags|=(menu->menu[arrayelemnr].dialogbin_flags&DIALOGBIN_FLAGS_ALLSMKBUTTONALIGN);
		    break;
	    case ISIMAGE:
    		    tempdialog->ControlType=DIALOGBIN_IMAGE;
    		    text=menu->menu[arrayelemnr].item.image->pcxfilename;
		    if (menu->menu[arrayelemnr].item.image->color2==254)
			flags|=DIALOGBIN_FLAGS_TRANSLUCENCYEFFECT;
		    if (menu->menu[arrayelemnr].item.image->color1==0)
			flags|=DIALOGBIN_FLAGS_TRANSPARENTEFFECT;
		    break;
        }
	tempdialog->ControlId=elemnr;
//        flags|=DIALOGBIN_FLAGS_ITEMVISIBLED;
	flags&=~(DIALOGBIN_FLAGS_FONT10|DIALOGBIN_FLAGS_FONT14|DIALOGBIN_FLAGS_FONT16|DIALOGBIN_FLAGS_FONT16X|DIALOGBIN_FLAGS_FONT50);
	switch(menu->menu[arrayelemnr].fontnr)
	{
	    case IDFONT10:
		flags|=DIALOGBIN_FLAGS_FONT10;
		break;
	    case IDFONT14:
		flags|=DIALOGBIN_FLAGS_FONT14;
		break;
	    case IDFONT16:
		flags|=DIALOGBIN_FLAGS_FONT16;
		break;
	    case IDFONT16X:
		flags|=DIALOGBIN_FLAGS_FONT16X;
		break;
	    case IDFONT50:
		flags|=DIALOGBIN_FLAGS_FONT50;
		break;
	}
	if (!text||!text[0])
	    text="";
	{
	    int havetxt=checkfordublicate(textdata,textdataoffset,text);
	    if (havetxt==-1)
	    {
		tempdialog->MainStringOffset=textdataoffset;
		if ((menu->menu[arrayelemnr].hotkey!=255)&&(menu->menu[arrayelemnr].hotkey!=0))
		{
		    textdata[textdataoffset++]=menu->menu[arrayelemnr].hotkey;
		    flags|=DIALOGBIN_FLAGS_HOTKEYFIRSTCHARACTER;
		}
		strcpy(textdata+textdataoffset,text);
		textdataoffset+=strlen(text)+1;
	    }
	    else
	    {
		tempdialog->MainStringOffset=havetxt;
	    }
	}
	//set smkdata structures if any
	dialogsmksize=0;
        if (menu->menu[arrayelemnr].itemtype==ISSMKVIDEO)
        {
	    if (menu->menu[arrayelemnr].item.smkvideo->smks)
	    {
    		tempdialog->SMK_Offset=(char *)tempdialog - (char *)dialogdata + sizeof(DIALOGBIN);
		int totalsmk=menu->menu[arrayelemnr].item.smkvideo->smks->totalsmks;
		dialogsmksize=totalsmk*sizeof(DIALOGBIN_SMK);
		tempsmk=(DIALOGBIN_SMK *)((char *)tempdialog+sizeof(DIALOGBIN));
		for (i=0;i<totalsmk;i++)
		{
		    if (i!=totalsmk-1)
		    {
			tempsmk->SMK_Overlay_Offset=tempdialog->SMK_Offset+(i+1)*sizeof(DIALOGBIN_SMK);
		    }
		    else
		    {
			tempsmk->SMK_Overlay_Offset=0;//no next smk
		    }
		    tempsmk->SMK_Flags=menu->menu[arrayelemnr].dialogbin_smk_flags[i]&0xf;
		    tempsmk->SMK_StringOffset=textdataoffset;
		    len=strlen(menu->menu[arrayelemnr].item.smkvideo->smks->smkid[i].smkfile);
		    strcpy(textdata+textdataoffset,menu->menu[arrayelemnr].item.smkvideo->smks->smkid[i].smkfile);
		    textdataoffset+=len+1;
		    tempsmk->SMK_OverlayXPos=menu->menu[arrayelemnr].item.smkvideo->smks->smkid[i].posx-xwin-menu->menu[arrayelemnr].hotdeltax;
		    tempsmk->SMK_OverlayYPos=menu->menu[arrayelemnr].item.smkvideo->smks->smkid[i].posy-ywin-menu->menu[arrayelemnr].hotdeltay;
		    tempsmk=(DIALOGBIN_SMK *)((char *)tempsmk+sizeof(DIALOGBIN_SMK));
		}
    		if (!totalsmk)
    		    tempdialog->SMK_Offset=0x0;
	    }
	}
	else
	{
    	    tempdialog->SMK_Offset=0x0;
    	}
        tempdialog->Flags=flags;
        newdialog=(DIALOGBIN *)((char *)tempdialog+sizeof(DIALOGBIN)+dialogsmksize);
	
	elemnr++;
        menuelemnr+=incrmenuelem;
	
	if (menuelemnr<menu->elements)
	    tempdialog->NextEntryOffset=(int)((char *)(newdialog)-(char *)(dialogdata));
	else
	    tempdialog->NextEntryOffset=0;//end of all elems
	tempdialog=newdialog;
    }while(menuelemnr<menu->elements);
    sizedialog=int((char *)(tempdialog)-(char *)(dialogdata));
    tempdialog=(DIALOGBIN *)dialogdata;

    tempdialog->MainStringOffset+=sizedialog;
    tempdialog=(DIALOGBIN *)(long((char *)dialogdata+sizeof(DIALOGBIN)));
    elemnr--;
    do
    {
	tempdialog->MainStringOffset+=sizedialog;
	if (tempdialog->SMK_Offset)
	{
	    tempsmk=(DIALOGBIN_SMK *)((char *)tempdialog+sizeof(DIALOGBIN));
	    do{
		oldsmk=tempsmk;
		tempsmk->SMK_StringOffset+=sizedialog;
		tempsmk=(DIALOGBIN_SMK *)((char *)tempsmk+sizeof(DIALOGBIN_SMK));
	    }while(oldsmk->SMK_Overlay_Offset);
	}
	tempdialog=(DIALOGBIN *)(long ((char *)dialogdata+tempdialog->NextEntryOffset));
    }while(--elemnr);
    FILE *f=fopen(filename,"w");
    if (!f)
	printf("Error open %s for write\n",filename);
    else
    {
	fwrite(dialogdata,sizedialog,1,f);
	fwrite(textdata,textdataoffset,1,f);
	fclose(f);
    }
    wfree(textdata);
    wfree(dialogdata);
}
//==========================================
MENUSTR *DialogBin_ShowPermanent(char *dialogbinfilename,int dialog_opt,int addforreserv,
				void (*refreshfunc)(void *,int))
{
    MENUSTR *dialog=LoadDialogBin(dialogbinfilename,dialog_opt,addforreserv);
    if (dialog)
    {
	mytimer.SetMyTimerFunc(refreshfunc,dialog);//launch first time and set the timer 
    }
    return(dialog);
}
//==========================================
void DialogBin_ClosePermanent(MENUSTR *menu)
{
    if (menu)
	mytimer.ClearMyTimerFunc();
}
//==========================================
