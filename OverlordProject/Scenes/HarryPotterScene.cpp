#include "stdafx.h"
#include "HarryPotterScene.h"

#include "Materials/ColorMaterial.h"
#include "Materials/DiffuseMaterial.h"

void HarryPotterScene::Initialize()
{
	m_SceneContext.settings.drawGrid = false;

	// Physics
	PxMaterial* pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	// Level Creation
	// --------------
	GameObject* pLevelObject = AddChild(new GameObject());
	const float levelScale{ 0.1f };

	// Level mesh
	ModelComponent* pLevelMesh = pLevelObject->AddComponent(new ModelComponent(L"Meshes/Level.ovm"));
	//pLevelMesh->SetMaterial(MaterialManager::Get()->CreateMaterial<ColorMaterial>());
	DiffuseMaterial* pDiffuseTexture{ MaterialManager::Get()->CreateMaterial<DiffuseMaterial>() };
	pDiffuseTexture->SetDiffuseTexture(L"Textures/LevelTexture.mtl");
	pLevelMesh->SetMaterial(pDiffuseTexture);

	// Level Collision
	RigidBodyComponent* pLevelActor = pLevelObject->AddComponent(new RigidBodyComponent(true));
	PxTriangleMesh* pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Meshes/Level.ovpt");
	pLevelActor->AddCollider(PxTriangleMeshGeometry(pPxTriangleMesh, PxMeshScale(levelScale)), *pDefaultMaterial);
	
	// Transform
	pLevelObject->GetTransform()->Scale(levelScale);
}

void HarryPotterScene::Update()
{

}