#include "stdafx.h"
#include "SpikyScene.h"
#include "Materials/SpikyMaterial.h"
#include "Components/ModelComponent.h"

SpikyScene::SpikyScene() :
	GameScene(L"SpikyScene") {}

void SpikyScene::Initialize()
{
	// Change settings
	// ---------------
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;

	// Camera
	// ------
	//m_SceneContext.pCamera->GetTransform()->Translate(XMFLOAT3{ 0.f, 10.f, -15.f });

	// Sphere
	// ------
	// GameObject
	m_pSpike = new GameObject{};
	AddChild(m_pSpike);

	// Create material
	m_pSpikyMaterial = MaterialManager::Get()->CreateMaterial<SpikyMaterial>();

	// Mesh
	ModelComponent* pModel{ new ModelComponent{ L"Meshes/OctaSphere.ovm" } };
	pModel->SetMaterial(m_pSpikyMaterial);
	m_pSpike->AddComponent<ModelComponent>(pModel);

	// Scale
	const XMFLOAT3 scale{ 15.f, 15.f, 15.f };
	m_pSpike->GetTransform()->Scale(scale);
}

void SpikyScene::Update()
{
	// Rotate
	const float degreesPerSecond{ 20.f };
	const float totalTime{ m_SceneContext.pGameTime->GetTotal() };

	XMFLOAT3 rotation{ 0.f, degreesPerSecond * totalTime, 0.f };
	m_pSpike->GetTransform()->Rotate(rotation, true);
}

void SpikyScene::Draw()
{

}

void SpikyScene::OnGUI()
{
	m_pSpikyMaterial->DrawImGui();
}