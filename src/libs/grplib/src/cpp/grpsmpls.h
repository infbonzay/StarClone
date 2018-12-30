//==========================================================
#define _PUTGRPSAMPLE_(METHOD1,METHOD2)									\
int 	FromLine,SkipLeftPixels,LinesPerPicture,PixelPerLine,PpL,i;		\
int 	SlP,AlreadyPut,putmethod = 0,colordword;						\
GRPPICTURE    *grppict;													\
unsigned char *bytes,*vidadr;											\
unsigned short int *elem;												\
unsigned char packbyte,colorbyte;										\
grppict  = &GRP->Picture[nrpicture];									\
PixelPerLine = grppict->PixelPerLine;									\
LinesPerPicture = grppict->LinesPerPicture;								\
FromLine = 0;															\
y += grppict->SkipUp;													\
if (y<GRP_wminy)														\
{																		\
	FromLine = GRP_wminy - y;											\
	LinesPerPicture -= FromLine;										\
	y = GRP_wminy;														\
	_GRPFROMTEXT1_;														\
}																		\
if (y+LinesPerPicture>GRP_wmaxy)										\
{																		\
	LinesPerPicture = GRP_wmaxy - y + 1;								\
}																		\
SkipLeftPixels = 0;														\
x += grppict->SkipLeft;													\
if (x<GRP_wminx)														\
{																		\
	SkipLeftPixels = GRP_wminx - x;										\
	PixelPerLine -= SkipLeftPixels; 									\
	if (PixelPerLine<=0)												\
		return;															\
		x = GRP_wminx;													\
		putmethod |= 1;													\
		_GRPFROMTEXT2_;													\
}																		\
if (x+PixelPerLine>GRP_wmaxx)											\
{																		\
	PixelPerLine = GRP_wmaxx - x + 1;									\
	if (PixelPerLine<=0)												\
		return;															\
		putmethod |= 2;													\
}																		\
vidadr = (unsigned char *)GRP_vidmem + GRP_scanlineoffsets[y] + x;		\
elem = (unsigned short int *)((unsigned char *)GRP+grppict->OffsetForLine);\
switch(putmethod)														\
{																		\
	case 0:																\
		while(LinesPerPicture-->0)										\
		{																\
			PpL = PixelPerLine;											\
			bytes = (unsigned char *)elem+(unsigned short int)elem[FromLine++];		\
			do{															\
				packbyte = *bytes++;									\
				if (!(packbyte & 0x80))									\
				{														\
					if (packbyte & 0x40)								\
					{													\
						packbyte &= 0x3f;								\
						PpL -= packbyte;								\
						METHOD1;										\
					}													\
					else												\
					{													\
						PpL -= packbyte;								\
						METHOD2;										\
					}													\
				}														\
				else													\
				{														\
					packbyte &= 0x7f;									\
					PpL -= packbyte;									\
					vidadr += packbyte;									\
					_GRPFROMTEXTSKIP_;									\
				}														\
			}while(PpL);												\
			vidadr += GRP_screensizex;									\
			vidadr -= PixelPerLine;										\
			_GRPFROMTEXT3_;												\
		}																\
		break;															\
	case 1:																\
		while(LinesPerPicture-->0)										\
		{																\
			PpL = PixelPerLine;											\
			SlP = SkipLeftPixels;										\
			bytes = (unsigned char *)elem+(unsigned short int)elem[FromLine++];	\
			do{															\
				packbyte = *bytes++;									\
				if (!(packbyte & 0x80))									\
				{														\
					if (packbyte & 0x40)								\
					{													\
						packbyte &= 0x3f;								\
						if (packbyte>SlP)								\
						{												\
							packbyte -= SlP;							\
							SlP = 0;									\
							PpL -= packbyte;							\
							METHOD1;				    				\
						}												\
						else											\
						{												\
							SlP -= packbyte;							\
							bytes++;									\
						}												\
					}													\
					else												\
					{													\
						if (packbyte>SlP)								\
						{												\
							packbyte -= SlP;							\
							bytes += SlP;								\
							SlP = 0;									\
							PpL -= packbyte;							\
							METHOD2;									\
						}												\
						else											\
						{												\
							SlP -= packbyte;							\
							bytes+=packbyte;							\
						}												\
					}													\
				}														\
				else													\
				{														\
					packbyte &= 0x7f;									\
					if (packbyte>SlP)									\
					{													\
						packbyte -= SlP;								\
						SlP = 0;										\
						PpL -= packbyte;								\
						vidadr += packbyte;								\
						_GRPFROMTEXTSKIP_;								\
					}													\
					else												\
					{													\
						SlP -= packbyte;								\
					}													\
				}														\
			}while(PpL);												\
			vidadr += GRP_screensizex;									\
			vidadr -= PixelPerLine;										\
			_GRPFROMTEXT3_;												\
		}																\
		break;															\
		case 2:															\
			while(LinesPerPicture-->0)									\
			{															\
				PpL = PixelPerLine;										\
				AlreadyPut=0;											\
				bytes = (unsigned char *)elem+(unsigned short int)elem[FromLine++];\
				do{														\
					packbyte = *bytes++;								\
					if (!(packbyte & 0x80))								\
					{													\
						if (packbyte & 0x40)							\
						{												\
							packbyte &= 0x3f;							\
							if (packbyte>PpL)							\
								packbyte = PpL;							\
								PpL -= packbyte;						\
								METHOD1;								\
								AlreadyPut += packbyte;					\
						}												\
						else											\
						{												\
							if (packbyte>PpL)							\
								packbyte = PpL;							\
								PpL -= packbyte;						\
								METHOD2;								\
								AlreadyPut += packbyte;					\
						}												\
					}													\
					else												\
					{													\
						packbyte &= 0x7f;								\
						if (packbyte>PpL)								\
							packbyte = PpL;								\
							PpL -= packbyte;							\
							vidadr += packbyte;							\
							AlreadyPut += packbyte;						\
							_GRPFROMTEXTSKIP_;							\
					}													\
				}while(PpL);											\
				vidadr += GRP_screensizex;								\
				vidadr -= AlreadyPut;									\
				_GRPFROMTEXT4_;											\
			}															\
			break;														\
		case 3:															\
			while(LinesPerPicture-->0)									\
			{															\
				PpL = PixelPerLine;										\
				SlP = SkipLeftPixels;									\
				AlreadyPut=0;											\
				bytes = (unsigned char *)elem+(unsigned short int)elem[FromLine++];\
				do{														\
					packbyte = *bytes++;								\
					if (!(packbyte & 0x80))								\
					{													\
						if (packbyte & 0x40)							\
						{												\
							packbyte &= 0x3f;							\
							if (packbyte>SlP)							\
							{											\
								packbyte -= SlP;						\
								SlP = 0;								\
								if (packbyte>PpL)						\
									packbyte = PpL;						\
									PpL -= packbyte;					\
									METHOD1;							\
									AlreadyPut += packbyte;				\
							}											\
							else										\
							{											\
								SlP -= packbyte;						\
								bytes++;								\
							}											\
						}												\
						else											\
						{												\
							if (packbyte>SlP)							\
							{											\
								packbyte -= SlP;						\
								bytes += SlP;							\
								SlP = 0;								\
								if (packbyte>PpL)						\
									packbyte = PpL;						\
									PpL -= packbyte;					\
									METHOD2;							\
									AlreadyPut += packbyte;				\
							}											\
							else										\
							{											\
								SlP -= packbyte;						\
								bytes+=packbyte;						\
							}											\
						}												\
					}													\
					else												\
					{													\
						packbyte &= 0x7f;								\
						if (packbyte>SlP)								\
						{												\
							packbyte -= SlP;							\
							SlP = 0;									\
							if (packbyte>PpL)							\
								packbyte = PpL;							\
								PpL -= packbyte;						\
								vidadr += packbyte;						\
								AlreadyPut += packbyte;					\
								_GRPFROMTEXTSKIP_;						\
						}												\
						else											\
						{												\
							SlP -= packbyte;							\
						}												\
					}													\
				}while(PpL);											\
				vidadr += GRP_screensizex;								\
				vidadr -= AlreadyPut;									\
				_GRPFROMTEXT4_;											\
			}															\
			break;														\
}
//==========================================================
#define _PUTINVGRPSAMPLE_(METHOD1,METHOD2)								\
int 	FromLine,SkipRightPixels,LinesPerPicture,PixelPerLine,PpL,i;	\
int 	SrP,AlreadyPut,putmethod=0,colordword;							\
GRPPICTURE    *grppict;													\
unsigned char *bytes,*vidadr;											\
unsigned short int *elem;												\
unsigned char packbyte,colorbyte;										\
grppict  = &GRP->Picture[nrpicture];									\
PixelPerLine = grppict->PixelPerLine;									\
LinesPerPicture = grppict->LinesPerPicture;								\
x += GRP->SizeX - 1;													\
FromLine = 0;															\
y += grppict->SkipUp;													\
if (y<GRP_wminy)														\
{																		\
	FromLine = GRP_wminy - y;											\
	LinesPerPicture -= FromLine;										\
	y = GRP_wminy;														\
}																		\
if (y+LinesPerPicture>GRP_wmaxy)										\
{																		\
	LinesPerPicture = GRP_wmaxy - y + 1;								\
}																		\
SkipRightPixels = 0;													\
x -= grppict->SkipLeft;													\
if (x>GRP_wmaxx)														\
{																		\
	SkipRightPixels = x - GRP_wmaxx;									\
	PixelPerLine -= SkipRightPixels;									\
	if (PixelPerLine<=0)												\
		return;															\
		x = GRP_wmaxx;													\
		putmethod |= 1;													\
}																		\
if (x-PixelPerLine<GRP_wminx)											\
{																		\
	PixelPerLine = x - GRP_wminx + 1;									\
	if (PixelPerLine<=0)												\
		return;															\
		putmethod |= 2;													\
}																		\
vidadr = (unsigned char *)GRP_vidmem+GRP_scanlineoffsets[y]+x;			\
elem = (unsigned short int *)((char *)GRP+grppict->OffsetForLine);		\
switch(putmethod)														\
{																		\
	case 0:																\
		while(LinesPerPicture-->0)										\
		{																\
			PpL = PixelPerLine;											\
			bytes = (unsigned char *)elem+(unsigned short int)elem[FromLine++];\
			do{															\
				packbyte = *bytes++;									\
				if (!(packbyte & 0x80))									\
				{														\
					if (packbyte & 0x40)								\
					{													\
						packbyte &= 0x3f;								\
						PpL -= packbyte;								\
						METHOD1;										\
					}													\
					else												\
					{													\
						PpL -= packbyte;								\
						METHOD2;										\
					}													\
				}														\
				else													\
				{														\
					packbyte &= 0x7f;									\
					PpL -= packbyte;									\
					vidadr -= packbyte;									\
				}														\
			}while(PpL);												\
			vidadr += GRP_screensizex;									\
			vidadr += PixelPerLine;										\
		}																\
		break;															\
	case 1:																\
		while(LinesPerPicture-->0)										\
		{																\
			PpL = PixelPerLine;											\
			SrP = SkipRightPixels;										\
			bytes = (unsigned char *)elem+(unsigned short int)elem[FromLine++];\
			do{															\
				packbyte = *bytes++;									\
				if (!(packbyte & 0x80))									\
				{														\
					if (packbyte & 0x40)								\
					{													\
						packbyte &= 0x3f;								\
						if (packbyte>SrP)								\
						{												\
							packbyte -= SrP;							\
							SrP = 0;									\
							PpL -= packbyte;							\
							METHOD1;									\
						}												\
						else											\
						{												\
							SrP -= packbyte;							\
							bytes++;									\
						}												\
					}													\
					else												\
					{													\
						if (packbyte>SrP)								\
						{												\
							packbyte -= SrP;					\
							bytes += SrP;						\
							SrP = 0;						\
							PpL -= packbyte;					\
							METHOD2;						\
						}								\
						else							\
						{								\
							SrP -= packbyte;					\
							bytes+=packbyte;					\
						}								\
					}								\
				}									\
				else								\
				{									\
					packbyte &= 0x7f;						\
					if (packbyte>SrP)						\
					{								\
						packbyte -= SrP;						\
						SrP = 0;							\
						PpL -= packbyte;						\
						vidadr -= packbyte;						\
					}								\
					else								\
					{								\
						SrP -= packbyte;						\
					}								\
				}									\
			}while(PpL);								\
			vidadr += GRP_screensizex;						\
			vidadr += PixelPerLine;							\
		}										\
		break;										\
		case 2:										\
			while(LinesPerPicture-->0)							\
			{										\
				PpL = PixelPerLine;							\
				AlreadyPut=0;								\
				bytes = (unsigned char *)elem+(unsigned short int)elem[FromLine++];		\
				do{									\
					packbyte = *bytes++;						\
					if (!(packbyte & 0x80))						\
					{									\
						if (packbyte & 0x40)						\
						{								\
							packbyte &= 0x3f;						\
							if (packbyte>PpL)						\
								packbyte = PpL;						\
								PpL -= packbyte;						\
								METHOD1;							\
								AlreadyPut += packbyte;					\
						}								\
						else								\
						{								\
							if (packbyte>PpL)						\
								packbyte = PpL;						\
								PpL -= packbyte;						\
								METHOD2;							\
								AlreadyPut += packbyte;					\
						}								\
					}									\
					else								\
					{									\
						packbyte &= 0x7f;						\
						if (packbyte>PpL)						\
							packbyte = PpL;						\
							PpL -= packbyte;						\
							vidadr -= packbyte;						\
							AlreadyPut += packbyte;						\
					}									\
				}while(PpL);								\
				vidadr += GRP_screensizex;						\
				vidadr += AlreadyPut;							\
			}										\
			break;									\
		case 3:										\
			while(LinesPerPicture-->0)							\
			{										\
				PpL = PixelPerLine;							\
				SrP = SkipRightPixels;							\
				AlreadyPut=0;								\
				bytes = (unsigned char *)elem+(unsigned short int)elem[FromLine++];		\
				do{									\
					packbyte = *bytes++;						\
					if (!(packbyte & 0x80))						\
					{									\
						if (packbyte & 0x40)						\
						{								\
							packbyte &= 0x3f;						\
							if (packbyte>SrP)						\
							{								\
								packbyte -= SrP;					\
								SrP = 0;						\
								if (packbyte>PpL)					\
									packbyte = PpL;					\
									PpL -= packbyte;					\
									METHOD1;						\
									AlreadyPut += packbyte;					\
							}								\
							else							\
							{								\
								SrP -= packbyte;					\
								bytes++;						\
							}								\
						}								\
						else								\
						{								\
							if (packbyte>SrP)						\
							{								\
								packbyte -= SrP;					\
								bytes += SrP;						\
								SrP = 0;						\
								if (packbyte>PpL)					\
									packbyte = PpL;					\
									PpL -= packbyte;					\
									METHOD2;						\
									AlreadyPut += packbyte;					\
							}								\
							else							\
							{								\
								SrP -= packbyte;					\
								bytes+=packbyte;					\
							}								\
						}								\
					}									\
					else								\
					{									\
						packbyte &= 0x7f;						\
						if (packbyte>SrP)						\
						{								\
							packbyte -= SrP;						\
							SrP = 0;							\
							if (packbyte>PpL)						\
								packbyte = PpL;						\
								PpL -= packbyte;						\
								vidadr -= packbyte;						\
								AlreadyPut += packbyte;					\
						}								\
						else								\
						{								\
							SrP -= packbyte;						\
						}								\
					}									\
				}while(PpL);								\
				vidadr += GRP_screensizex;						\
				vidadr += AlreadyPut;							\
			}										\
			break;									\
}											\
//==========================================================
