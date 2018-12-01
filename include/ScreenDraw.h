#ifndef _W_SCREENDRAW

#include "Controller.h"

#define _W_SCREENDRAW

class ScreenDraw
{
		SCREEN_REGION	Regions[2];
		int				UsedRegions;
public:
		void			AddRegion(int regionNr,int x,int y,int sx,int sy);
		void			AddRegion(int regionNr,SCREEN_REGION *Region);
		void			CloneRegion(int toRegion,int fromRegion);
		void			TopMessage(void);
		void			UpdateScreen(void);

		inline void		SetRegions(int regions){ UsedRegions = regions; };
		inline void		ClearRegions(void){ SetRegions(0); };

};

#endif
