
#define _SIMPLEGRPDEF1   										\
colorbyte = *bytes++;											\
colorbyte = GRP_tableforunitcolor[colorbyte+grcolor];			\
memset(vidadr,colorbyte,packbyte);  							\
vidadr += packbyte;
//=================================================
#define _SIMPLEGRPDEF2   										\
i = packbyte;													\
do{																\
	colorbyte = *bytes++;										\
	colorbyte = GRP_tableforunitcolor[colorbyte+grcolor];		\
	*vidadr++ = colorbyte;										\
}while(--i);
//=================================================
#define _SIMPLEINVGRPDEF1   									\
colorbyte = *bytes++;											\
colorbyte = GRP_tableforunitcolor[colorbyte+grcolor];			\
invmemset(vidadr,colorbyte,packbyte);							\
vidadr-=packbyte;
//=================================================
#define _SIMPLEINVGRPDEF2   									\
i = packbyte;													\
do{																\
	colorbyte = *bytes++;										\
	colorbyte = GRP_tableforunitcolor[colorbyte+grcolor];		\
	*vidadr-- = colorbyte;										\
}while(--i);
//=================================================
#define _TRANSPGRPDEF1   						\
colorbyte = *bytes++;						\
colorbyte = GRP_tableforunitcolor[colorbyte+grcolor];		\
if (colorbyte<=GRP_gr_maxcolordesen)				\
{									\
	i = packbyte;							\
	colordword = colorbyte<<8;					\
	do{								\
		*vidadr = GRP_transpcolors[colordword|(*vidadr)];		\
		vidadr++;							\
	}while(--i);							\
}									\
else								\
{									\
	memset(vidadr,colorbyte,packbyte);				\
	vidadr+=packbyte;						\
}
//=================================================
#define _TRANSPGRPDEF2   						\
i = packbyte;	    						\
do{									\
	colorbyte = *bytes++;						\
	colorbyte = GRP_tableforunitcolor[colorbyte+grcolor];		\
	if (colorbyte<=GRP_gr_maxcolordesen)				\
	{								\
		colorbyte = GRP_transpcolors[((*vidadr)<<8)|colorbyte];	\
	}								\
	*vidadr++ = colorbyte;						\
}while(--i);
//==========================================================
#define _TRANSPINVGRPDEF1   						\
colorbyte = *bytes++;						\
colorbyte = GRP_tableforunitcolor[colorbyte+grcolor];		\
if (colorbyte<=GRP_gr_maxcolordesen)				\
{									\
	i = packbyte;							\
	colordword = colorbyte<<8;					\
	do{								\
		*vidadr = GRP_transpcolors[colordword|(*vidadr)];		\
		vidadr--;							\
	}while(--i);							\
}									\
else								\
{									\
	invmemset(vidadr,colorbyte,packbyte);				\
	vidadr-=packbyte;						\
}
//=================================================
#define _TRANSPINVGRPDEF2   						\
i = packbyte;    							\
do{									\
	colorbyte = *bytes++;						\
	colorbyte = GRP_tableforunitcolor[colorbyte+grcolor];		\
	if (colorbyte<=GRP_gr_maxcolordesen)				\
	{								\
		colorbyte = GRP_transpcolors[((*vidadr)<<8)|colorbyte];	\
	}								\
	*vidadr-- = colorbyte;						\
}while(--i);
//==========================================================
#define _SHADOWGRPDEF1   						\
bytes++;								\
i = packbyte;							\
do{									\
	*vidadr = table[*vidadr];					\
	vidadr++;							\
}while(--i);
//==========================================================
#define _SHADOWGRPDEF2   						\
i=packbyte;								\
bytes+=packbyte;							\
do{									\
	*vidadr = table[*vidadr];					\
	vidadr++;							\
}while(--i);
//==========================================================
#define _SHADOWINVGRPDEF1   						\
bytes++;								\
i = packbyte;							\
do{									\
	*vidadr = table[*vidadr];					\
	vidadr--;							\
}while(--i);
//==========================================================
#define _SHADOWINVGRPDEF2   						\
i=packbyte;								\
bytes+=packbyte;							\
do{									\
	*vidadr = table[*vidadr];					\
	vidadr--;							\
}while(--i);
//==========================================================
#define MASKCOPOSH 3

#define _NONVISGRPDEF1   						\
colorbyte = *bytes++;						\
colorbyte = GRP_tableforunitcolor[colorbyte];			\
if (colorbyte<=GRP_gr_maxcolordesen)				\
{									\
	colorbyte &= MASKCOPOSH;					\
	i = packbyte;							\
	do{								\
		*vidadr = *(vidadr+colorbyte);				\
		vidadr++;							\
	}while(--i);							\
}									\
else								\
{									\
	memset(vidadr,colorbyte,packbyte);				\
	vidadr+=packbyte;						\
}
//=================================================
#define _NONVISGRPDEF2   						\
i = packbyte;    							\
do{									\
	colorbyte = *bytes++;						\
	colorbyte = GRP_tableforunitcolor[colorbyte];			\
	if (colorbyte<=GRP_gr_maxcolordesen)				\
	{								\
		colorbyte = *(vidadr+(colorbyte & MASKCOPOSH));		\
	}								\
	*vidadr++ = colorbyte;						\
}while(--i);
//========================================
#define _NONVISINVGRPDEF1   						\
colorbyte = *bytes++;						\
colorbyte = GRP_tableforunitcolor[colorbyte];			\
if (colorbyte<=GRP_gr_maxcolordesen)				\
{									\
	colorbyte &= MASKCOPOSH;					\
	i = packbyte;							\
	do{								\
		*vidadr = *(vidadr-colorbyte);				\
		vidadr--;							\
	}while(--i);							\
}									\
else								\
{									\
	invmemset(vidadr,colorbyte,packbyte);				\
	vidadr-=packbyte;						\
}
//=================================================
#define _NONVISINVGRPDEF2   						\
i = packbyte;    							\
do{									\
	colorbyte = *bytes++;						\
	colorbyte = GRP_tableforunitcolor[colorbyte];			\
	if (colorbyte<=GRP_gr_maxcolordesen)				\
	{								\
		colorbyte = *(vidadr-(colorbyte & MASKCOPOSH));		\
	}								\
	*vidadr-- = colorbyte;						\
}while(--i);
//========================================
#define _GRDGRPDEF1   							\
i = packbyte;							\
colorbyte = *bytes++;						\
colordword = colorbyte<<8;						\
do{									\
	*vidadr = table[colordword|(*vidadr)];				\
	vidadr++;							\
}while(--i);
//==========================================================
#define _GRDGRPDEF2   							\
i = packbyte;							\
do{									\
	colorbyte = *bytes++;						\
	*vidadr = table[(*vidadr)|(colorbyte<<8)];			\
	vidadr++;							\
}while(--i);
//==========================================================
#define _GRDINVGRPDEF1 							\
i = packbyte;							\
colorbyte = *bytes++;						\
colordword = colorbyte<<8;						\
do{									\
	*vidadr = table[colordword|(*vidadr)];				\
	vidadr--;							\
}while(--i);
//==========================================================
#define _GRDINVGRPDEF2 							\
i = packbyte;							\
do{									\
	colorbyte = *bytes++;						\
	*vidadr = table[(*vidadr)|(colorbyte<<8)];			\
	vidadr--;							\
}while(--i);
//==========================================================
#define _TOTABLEGRPDEF1							\
colorbyte = *bytes++;						\
colorbyte = GRP_tableforunitcolor[colorbyte+grcolor];		\
colorbyte = table[colorbyte];					\
memset(vidadr,colorbyte,packbyte);  				\
vidadr+=packbyte;
//==========================================================
#define _TOTABLEGRPDEF2							\
i = packbyte;							\
do{									\
	colorbyte = *bytes++;						\
	colorbyte = GRP_tableforunitcolor[colorbyte+grcolor];		\
	*vidadr++ = table[colorbyte];					\
}while(--i);
//==========================================================
#define _TOTABLEINVGRPDEF1						\
colorbyte = *bytes++;						\
colorbyte = GRP_tableforunitcolor[colorbyte+grcolor];		\
colorbyte = table[colorbyte];					\
invmemset(vidadr,colorbyte,packbyte);  				\
vidadr-=packbyte;
//==========================================================
#define _TOTABLEINVGRPDEF2						\
i = packbyte;							\
do{									\
	colorbyte = *bytes++;						\
	colorbyte = GRP_tableforunitcolor[colorbyte+grcolor];		\
	*vidadr-- = table[colorbyte];					\
}while(--i);
//==========================================================
#define _FROMTEXTUREGRPDEF1						\
bytes++;								\
memcpy(vidadr,adrtext,packbyte);	  				\
vidadr+=packbyte;							\
adrtext+=packbyte;
//==========================================================
#define _FROMTEXTUREGRPDEF2						\
bytes+=packbyte;							\
memcpy(vidadr,adrtext,packbyte);	  				\
vidadr+=packbyte;							\
adrtext+=packbyte;
//==========================================================





