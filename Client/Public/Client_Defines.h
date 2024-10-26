#pragma once

#include <process.h>
#include <string>
#define Client_On
namespace Client
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };
	enum EQUIPSLOTUSAGE { EQUIP_WEAPON, EQUIP_ARMOR, EQUIP_BOW, EQIP_END };
	enum ITEMRARITY { COMMON, RARE, UNIQUE, RARITY_END};
	enum EFFECTTYPE {
		SPREAD, DROP, GROWOUT, SPREAD_SIZEUP,
		SPREAD_NONROTATION, TORNADO, SPREAD_SPEED_DOWN,
		SLASH_EFFECT, SPREAD_SPEED_DOWN_SIZE_UP, GATHER, EXTINCTION,
		GROWOUTY, EFFECT_END
	};
	enum EFFECTMODELTYPE { CUBE, CIRCLE, FORK, SLASH, TYPE_END };
	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;
