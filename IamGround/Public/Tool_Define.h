#pragma once
#include <Windows.h>
#include <process.h>


namespace TOOL
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_EFFECT, LEVEL_END };
	enum EFFECTTYPE { SPREAD, DROP, GROWOUT, SPREAD_SIZEUP,
		SPREAD_NONROTATION, TORNADO, SPREAD_SPEED_DOWN,
		SLASH_EFFECT, SPREAD_SPEED_DOWN_SIZE_UP, GATHER, EXTINCTION,
		GROWOUTY ,EFFECT_END };
	enum EFFECTMODELTYPE { CUBE, CIRCLE,FORK, SLASH, TYPE_END };
	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;
using namespace TOOL;