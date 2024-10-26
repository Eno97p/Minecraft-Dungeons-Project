#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	/* 엔진을 초기화하기위해 필요한 데이터 여러개를 묶었다. */
	typedef struct ENGINE_DESC
	{
		bool			isWindowed;
		HWND			hWnd;
		unsigned int	iWinSizeX;
		unsigned int	iWinSizeY;
	};

	typedef struct tagFaceIndices32
	{
		unsigned long		_0, _1, _2;
	}FACEINDICES32;


	typedef struct LIGHT_DESC
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };

		TYPE		eType = { TYPE_END };

		XMFLOAT4	vDirection;
		XMFLOAT4	vPosition;
		float		fRange;

		/* 빛의 기본적인 색상. */
		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;
	};

	typedef struct KEYFRAME
	{
		XMFLOAT3		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT3		vPosition;
		double			Time;
	};

	typedef struct MESH_MATERIAL //메쉬에서 마테리얼 타입을 불러올 때 필요한 텍스쳐 배열을 들고 있는 구조체, 18개 들어있고 첫번째는 NONE
	{
		class CTexture* MaterialTextures[AI_TEXTURE_TYPE_MAX]; 
	}; 

	typedef struct ENGINE_DLL VTXPOS
	{
		XMFLOAT3		vPosition;

		static const unsigned int		iNumElements = { 1 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[1];

	};

	typedef struct ENGINE_DLL VTXPOINT
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;

		static const unsigned int		iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];
	};


	typedef struct ENGINE_DLL VTXPOSTEX
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int		iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];

	};

	typedef struct ENGINE_DLL VTXNORTEX
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int		iNumElements = { 3 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[3];

	};
	
	typedef struct ENGINE_DLL VTXMESH
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		static const unsigned int		iNumElements = { 4 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[4];

	};


	typedef struct ENGINE_DLL VTXANIMMESH
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		XMUINT4			vBlendIndices;
		XMFLOAT4		vBlendWeights;

		static const unsigned int		iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[6];

	};

	typedef struct ENGINE_DLL VTXMATRIX
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vTranslation;
		XMFLOAT2			vLifeTime;
		_float			   vGravity;
	};

	typedef struct ENGINE_DLL VTXINSTANCE_RECT
	{
		static const unsigned int		iNumElements = { 7 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[7];
	};

	typedef struct ENGINE_DLL VTXINSTANCE_POINT
	{
		static const unsigned int		iNumElements = { 7 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[7];
	};




	typedef struct ENGINE_DLL tEvent
	{
		eEVENT_TYPE		eEven;
		DWORD_PTR		lParam;
		DWORD_PTR		wParam;
		DWORD_PTR		pParam;
		DWORD_PTR		eParam;
		DWORD_PTR		fParam;
	};

}
#endif // Engine_Struct_h__
