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
	const float levelScale{ 0.05f };

	// Level mesh
	ModelComponent* pLevelMesh = pLevelObject->AddComponent(new ModelComponent(L"Meshes/Level.ovm"));
	//pLevelMesh->SetMaterial(MaterialManager::Get()->CreateMaterial<ColorMaterial>());
	
	const auto pLevelMaterials{ ContentManager::Load<std::vector<TextureData*>>(L"Textures/Map/1_LevelTexture.mtl") };
	DiffuseMaterial* pDiffuseMaterial{ nullptr };
	for (size_t idx{}; idx < pLevelMaterials->size(); ++idx)
	{
		// Create diffuseMaterials
		pDiffuseMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
		pDiffuseMaterial->SetDiffuseTexture(pLevelMaterials->at(idx));

		// Set material
		pLevelMesh->SetMaterial(pDiffuseMaterial, static_cast<UINT8>(idx));
	}


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