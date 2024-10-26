#pragma once

#pragma warning (disable : 4251)

#include <d3d11.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <d3dcompiler.h>

#include "fx11\d3dx11effect.h"
#include "DirectXTK\DDSTextureLoader.h"
#include "DirectXTK\WICTextureLoader.h"
#include "DirectXTK\Keyboard.h"
#include "DirectXTK\Mouse.h"
#include "DirectXTK\PrimitiveBatch.h"
#include "DirectXTK\VertexTypes.h"
#include "DirectXTK\SpriteBatch.h"
#include "DirectXTK\SpriteFont.h"
#include "DirectXTK\Effects.h"


#include "assimp\scene.h"
#include "assimp\Importer.hpp"
#include "assimp\postprocess.h"
#include <functional>

#include <string>
#include <iostream>
#include <fstream>
using namespace DirectX;

#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <ctime>
#include <random>
#include <cmath>
#include <io.h>
#include "fmod.hpp"
#pragma comment(lib, "fmod_vc.lib")

namespace Engine
{
	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };
	enum MOUSEWHEELSTATE { WHEEL_UP, WHEEL_DOWN , WHEEL_NONE};
	enum NodeStates { SUCCESS, FAILURE, RUNNING,COOLING, NONE};
	enum Decorator { CoolDown, Deco_End };
	enum CHANNELID { SOUND_BGM, SOUND_BGM2, SOUND_BGM3, SOUND_EFFECT, SOUND_PLAYER, SOUND_MONSTER,SOUND_BOSS, SOUND_UI,
		DUCHANNEL0, DUCHANNEL1, DUCHANNEL2, DUCHANNEL3, DUCHANNEL4, DUCHANNEL5, DUCHANNEL6, DUCHANNEL7,
		DUCHANNEL8, DUCHANNEL9, DUCHANNEL11, DUCHANNEL12, DUCHANNEL13, DUCHANNEL14, DUCHANNEL15, DUCHANNEL16, 
		DUCHANNEL17, DUCHANNEL18, DUCHANNEL19, DUCHANNEL20, DUCHANNEL21, DUCHANNEL22, DUCHANNEL23, DUCHANNEL24, MAXCHANNEL};


	enum class eEVENT_TYPE
	{
		CREATE_OBJECT,		// CreateObject
		DELETE_OBJECT,		// DeleteObject
		SCENE_CHANGE,		// SceneChange
		CLEAR_UI,
		NONE,
	};


}

#include "Engine_Typedef.h"
#include "Engine_Function.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Functor.h"


#if defined(_DEBUG) && !defined(_TOOL)

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG

using namespace Engine;
using namespace std;

