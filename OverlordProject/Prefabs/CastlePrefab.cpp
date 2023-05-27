#include "stdafx.h"
#include "CastlePrefab.h"

#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Materials/DiffuseMaterial.h"

CastlePrefab::CastlePrefab(float generalScale)
	: m_GeneralScale{ generalScale }
{
}

void CastlePrefab::Initialize(const SceneContext& /*sceneContext*/)
{
	// Physics
	PxMaterial* pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	// Level Creation
	// --------------
	GameObject* pLevelObject = AddChild(new GameObject());

	// Level mesh
	ModelComponent* pLevelMesh = pLevelObject->AddComponent(new ModelComponent(L"Meshes/Map/Level.ovm"));

	auto pLevelMaterials{ ContentManager::Load<std::vector<TextureData*>>(L"Textures/Map/1.0_LevelMesh.mtl") };
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
	PxTriangleMesh* pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Meshes/Map/Level.ovpt");
	pLevelActor->AddCollider(PxTriangleMeshGeometry(pPxTriangleMesh, PxMeshScale(m_GeneralScale)), *pDefaultMaterial);

	// Transform
	pLevelObject->GetTransform()->Scale(m_GeneralScale);
}