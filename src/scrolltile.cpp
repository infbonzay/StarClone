#include "starmap.h"
#include "scrolltile.h"

//=================================
void scrollpal(StarMapInfo *map)
{
	switch (map->terrain)
	{
	case TERRAIN_BADLANDS:
		scrollbadlands((unsigned int *)map->palette);
		map->palettechanges = 1;
		break;
	case TERRAIN_SPACEPLATFORM:
		//			scrollspace((unsigned int *)map->palette);
		break;
	case TERRAIN_INSTALLATION:
		//			scrollinstall((unsigned int *)map->palette);
		break;
	case TERRAIN_ASHWORLD:
		scrollashworld((unsigned int *)map->palette);
		map->palettechanges = 1;
		break;
	case TERRAIN_JUNGLE:
		scrolljungle((unsigned int *)map->palette);
		map->palettechanges = 1;
		break;
	case TERRAIN_DESERT:
		scrolldesert((unsigned int *)map->palette);
		map->palettechanges = 1;
		break;
	case TERRAIN_ARCTIC:
		scrollarctic((unsigned int *)map->palette);
		map->palettechanges = 1;
		break;
	case TERRAIN_TWILIGHT:
		scrolltwilight((unsigned int *)map->palette);
		map->palettechanges = 1;
		break;
	}
}
//=================================
void scrollbadlands(unsigned int *pal)
{
	unsigned int rgba;
	rgba = pal[13];
	pal[13] = pal[12];
	pal[12] = pal[11];
	pal[11] = pal[10];
	pal[10] = pal[9];
	pal[9] = pal[8];
	pal[8] = pal[7];
	pal[7] = rgba;
	rgba = pal[6];
	pal[6] = pal[5];
	pal[5] = pal[4];
	pal[4] = pal[3];
	pal[3] = pal[2];
	pal[2] = pal[1];
	pal[1] = rgba;
	pal[1] = rgba;
	rgba = pal[254];
	pal[254] = pal[253];
	pal[253] = pal[252];
	pal[252] = pal[251];
	pal[251] = pal[250];
	pal[250] = pal[249];
	pal[249] = pal[248];
	pal[248] = rgba;
}
//=================================
void scrollspace(unsigned int *pal)
{
}
//=================================
void scrollinstall(unsigned int *pal)
{
}
//=================================
void scrollashworld(unsigned int *pal)
{
	unsigned int rgba;

	rgba = pal[6];
	pal[6] = pal[5];
	pal[5] = pal[4];
	pal[4] = pal[3];
	pal[3] = pal[2];
	pal[2] = pal[1];
	pal[1] = rgba;
	rgba = pal[254];
	pal[254] = pal[253];
	pal[253] = pal[252];
	pal[252] = pal[251];
	pal[251] = pal[250];
	pal[250] = pal[249];
	pal[249] = pal[248];
	pal[248] = rgba;

}
//=================================
void scrolljungle(unsigned int *pal)
{
	unsigned int rgba;
	rgba = pal[13];
	pal[13] = pal[12];
	pal[12] = pal[11];
	pal[11] = pal[10];
	pal[10] = pal[9];
	pal[9] = pal[8];
	pal[8] = pal[7];
	pal[7] = rgba;
	rgba = pal[6];
	pal[6] = pal[5];
	pal[5] = pal[4];
	pal[4] = pal[3];
	pal[3] = pal[2];
	pal[2] = pal[1];
	pal[1] = rgba;
	pal[1] = rgba;
	rgba = pal[254];
	pal[254] = pal[253];
	pal[253] = pal[252];
	pal[252] = pal[251];
	pal[251] = pal[250];
	pal[250] = pal[249];
	pal[249] = pal[248];
	pal[248] = rgba;
}
//=================================
void scrolldesert(unsigned int *pal)
{
	unsigned int rgba;
	rgba = pal[13];
	pal[13] = pal[12];
	pal[12] = pal[11];
	pal[11] = pal[10];
	pal[10] = pal[9];
	pal[9] = pal[8];
	pal[8] = pal[7];
	pal[7] = pal[6];
	pal[6] = pal[5];
	pal[5] = pal[4];
	pal[4] = pal[3];
	pal[3] = pal[2];
	pal[2] = pal[1];
	pal[1] = rgba;
	rgba = pal[254];
	pal[254] = pal[253];
	pal[253] = pal[252];
	pal[252] = pal[251];
	pal[251] = pal[250];
	pal[250] = pal[249];
	pal[249] = pal[248];
	pal[248] = rgba;
}
//=================================
void scrollarctic(unsigned int *pal)
{
	unsigned int rgba;
	rgba = pal[13];
	pal[13] = pal[12];
	pal[12] = pal[11];
	pal[11] = pal[10];
	pal[10] = pal[9];
	pal[9] = pal[8];
	pal[8] = pal[7];
	pal[7] = pal[6];
	pal[6] = pal[5];
	pal[5] = pal[4];
	pal[4] = pal[3];
	pal[3] = pal[2];
	pal[2] = pal[1];
	pal[1] = rgba;
	rgba = pal[254];
	pal[254] = pal[253];
	pal[253] = pal[252];
	pal[252] = pal[251];
	pal[251] = pal[250];
	pal[250] = pal[249];
	pal[249] = pal[248];
	pal[248] = rgba;
}
//=================================
void scrolltwilight(unsigned int *pal)
{
	unsigned int rgba;
	rgba = pal[13];
	pal[13] = pal[12];
	pal[12] = pal[11];
	pal[11] = pal[10];
	pal[10] = pal[9];
	pal[9] = pal[8];
	pal[8] = pal[7];
	pal[7] = pal[6];
	pal[6] = pal[5];
	pal[5] = pal[4];
	pal[4] = pal[3];
	pal[3] = pal[2];
	pal[2] = pal[1];
	pal[1] = rgba;
	rgba = pal[254];
	pal[254] = pal[253];
	pal[253] = pal[252];
	pal[252] = pal[251];
	pal[251] = pal[250];
	pal[250] = pal[249];
	pal[249] = pal[248];
	pal[248] = rgba;
}
//=================================
