#include "stdafx.h"
#include "UberMaterialScene.h"
#include "Materials/UberMaterial.h"
#include "Components/ModelComponent.h"

UberMaterialScene::UberMaterialScene() :
	GameScene(L"UberMaterialScene") {}

void UberMaterialScene::Initialize()
{
	// Change settings
	// ---------------
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;

	// Camera
	// ------
	m_SceneContext.pCamera->GetTransform()->Translate(XMFLOAT3{ 0.f, 10.f, -15.f });

	// Sphere
	// ------
	// GameObject
	GameObject* pSphere{ new GameObject{} };
	AddChild(pSphere);

	// Create material
	m_pUberMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();

	// Mesh
	ModelComponent* pModel{ new ModelComponent{ L"Meshes/Sphere.ovm" } };
	pModel->SetMaterial(m_pUberMaterial);
	pSphere->AddComponent<ModelComponent>(pModel);

	// Scale
	const XMFLOAT3 scale{ 3.f, 3.f, 3.f };
	pSphere->GetTransform()->Scale(scale);
}

void UberMaterialScene::Update()
{

}

void UberMaterialScene::Draw()
{

}

void UberMaterialScene::OnGUI()
{
	m_pUberMaterial->DrawImGui();
}