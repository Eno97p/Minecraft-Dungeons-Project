
#pragma region SYSTEM
#include "ImguiMgr.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Layer.h"
#pragma endregion SYSTEM

#include "PipeLine.h"
#include "Terrain.h"
#include "MapTest.h"

#pragma region MONSTER
#include "Skeleton.h"
#include "Skeleton_Horse.h"
#include "ForkLift.h"
#include "Fiona.h"
#include "Creeper.h"
#include "Zombie.h"
#pragma endregion MONSTER

#pragma region ENVIRONMENT
#include "NPC.h"
#include "ParticleMesh.h"
#pragma endregion ENVIRONMENT
IMPLEMENT_SINGLETON(CImguiMgr)



CImguiMgr::CImguiMgr()
{
	
}

void CImguiMgr::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	//m_pGameInstance = CGameInstance::GetInstance();
	//Safe_AddRef(m_pGameInstance);

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(pDevice, pContext);
	ImGui::StyleColorsDark();
	ZeroMemory(&vPicking, sizeof(_float3));
	ZeroMemory(&vCampos, sizeof(_float3));
	ZeroMemory(&vLastPicking, sizeof(_float3));

	ZeroMemory(&m_MonsterDec, sizeof(CMonster::MONSTER_DEC));
	ZeroMemory(&m_playerDec, sizeof(CPlayer::PLAYERDEC));


	m_Curtype = CMonster::MONSTER_ID::ID_END;
	CurrentCell = 'A';
}

void CImguiMgr::Render()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();



	Tool_Data();
	Standard_Tool();
	EffectTool();

	if (m_bCheck)
		Object_List();
	if (m_bNaviEditor)
		Navigation_Editor();


	ImGui::Render();

	CGameInstance::GetInstance()->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	CGameInstance::GetInstance()->Clear_DepthStencil_View();
	CGameInstance::GetInstance()->Draw();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

}

void CImguiMgr::Tick(_float fTimiedelta)
{
	static _float Acctime = 0;
	Acctime += fTimiedelta;
	if (Acctime > 1.f)
	{
		if (m_bFrameKeyLock)
			m_bFrameKeyLock = false;
		Acctime = 0.f;
	}


	if (GetGI->Get_CurrentLevel() < LEVEL_GAMEPLAY)
		return;

	XMStoreFloat3(&vCampos, GetGI->Get_CamPosition());
	
	
	_float4 vPickedpos;
	GetGI->Get_PickPos(&vPickedpos);
	vPicking = XMLoadFloat4(&vPickedpos);


	if (GetGI->isKeyDown(VK_RBUTTON))
	{
		if(m_bClickLock == true)
			m_bClickLock = false;
		if (m_bPickup == true)
			m_bPickup = false;
		if (m_bPlayerPickup == true)
			m_bPlayerPickup = false;
		if (m_bNaviPicking == true)
			m_bNaviPicking = false;
	}

	
	if (GetGI->isKeyDown(VK_LBUTTON))
	{
		if (m_bNaviPicking)
			Add_Cells();
		Picking();
	}


	if (m_bPickup == true)
	{
		static_cast<CMonster*>(selectedObject)->Set_Pos(vPicking);
	}
	if (m_bPlayerPickup == true)
	{
		static_cast<CPlayer*>(GetGI->Find_Object_By_ID(GetGI->Get_CurrentLevel(), TEXT("Layer_Player"), "Player"))->Set_Pos(vPicking);
	}

	

}

HRESULT CImguiMgr::Add_TerrainList(CGameObject* pGameobject)
{
	if (pGameobject == nullptr)
	{
		MSG_BOX("Terrain was NULL");
		return E_FAIL;
	}
	else
	{
		m_TerrainList.push_back(pGameobject);
		return S_OK;
	}
}

HRESULT CImguiMgr::Add_MonsterList(CGameObject* pGameobject)
{
	if (pGameobject == nullptr)
	{
		MSG_BOX("Monster was NULL");
		return E_FAIL;
	}
	else
	{
		m_MonsterList.push_back(pGameobject);
		return S_OK;
	}
}

HRESULT CImguiMgr::Add_Map_Mesh()
{
	CMap::MAP_DEC Dec;
	Dec.Pos = {0.f,0.f,0.f,1.f};
	Dec.Scale = vMapScale;
	CGameInstance::GetInstance()->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Map"), TEXT("Prototype_GameObject_MapTest"), &Dec);
	return S_OK;
}

void CImguiMgr::Tool_Data()
{
	ImGui::Begin("TOOL_DATA");
	ImGui::Text("This is Data of ViewPort.");
	ImGui::Text("Campos : %.2f, %.2f, %.2f",
		vCampos.x,
		vCampos.y,
		vCampos.z
	);

	ImGui::Text("Last Picking Pos : %.2f, %.2f, %.2f",
		vLastPicking.x,
		vLastPicking.y,
		vLastPicking.z
	);


	
	ImGui::Text("Frame : %f", ImGui::GetIO().Framerate);


	if (!m_bClickLock)
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Picking Locked");
	else
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Picking Activated");


	
	ImGui::End();
}

void CImguiMgr::Standard_Tool()
{
	
	ImGui::Begin("Emplace_Window");

	if (ImGui::CollapsingHeader("Monster_Tool", ImGuiTreeNodeFlags_None))
	{
		

		static _float fScale = 0.f;
		static _float fAngle = 0.f;
		ImGui::InputFloat("Scale", &fScale);
		ImGui::InputFloat("Rotation", &fAngle);
		
		m_MonsterDec.Scale = fScale;
		m_MonsterDec.RotationAngle = XMConvertToRadians(fAngle);

		
		if (ImGui::BeginListBox("Monster List"))
		{
			const char* Items[] = { "Creeper", "Zombie","Skeleton","SkeletonHorse", "ForkLift", "Fiona", "Enderman"
				,"NPC","Golem","RedstoneBoss","Geomancer","Vindicator" , "Archillager", "HeartOfEnder" , "Husk", "ItemBox",
			"Door","Chair"};

			for (int i = 0; i < IM_ARRAYSIZE(Items); ++i)
			{
				const bool bSelected = (iItemCurrentMonsterIdx == i);
				if (ImGui::Selectable(Items[i], bSelected))
				{
					iItemCurrentMonsterIdx = i;
					m_Curtype = (CMonster::MONSTER_ID)iItemCurrentMonsterIdx;
				}

				if (bSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
		if (ImGui::Button("PickingActive"))
		{
			m_bClickLock = true;
		}

		/*static _bool check = false;*/
		if (ImGui::Checkbox("CurrentMonsterList", &m_bCheck))
			m_bNaviEditor = false;
	}
	if (ImGui::CollapsingHeader("Map_Tool", ImGuiTreeNodeFlags_None))
	{
		
		ImGui::InputFloat("MapScale", &MapScalePro);
		vMapScale = { MapScalePro,MapScalePro,MapScalePro };

		if (ImGui::Button("Create_Map"))
		{
			if(m_bMapAdded)
				MSG_BOX("Map Already Added");
			else
			{
				Add_Map_Mesh();
				m_bMapAdded = true;
			}
		}

		if (ImGui::Button("Save_Map"))
		{
			Save_Map();
		}
		ImGui::SameLine();
		if (ImGui::Button("Load_Map"))
		{
			Load_Map();
		}

		
		if (ImGui::Checkbox("Navigation_Editor", &m_bNaviEditor))
			m_bCheck = false;


	}
	if (ImGui::CollapsingHeader("Player_Tool", ImGuiTreeNodeFlags_None))
	{
		static _float playerScale = 0.f;
		static _float playerRotation = 0.f;
		ImGui::InputFloat("Scale", &playerScale);
		ImGui::SliderFloat("Rotation", &playerRotation, 0.f, 360.f);

		m_playerDec.Scale = playerScale;
		m_playerDec.RotationAngle = playerRotation;
		XMStoreFloat4(&m_playerDec.Pos , vPicking);

		
		static _bool PlayerAdded = false;
		if (ImGui::Button("Add_Player"))
		{
			if (!PlayerAdded)
			{
				Generate_Player();
				PlayerAdded = true;
			}
			else
				MSG_BOX("Already Added");
		}

		if (ImGui::Button("Save_Player"))
		{
			Save_Player();
		}
		if (ImGui::Button("Load_Player"))
		{
			Load_Player();
		}

		static CGameObject* pPlayer = nullptr;
		if (PlayerAdded == true)
		{
			pPlayer = GetGI->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Player"), "Player");
			vector<string> animvec = pPlayer->Get_AnimNameVec();
			
			for (int i = 0; i < animvec.size(); ++i) {
				ImGui::Text("%d: %s", i, animvec[i].c_str());
			}
			
		}
	}

	ImGui::End();
}



void CImguiMgr::Picking()
{
	
	if (m_bClickLock == false)
		return;


	if (m_Curtype == CMonster::MONSTER_ID::ID_END)
		return;

	XMStoreFloat4(&m_MonsterDec.Pos, vPicking);

	
	switch (m_Curtype)
	{
	case CMonster::MONSTER_ID::CREEPER:
		m_MonsterDec.ID = CMonster::MONSTER_ID::CREEPER;
		CGameInstance::GetInstance()->Add_CloneObject(GetGI->Get_CurrentLevel(), TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Creeper"), &m_MonsterDec);
		break;
	case CMonster::MONSTER_ID::ZOMBIE:
		m_MonsterDec.ID = CMonster::MONSTER_ID::ZOMBIE;
		CGameInstance::GetInstance()->Add_CloneObject(GetGI->Get_CurrentLevel(), TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Zombie"), &m_MonsterDec);
		break;
	case CMonster::MONSTER_ID::SKELETON:
		m_MonsterDec.ID = CMonster::MONSTER_ID::SKELETON;
		CGameInstance::GetInstance()->Add_CloneObject(GetGI->Get_CurrentLevel(), TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Skeleton"), &m_MonsterDec);
		break;
	case CMonster::MONSTER_ID::SKELETON_HORSE:
		m_MonsterDec.ID = CMonster::MONSTER_ID::SKELETON_HORSE;
		CGameInstance::GetInstance()->Add_CloneObject(GetGI->Get_CurrentLevel(), TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Skeleton_Horse"), &m_MonsterDec);
		break;
	case CMonster::MONSTER_ID::FORKLIFT:
		m_MonsterDec.ID = CMonster::MONSTER_ID::FORKLIFT;
		CGameInstance::GetInstance()->Add_CloneObject(GetGI->Get_CurrentLevel(), TEXT("Layer_Monster"), TEXT("Prototype_GameObject_ForkLift"), &m_MonsterDec);
		break;
	case CMonster::MONSTER_ID::FIONA:
		m_MonsterDec.ID = CMonster::MONSTER_ID::FIONA;
		CGameInstance::GetInstance()->Add_CloneObject(GetGI->Get_CurrentLevel(), TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Fiona"), &m_MonsterDec);
		break;
	case CMonster::MONSTER_ID::ENDERMAN:
		m_MonsterDec.ID = CMonster::MONSTER_ID::ENDERMAN;
		CGameInstance::GetInstance()->Add_CloneObject(GetGI->Get_CurrentLevel(), TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Enderman"), &m_MonsterDec);
		break;
	case CMonster::MONSTER_ID::NPC:
		m_MonsterDec.ID = CMonster::MONSTER_ID::NPC;
		CGameInstance::GetInstance()->Add_CloneObject(GetGI->Get_CurrentLevel(), TEXT("Layer_Monster"), TEXT("Prototype_GameObject_NPC"), &m_MonsterDec);
		break;
	case CMonster::MONSTER_ID::GOLEM:
		m_MonsterDec.ID = CMonster::MONSTER_ID::GOLEM;
		CGameInstance::GetInstance()->Add_CloneObject(GetGI->Get_CurrentLevel(), TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Golem"), &m_MonsterDec);
		break;
	case CMonster::MONSTER_ID::REDSTONEBOSS:
		m_MonsterDec.ID = CMonster::MONSTER_ID::REDSTONEBOSS;
		CGameInstance::GetInstance()->Add_CloneObject(GetGI->Get_CurrentLevel(), TEXT("Layer_Monster"), TEXT("Prototype_GameObject_RedstoneBoss"), &m_MonsterDec);
		break;
	case CMonster::MONSTER_ID::GEOMANCER:
		m_MonsterDec.ID = CMonster::MONSTER_ID::GEOMANCER;
		CGameInstance::GetInstance()->Add_CloneObject(GetGI->Get_CurrentLevel(), TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Geomancer"), &m_MonsterDec);
		break;
	case CMonster::MONSTER_ID::VINDICATOR:
		m_MonsterDec.ID = CMonster::MONSTER_ID::VINDICATOR;
		CGameInstance::GetInstance()->Add_CloneObject(GetGI->Get_CurrentLevel(), TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Vindicator"), &m_MonsterDec);
		break;
	case CMonster::MONSTER_ID::ARCHILLAHER:
		m_MonsterDec.ID = CMonster::MONSTER_ID::ARCHILLAHER;
		CGameInstance::GetInstance()->Add_CloneObject(GetGI->Get_CurrentLevel(), TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Archillager"), &m_MonsterDec);
		break;
	case CMonster::MONSTER_ID::HEARTOFENDER:
		m_MonsterDec.ID = CMonster::MONSTER_ID::HEARTOFENDER;
		CGameInstance::GetInstance()->Add_CloneObject(GetGI->Get_CurrentLevel(), TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Heart_Of_Ender"), &m_MonsterDec);
		break;
	case CMonster::MONSTER_ID::HUSK:
		m_MonsterDec.ID = CMonster::MONSTER_ID::HUSK;
		CGameInstance::GetInstance()->Add_CloneObject(GetGI->Get_CurrentLevel(), TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Husk"), &m_MonsterDec);
		break;
	case CMonster::MONSTER_ID::ITEM_BOX:
		m_MonsterDec.ID = CMonster::MONSTER_ID::ITEM_BOX;
		CGameInstance::GetInstance()->Add_CloneObject(GetGI->Get_CurrentLevel(), TEXT("Layer_Monster"), TEXT("Prototype_GameObject_ItemBox"), &m_MonsterDec);
		break;
	case CMonster::MONSTER_ID::DOOR:
		m_MonsterDec.ID = CMonster::MONSTER_ID::DOOR;
		CGameInstance::GetInstance()->Add_CloneObject(GetGI->Get_CurrentLevel(), TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Door"), &m_MonsterDec);
		break;
	case CMonster::MONSTER_ID::CHAIR:
		m_MonsterDec.ID = CMonster::MONSTER_ID::CHAIR;
		CGameInstance::GetInstance()->Add_CloneObject(GetGI->Get_CurrentLevel(), TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Chair"), &m_MonsterDec);
		break;
	default:
		return;
	

	}


	XMStoreFloat3(&vLastPicking, vPicking);

}

void CImguiMgr::Object_List()
{
	if (LEVEL_GAMEPLAY > GetGI->Get_CurrentLevel())
		return;

	ImGui::Begin("Object_List");

	CLayer* Layer = GetGI->Find_Layer(GetGI->Get_CurrentLevel(), TEXT("Layer_Monster"));
	if (Layer != nullptr)
	{
		m_Objects = Layer->Get_Layer_Objects();
	}

	vector<const char*> objectNames;
	for (const auto& obj : m_Objects) {
		objectNames.push_back(obj->Get_Unique_ID());
	}

	static int selectedItemIndex = -1;
	if (ImGui::ListBox("Objects", &selectedItemIndex, objectNames.data(), static_cast<int>(objectNames.size()))) 
	{
		// 선택한 항목에 대한 작업을 수행할 수 있음
		if (selectedItemIndex >= 0 && selectedItemIndex < objectNames.size()) 
		{
			auto iter = m_Objects.begin();
			std::advance(iter, selectedItemIndex);
			selectedObject = *iter;
			static_cast<CMonster*>(selectedObject)->Set_Pick(true);
			m_IsPickSomeThing = true;
			

			for (auto& obj : m_Objects) {
				if (obj != selectedObject) { // 선택된 객체가 아닌 경우에만
					static_cast<CMonster*>(obj)->Set_Pick(false);
				}
			}


		

		}
		
	}
	else
	{
		m_IsPickSomeThing = false;
	}

	

	if (ImGui::Button("Pickup"))
	{
		m_bPickup = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete"))
	{
		GetGI->Delete_Object(GetGI->Get_CurrentLevel(), TEXT("Layer_Monster"), selectedObject);
		selectedObject = nullptr;
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete All"))
	{
		GetGI->Delete_All(GetGI->Get_CurrentLevel(), TEXT("Layer_Monster"));
		selectedObject = nullptr;
	}
	
	if (selectedObject != nullptr)
	{
		if (ImGui::DragInt("AnimIndex", static_cast<CMonster*>(selectedObject)->Get_AnimIndex(), 1.0f, 0,
			static_cast<CMonster*>(selectedObject)->Get_NumAnim()-1))
		{
			static_cast<CMonster*>(selectedObject)->Set_AnimIndex();
		}
	}

	if (ImGui::Button("Save"))
	{
		if (FAILED(Save_File()))
		{
			MSG_BOX("Failed Save Data");
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		if (FAILED(Load_File()))
		{
			MSG_BOX("Failed Load Data");
		}
	}

	




	
	ImGui::End();
}

void CImguiMgr::Generate_Player()
{
	GetGI->Add_CloneObject(GetGI->Get_CurrentLevel(), TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"), &m_playerDec);
	m_bPlayerPickup = true;
}

void CImguiMgr::Navigation_Editor()
{
	if (LEVEL_GAMEPLAY > GetGI->Get_CurrentLevel())
		return;
	
	if(GetGI->Find_Object_By_ID(GetGI->Get_CurrentLevel(),TEXT("Layer_Map"),"Map") == nullptr)
		return;

	ImGui::Begin("Navigation_Editor");

	/*pMap = GetGI->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Map"), "Map");*/
	pNavigation = GetGI->Find_Object_By_ID(GetGI->Get_CurrentLevel(), TEXT("Layer_Map"), "Map")->Get_Component(TEXT("Com_Navigation"));
	
	_int iNumCells = static_cast<CNavigation*>(pNavigation)->Get_Cell_Size();


	ImGui::Text("Cell_Size : %d", iNumCells);


	int MouseCellIndex = static_cast<CNavigation*>(pNavigation)->Get_MouseIndex();
	static _bool DeleteMode = false;
	if (ImGui::Button("Delete"))
	{
		if (static_cast<CNavigation*>(pNavigation)->Get_PickMode())
		{
			static_cast<CNavigation*>(pNavigation)->Set_PickMode(false);
			DeleteMode = false;
		}
		else
		{
			static_cast<CNavigation*>(pNavigation)->Set_PickMode(true);
			DeleteMode = true;
		}
	}

	if (DeleteMode == true)
	{
		_int* Negibor = static_cast<CNavigation*>(pNavigation)->Get_NeighborIndex();
		if (Negibor != nullptr)
		{
			ImGui::Text("Neighbor : %d, %d ,%d", Negibor[0], Negibor[1], Negibor[2]);
		}
		if (GetGI->isKeyDown(VK_DELETE))
		{
			static_cast<CNavigation*>(pNavigation)->Delete_CurrentCell();
		}
	}




	if (!m_bNaviPicking)
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Picking Locked");
	else
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Picking Activated");

	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "MouseOnIndex : %d", MouseCellIndex);

	if (ImGui::Button("NaviMesh_Picking"))
	{
		m_bNaviPicking = true;
	}

	ImGui::Text("CurrentCell : %c", CurrentCell);
	ImGui::Text("LastCellPosA x: %.1f ,y: %.1f,z: %.1f", vPreCells[0].x, vPreCells[0].y, vPreCells[0].z);
	ImGui::Text("LastCellPosB x: %.1f ,y: %.1f,z: %.1f", vPreCells[1].x, vPreCells[1].y, vPreCells[1].z);
	ImGui::Text("LastCellPosC x: %.1f ,y: %.1f,z: %.1f", vPreCells[2].x, vPreCells[2].y, vPreCells[2].z);
	/*Delete_Last_Cell*/
	/*if (GetGI->isKeyUP(VK_CONTROL) && GetGI->isKeyUP('Z'))
	{
		
	}*/

	if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState('Z') && !m_bFrameKeyLock)
	{
		static_cast<CNavigation*>(pNavigation)->Delete_Last_Cell();
		if (static_cast<CNavigation*>(pNavigation)->Get_Cell_Size() > 1)
		{
			memcpy(vPreCells, static_cast<CNavigation*>(pNavigation)->Get_Last_Cells_Position(), sizeof(_float3) * 3);
		}
		m_bFrameKeyLock = true;
	}

	if (ImGui::Button("Save"))
	{
		static_cast<CNavigation*>(pNavigation)->Save_Navigation("../../Client/Bin/BinaryFile/Navigation.dat");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		static_cast<CNavigation*>(pNavigation)->Load_Navigation("../../Client/Bin/BinaryFile/Navigation.dat");
	}

	ImGui::End();

}

void CImguiMgr::Add_Cells()
{
	switch (CurrentCell)
	{
	case 'A':
		XMStoreFloat3(&vCells[0], vPicking);
		CurrentCell++;
		break;
	case 'B':
		XMStoreFloat3(&vCells[1], vPicking);
		CurrentCell++;
		break;
	case 'C':
		XMStoreFloat3(&vCells[2], vPicking);
		static_cast<CNavigation*>(pNavigation)->Create_Cell(vCells);
		CurrentCell -= 2;
		memcpy(vPreCells, vCells, sizeof(_float3) * 3);
		for (auto& iter : vCells)
			XMStoreFloat3(&iter, XMVectorZero());
		break;
	}

	XMStoreFloat3(&vLastPicking, vPicking);

}





void CImguiMgr::EffectTool()
{
	if (GetGI->Get_CurrentLevel() < LEVEL_EFFECT)
		return;

	ImGui::Begin("Effect_Editor");

	static CParticleMesh::PARTICLE_DESC classDesc = {};
	static CVIBuffer_Instance::INSTANCE_DESC instDesc = {};


	ImGui::Text("MeshType :");
	ImGui::SameLine();
	static _bool checkbox1 = false;
	static _bool checkbox2 = false;
	static _bool checkbox3 = false;
	static _bool checkbox4 = false;

	if (ImGui::Checkbox("Cube", &checkbox1))
	{
		if (checkbox1)
		{
			checkbox2 = false;
			checkbox3 = false;
			checkbox4 = false;
		}
		classDesc.eModelType = CUBE;
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Circle", &checkbox2))
	{
		if (checkbox2)
		{
			checkbox1 = false;
			checkbox3 = false;
			checkbox4 = false;
		}
		classDesc.eModelType = CIRCLE;
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("ForkLift", &checkbox3))
	{
		if (checkbox3)
		{
			checkbox1 = false;
			checkbox2 = false;
			checkbox4 = false;
		}
		classDesc.eModelType = FORK;
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Slash", &checkbox4))
	{
		if (checkbox4)
		{
			checkbox1 = false;
			checkbox2 = false;
			checkbox3 = false;
		}
		classDesc.eModelType = SLASH;
	}


	static _float vStartColor[3] = { 1.0f, 1.0f, 1.0f };
	if (ImGui::ColorEdit3("Start_Color", vStartColor))
	{
		classDesc.vStartColor = _float3(vStartColor[0], vStartColor[1], vStartColor[2]);
	}

	static _float vEndColor[3] = { 1.0f, 1.0f, 1.0f };
	if (ImGui::ColorEdit3("End_Color", vEndColor))
	{
		classDesc.vEndColor = _float3(vEndColor[0], vEndColor[1], vEndColor[2]);
	}

	ImGui::Text("FunctionType :");
	static EFFECTTYPE eType = SPREAD;
	if (ImGui::RadioButton("Spread", eType == SPREAD))
	{
		eType = SPREAD;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Drop", eType == DROP))
	{
		eType = DROP;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("GrowOut", eType == GROWOUT))
	{
		eType = GROWOUT;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Spread_SizeUp", eType == SPREAD_SIZEUP))
	{
		eType = SPREAD_SIZEUP;
	}

	if (ImGui::RadioButton("Spread_NonRotation", eType == SPREAD_NONROTATION))
	{
		eType = SPREAD_NONROTATION;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Tornado", eType == TORNADO))
	{
		eType = TORNADO;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Spread_SpeedDown", eType == SPREAD_SPEED_DOWN))
	{
		eType = SPREAD_SPEED_DOWN;
	}

	if (ImGui::RadioButton("SlashEffect", eType == SLASH_EFFECT))
	{
		eType = SLASH_EFFECT;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Spread_SpeedDown_SizeUp", eType == SPREAD_SPEED_DOWN_SIZE_UP))
	{
		eType = SPREAD_SPEED_DOWN_SIZE_UP;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Gather", eType == GATHER))
	{
		eType = GATHER;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Extinction", eType == EXTINCTION))
	{
		eType = EXTINCTION;
	}
	if (ImGui::RadioButton("GrowOutY", eType == GROWOUTY))
	{
		eType = GROWOUTY;
	}
	

	classDesc.eType = eType;

	ImGui::InputScalar("NumInstance", ImGuiDataType_U32, &instDesc.iNumInstance, NULL, NULL, "%u");
	ImGui::InputFloat3("OffsetPos", reinterpret_cast<float*>(&instDesc.vOffsetPos));
	ImGui::InputFloat3("PivotPos", reinterpret_cast<float*>(&instDesc.vPivotPos));
	ImGui::InputFloat3("Range", reinterpret_cast<float*>(&instDesc.vRange));
	ImGui::InputFloat2("Size", reinterpret_cast<float*>(&instDesc.vSize));
	ImGui::InputFloat2("Speed", reinterpret_cast<float*>(&instDesc.vSpeed));
	ImGui::InputFloat2("LifeTime", reinterpret_cast<float*>(&instDesc.vLifeTime));
	ImGui::InputFloat2("Gravity", reinterpret_cast<float*>(&instDesc.vGravity));
	ImGui::InputFloat4("StartPosition", reinterpret_cast<float*>(&classDesc.vStartPos));
	ImGui::Checkbox("IsLoop", &instDesc.isLoop);
	ImGui::SameLine();
	ImGui::Checkbox("IsBlur", &classDesc.IsBlur);

	classDesc.InstanceDesc = instDesc;

	
	if (ImGui::Button("Add" ,ImVec2(100,30)))
	{
		GetGI->Delete_All(GetGI->Get_CurrentLevel(), TEXT("Layer_Effect"));
		GetGI->Add_CloneObject(GetGI->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_ParticleMesh"), &classDesc);
	}

	
	static char text[256] = "";

	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Name :");
	ImGui::SameLine();
	ImGui::InputText("a", text, IM_ARRAYSIZE(text));

	if (ImGui::Button("Store", ImVec2(50, 30)))
	{
		if (text[0] == '\0') 
		{
			MSG_BOX("이름을 입력해주세요");
		}
		else
		{
			Store_Effects(text, &classDesc);
		}
	}


	if (ImGui::Button("Save", ImVec2(100, 30)))
	{
		if (FAILED(Save_Effect()))
		{
			MSG_BOX("저장 실패");
		}
		else
			MSG_BOX("저장 성공");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ImVec2(100, 30)))
	{
		if (FAILED(Load_Effect()))
		{
			MSG_BOX("로드 실패");
		}
		else
			MSG_BOX("로드 성공");
	}


	EffectListBox(&classDesc);
	
	ImGui::End();
}

void CImguiMgr::Store_Effects(char* Name, void* Arg)
{
	CParticleMesh::PARTICLE_DESC* store = new CParticleMesh::PARTICLE_DESC;
	*store = *((CParticleMesh::PARTICLE_DESC*)Arg);
	m_vecDesc.emplace_back(store);
	
	string sName = Name;
	m_vecEffectNames.emplace_back(sName);

}

void CImguiMgr::EffectListBox(void* Desc)
{

	if (m_vecDesc.size() < 1)
		return;


	ImGui::Begin("Effect_List");

	if (m_vecDesc.size() != m_vecEffectNames.size())
	{
		MSG_BOX("사이즈가 달라요");
		return;
	}
	static int current_item = 0;

	ImVec2 list_box_size = ImVec2(-1, 200); // 너비는 자동, 높이는 200으로 지정
	if (ImGui::BeginListBox("Effect List", list_box_size))
	{
		for (int i = 0; i < m_vecEffectNames.size(); ++i)
		{
			const bool is_selected = (current_item == i);
			if (ImGui::Selectable(m_vecEffectNames[i].c_str(), is_selected))
			{
				current_item = i;
			}

			// 선택된 항목을 리스트박스 중앙에 스크롤
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	// Delete 버튼을 렌더링하고 클릭 이벤트를 처리
	if (current_item >= 0 && current_item < m_vecDesc.size())
	{

		if (ImGui::Button("Add", ImVec2(100, 30)))
		{
			GetGI->Delete_All(GetGI->Get_CurrentLevel(), TEXT("Layer_Effect"));
			GetGI->Add_CloneObject(GetGI->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_ParticleMesh"), m_vecDesc[current_item]);
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit", ImVec2(100, 30)))
		{
			Safe_Delete(m_vecDesc[current_item]);
			CParticleMesh::PARTICLE_DESC* store = new CParticleMesh::PARTICLE_DESC;
			*store = *((CParticleMesh::PARTICLE_DESC*)Desc);
			m_vecDesc[current_item] = store;
		}

		if (ImGui::Button("Set_Blur", ImVec2(100, 30)))
		{
			if (((CParticleMesh::PARTICLE_DESC*)m_vecDesc[current_item])->IsBlur == true)
				((CParticleMesh::PARTICLE_DESC*)m_vecDesc[current_item])->IsBlur = false;
			else
				((CParticleMesh::PARTICLE_DESC*)m_vecDesc[current_item])->IsBlur = true;
		}


		if (ImGui::Button("Delete", ImVec2(100, 30)))
		{
			// 선택된 항목 삭제
			Safe_Delete(m_vecDesc[current_item]); 
			m_vecDesc.erase(m_vecDesc.begin() + current_item);
			m_vecEffectNames.erase(m_vecEffectNames.begin() + current_item);

			// 항목 삭제 후 인덱스 조정
			if (current_item >= m_vecDesc.size())
				current_item = m_vecDesc.size() - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete All", ImVec2(100, 30)))
		{
			// 선택된 항목 삭제
			for (auto& iter : m_vecDesc)
				Safe_Delete(iter);
			m_vecDesc.clear();
			m_vecEffectNames.clear();

			// 항목 삭제 후 인덱스 조정
			current_item = 0;
		}





	}

	ImGui::End();

}

HRESULT CImguiMgr::Save_File()
{
	string finalPath = "../../Client/Bin/BinaryFile/SaveData.dat";
	ofstream file(finalPath, ios::out | ios::binary);

	size_t objNum = m_Objects.size();
	file.write((char*)&objNum, sizeof(size_t));
	for (auto iter : m_Objects)
	{
		CMonster::MONSTER_DEC* Dec = {}; 
		if (static_cast<CMonster*>(iter)->Get_Dec() == nullptr)
			continue;
		else
			Dec = static_cast<CMonster*>(iter)->Get_Dec();
		
		file.write((char*)Dec, sizeof(CMonster::MONSTER_DEC));
		
	}

	file.close();
	MSG_BOX("Save Complete");
	return S_OK;
}

HRESULT CImguiMgr::Load_File()
{
	string finalPath = "../../Client/Bin/BinaryFile/SaveData.dat";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())		//경로 안에 파일이 없으면
	{
		return E_FAIL;
	}		
	if (!inFile.is_open()) {
		// 이진 파일 열기 실패
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	else
	{
		size_t objNum;
		inFile.read((char*)&objNum, sizeof(size_t));
		for (size_t i = 0; i < objNum; ++i)
		{
			CMonster::MONSTER_DEC Dec = {};
			inFile.read((char*)&Dec, sizeof(CMonster::MONSTER_DEC));
			
			switch (Dec.ID)
			{
			case CMonster::MONSTER_ID::CREEPER:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Creeper"), &Dec);
				break;
			case CMonster::MONSTER_ID::ZOMBIE:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Zombie"), &Dec);
				break;
			case CMonster::MONSTER_ID::SKELETON:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Skeleton"), &Dec);
				break;
			case CMonster::MONSTER_ID::SKELETON_HORSE:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Skeleton_Horse"), &Dec);
				break;
			case CMonster::MONSTER_ID::FORKLIFT:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_ForkLift"), &Dec);
				break;
			case CMonster::MONSTER_ID::FIONA:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Fiona"), &Dec);
				break;
			case CMonster::MONSTER_ID::ENDERMAN:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Enderman"), &Dec);
				break;
			case CMonster::MONSTER_ID::NPC:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_NPC"), &Dec);
				break;
			case CMonster::MONSTER_ID::GOLEM:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Golem"), &Dec);
				break;
			case CMonster::MONSTER_ID::REDSTONEBOSS:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_RedstoneBoss"), &Dec);
				break;
			case CMonster::MONSTER_ID::GEOMANCER:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Geomancer"), &Dec);
				break;
			case CMonster::MONSTER_ID::VINDICATOR:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Vindicator"), &Dec);
				break;
			case CMonster::MONSTER_ID::ARCHILLAHER:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Archillager"), &Dec);
				break;
			case CMonster::MONSTER_ID::HEARTOFENDER:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Heart_Of_Ender"), &Dec);
				break;
			case CMonster::MONSTER_ID::HUSK:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Husk"), &Dec);
				break;
			case CMonster::MONSTER_ID::ITEM_BOX:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_ItemBox"), &Dec);
				break;
			case CMonster::MONSTER_ID::DOOR:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Door"), &Dec);
				break;
			case CMonster::MONSTER_ID::CHAIR:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Chair"), &Dec);
				break;
			}
		}

		inFile.close();
		MSG_BOX("Load Complete");
	}

	return S_OK;
}

HRESULT CImguiMgr::Save_Map()
{
	CGameObject* map = GetGI->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Map"), "Map");
	string finalPath = "../../Client/Bin/BinaryFile/SaveMap.dat";
	ofstream file(finalPath, ios::out | ios::binary);

	CMap::MAP_DEC dec = static_cast<CMap*>(map)->Get_MapDec();
	file.write((char*)&dec, sizeof(CMap::MAP_DEC));

	file.close();
	MSG_BOX("Save Map Complete");
	return S_OK;
}

HRESULT CImguiMgr::Load_Map()
{
	string finalPath = "../../Client/Bin/BinaryFile/SaveMap.dat";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())		//경로 안에 파일이 없으면
	{
		return E_FAIL;
	}
	if (!inFile.is_open()) {
		// 이진 파일 열기 실패
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	else
	{
		CMap::MAP_DEC dec = {};
		inFile.read((char*)&dec, sizeof(CMap::MAP_DEC));
		GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Map"), TEXT("Prototype_GameObject_MapTest"), &dec);
		MSG_BOX("Load Complete");
	}
	inFile.close();

	return S_OK;
}

HRESULT CImguiMgr::Save_Player()
{
	CPlayer::PLAYERDEC dec = {};
	CGameObject* player = GetGI->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Player"), "Player");
	dec = static_cast<CPlayer*>(player)->Get_Dec();
	string finalPath = "../../Client/Bin/BinaryFile/Save_Player.dat";
	ofstream file(finalPath, ios::out | ios::binary);

	file.write((char*)&dec, sizeof(CPlayer::PLAYERDEC));
	file.close();

	MSG_BOX("Save Player Complete");

	return S_OK;
}

HRESULT CImguiMgr::Load_Player()
{

	string finalPath = "../../Client/Bin/BinaryFile/Save_Player.dat";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())		//경로 안에 파일이 없으면
	{
		return E_FAIL;
	}
	if (!inFile.is_open()) {
		// 이진 파일 열기 실패
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	else
	{
		CPlayer::PLAYERDEC dec = {};
		inFile.read((char*)&dec, sizeof(CPlayer::PLAYERDEC));
		GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"), &dec);
		MSG_BOX("Load Complete");
	}
	inFile.close();

	return S_OK;
}

HRESULT CImguiMgr::Save_Effect()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Effect.Bin";
	ofstream file(finalPath, ios::out | ios::binary);

	_uint iSize = m_vecDesc.size();
	file.write((char*)&iSize, sizeof(_uint));
	for (auto& iter : m_vecDesc)
	{
		file.write((char*)&((CParticleMesh::PARTICLE_DESC*)iter)->eType, sizeof(EFFECTTYPE));
		file.write((char*)&((CParticleMesh::PARTICLE_DESC*)iter)->InstanceDesc, sizeof(CVIBuffer_Instance::INSTANCE_DESC));
		file.write((char*)&((CParticleMesh::PARTICLE_DESC*)iter)->eModelType, sizeof(EFFECTMODELTYPE));
		file.write((char*)&((CParticleMesh::PARTICLE_DESC*)iter)->vStartColor, sizeof(_float3));
		file.write((char*)&((CParticleMesh::PARTICLE_DESC*)iter)->vEndColor, sizeof(_float3));
		file.write((char*)&((CParticleMesh::PARTICLE_DESC*)iter)->IsBlur, sizeof(_bool));
	}
	file.close();


	string Path = "../Bin/BinaryFile/EffectText/EffectText.txt";
	ofstream Text(Path, ios::out);

	for (auto& iter : m_vecEffectNames)
	{
		_uint strlength = iter.size();
		Text.write((char*)&strlength, sizeof(_uint));
		Text.write(iter.c_str(), strlength);
	}
	Text.close();

	string filename = "../Bin/BinaryFile/EffectText/Number.txt";
	std::ofstream NumberFile(filename);
	for (size_t i = 0; i < m_vecEffectNames.size(); ++i)
	{
		NumberFile << i << ". " << m_vecEffectNames[i] << std::endl;
	}
	NumberFile.close();


	return S_OK;
}

HRESULT CImguiMgr::Load_Effect()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Effect.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())		//경로 안에 파일이 없으면
	{
		return E_FAIL;
	}

	for (auto& iter : m_vecDesc)
		Safe_Delete(iter);
	m_vecDesc.clear();
	m_vecEffectNames.clear();

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (_uint i = 0; i < iSize; ++i)
	{
		CParticleMesh::PARTICLE_DESC* desc = new CParticleMesh::PARTICLE_DESC;
		inFile.read((char*)&desc->eType, sizeof(EFFECTTYPE));
		inFile.read((char*)&desc->InstanceDesc, sizeof(CVIBuffer_Instance::INSTANCE_DESC));
		inFile.read((char*)&desc->eModelType, sizeof(EFFECTMODELTYPE));
		inFile.read((char*)&desc->vStartColor, sizeof(_float3));
		inFile.read((char*)&desc->vEndColor, sizeof(_float3));
		inFile.read((char*)&desc->IsBlur, sizeof(_bool));
		desc->vStartPos = _float4(0.f, 0.f, 0.f, 1.f);

		m_vecDesc.emplace_back(desc);
	}
	inFile.close();


	string Path = "../Bin/BinaryFile/EffectText/EffectText.txt";
	ifstream Text(Path);
	if (!Text.good())		//경로 안에 파일이 없으면
	{
		return E_FAIL;
	}
	for (_uint i = 0; i < iSize; ++i)
	{
		_uint length;
		Text.read((char*)&length, sizeof(_uint));
		string str(length, '\0');
		Text.read(&str[0], length);
		m_vecEffectNames.emplace_back(str);
	}
	Text.close();

	return S_OK;
}



void CImguiMgr::Free()
{
	for (auto& iter : m_vecDesc)
		Safe_Delete(iter);
}
