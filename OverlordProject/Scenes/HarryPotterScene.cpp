#include "stdafx.h"
#include "HarryPotterScene.h"

#include "Materials/ColorMaterial.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/UberMaterial.h"

#include "Prefabs/Character.h"

void HarryPotterScene::Initialize()
{
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = false;
	m_SceneContext.settings.enableOnGUI = true;

	m_GeneralScale = 0.025f;

	InitMap();
	InitPlayer();
}

void HarryPotterScene::Update()
{
	// Hide and set mouse
	//m_SceneContext.pInput->ForceMouseToCenter(true);
	//m_SceneContext.pInput->CursorVisible(false);

	HandleMeshTransform();
}

void HarryPotterScene::OnGUI()
{
	m_pCharacter->DrawImGui();
}

void HarryPotterScene::InitMap()
{
	// Physics
	PxMaterial* pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	// Level Creation
	// --------------
	GameObject* pLevelObject = AddChild(new GameObject());

	// Level mesh
	ModelComponent* pLevelMesh = pLevelObject->AddComponent(new ModelComponent(L"Meshes/Level.ovm"));
	//pLevelMesh->SetMaterial(MaterialManager::Get()->CreateMaterial<ColorMaterial>());

	auto pLevelMaterials{ ContentManager::Load<std::vector<TextureData*>>(L"Textures/Map/1.0_LevelTexture.mtl") };
	DiffuseMaterial* pDiffuseMaterial{ nullptr };
	for (size_t idx{}; idx < pLevelMaterials->size(); ++idx)
	{
		// Create materials
		pDiffuseMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
		pDiffuseMaterial->SetDiffuseTexture(pLevelMaterials->at(idx));

		// Set material
		pLevelMesh->SetMaterial(pDiffuseMaterial, static_cast<UINT8>(idx));
	}


	// Level Collision
	RigidBodyComponent* pLevelActor = pLevelObject->AddComponent(new RigidBodyComponent(true));
	PxTriangleMesh* pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Meshes/Level.ovpt");
	pLevelActor->AddCollider(PxTriangleMeshGeometry(pPxTriangleMesh, PxMeshScale(m_GeneralScale)), *pDefaultMaterial);

	// Transform
	pLevelObject->GetTransform()->Scale(m_GeneralScale);
}
void HarryPotterScene::InitPlayer()
{
	// Physics
	PxMaterial* pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	//Character
	CharacterDesc characterDesc{ pDefaultMaterial };
	characterDesc.actionId_MoveForward = CharacterMoveForward;
	characterDesc.actionId_MoveBackward = CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = CharacterMoveLeft;
	characterDesc.actionId_MoveRight = CharacterMoveRight;
	characterDesc.actionId_Jump = CharacterJump;
	
	characterDesc.controller.height *= 0.6f;
	m_ControllerHeight = characterDesc.controller.height;

	characterDesc.maxMoveSpeed = 6.0f;
	characterDesc.moveAccelerationTime = 0.3f;
	characterDesc.maxFallSpeed = 17.5f;
	characterDesc.fallAccelerationTime = 1.0f;
	characterDesc.JumpSpeed = 7.0f;
	characterDesc.rotationSpeed = 30.f;

	m_pCharacter = AddChild(new Character(characterDesc));
	m_pCharacter->GetTransform()->Translate(-5.f, -6.f, -79.f);

	// Mesh
	m_pCharacterMesh = new GameObject;
	ModelComponent* pModel = m_pCharacterMesh->AddComponent(new ModelComponent(L"Meshes/Harry.ovm"));
	//pModel->SetMaterial(MaterialManager::Get()->CreateMaterial<ColorMaterial>());

	auto pLevelMaterials{ ContentManager::Load<std::vector<TextureData*>>(L"Textures/Character/skharrymesh.mtl") };
	DiffuseMaterial* pDiffuseMaterial{ nullptr };
	for (size_t idx{}; idx < pLevelMaterials->size(); ++idx)
	{
		// Create materials
		pDiffuseMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
		pDiffuseMaterial->SetDiffuseTexture(pLevelMaterials->at(idx));

		// Set material
		pModel->SetMaterial(pDiffuseMaterial, static_cast<UINT8>(idx));
	}

	AddChild(m_pCharacterMesh);
	m_pCharacterMesh->GetTransform()->Scale(m_GeneralScale);

	//Input
	auto inputAction = InputAction(CharacterMoveLeft, InputState::down, 'A');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveRight, InputState::down, 'D');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveForward, InputState::down, 'W');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveBackward, InputState::down, 'S');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterJump, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A);
	m_SceneContext.pInput->AddInputAction(inputAction);
}

void HarryPotterScene::HandleMeshTransform()
{
	// Correctly transform model
	TransformComponent* pControllerTransform{ m_pCharacter->GetController()->GetTransform() };
	const XMFLOAT3 controllerPosition{ pControllerTransform->GetWorldPosition() };
	const float characterBuffer{ 0.5f };
	const float totalYaw{ m_pCharacter->GetTotalYaw() };
	const float angleBuffer{ 90.0f };

	m_pCharacterMesh->GetTransform()->Translate(controllerPosition.x, controllerPosition.y - m_ControllerHeight / 2.f - characterBuffer, controllerPosition.z);
	m_pCharacterMesh->GetTransform()->Rotate(0.f, totalYaw + angleBuffer, 0.f);
}