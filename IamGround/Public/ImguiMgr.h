#pragma once
#include "Base.h"
#include "Tool_Define.h"
#include "Monster.h"
#include "Player.h"
#include "Environment.h"
BEGIN(Engine)
class CGameObject;
class CComponent;
END



BEGIN(TOOL)
class CImguiMgr final:  public CBase
{
    DECLARE_SINGLETON(CImguiMgr)

    enum PICKINGTYPE { ENVIRONMENT, MONSTER, NAVIGATION, TYPE_END};

private:
    CImguiMgr();
    virtual ~CImguiMgr() = default;
public:
    void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void Render();
    void Tick(_float fTimiedelta);
    HRESULT Add_TerrainList(class CGameObject* pGameobject);
    HRESULT Add_MonsterList(class CGameObject* pGameobject);
    HRESULT Add_Map_Mesh();
private:
    void Tool_Data();
    void Standard_Tool();
    void Picking();
    void Object_List();
    void Generate_Player();
    void Navigation_Editor();
    void Add_Cells();


    void EffectTool();
    void Store_Effects(char* Name, void* Arg);
    void EffectListBox(void* Desc);



    HRESULT Save_File();
    HRESULT Load_File();
    HRESULT Save_Map();
    HRESULT Load_Map();
    HRESULT Save_Player();
    HRESULT Load_Player();
    HRESULT Save_Effect();
    HRESULT Load_Effect();



private:
    vector<class CGameObject*>  m_TerrainList;
    vector<class CGameObject*>  m_MonsterList;
    list<class CGameObject*>		m_Objects;
    vector<void*> m_vecDesc;
    vector<string> m_vecEffectNames;


    _vector vPicking;
    _float3 vCampos;
    _float3 vLastPicking;

    _float3 vMapScale;
    _float MapScalePro = 0.f;
    _bool m_bClickLock = false;
    _bool m_bMapAdded = false;
    _bool m_bCheck = false;
    _bool m_bNaviEditor = false;
    _int iItemCurrentMonsterIdx;
    CMonster::MONSTER_ID  m_Curtype;

    _bool m_IsPickSomeThing = false;
    CMonster::MONSTER_DEC m_MonsterDec;
    CPlayer::PLAYERDEC m_playerDec;
    CGameObject* selectedObject = nullptr;


    //CGameObject* pMap = nullptr;
    _bool m_bPickup = false;
    _bool m_bFrameKeyLock = false;
    _bool m_bPlayerPickup = false;
    _bool m_bNaviPicking = false;
    CComponent* pNavigation = nullptr;
    _float3 vPreCells[3] = {};
    _float3 vCells[3] = {};
    char CurrentCell;
public:
    virtual void Free() override;

};

END