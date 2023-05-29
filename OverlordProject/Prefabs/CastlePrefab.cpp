#include "stdafx.h"
#include "CastlePrefab.h"

//#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Materials/BasicMaterial_Deferred.h"

#include "Prefabs/SkyBox.h"

CastlePrefab::CastlePrefab(float generalScale)
	: m_GeneralScale{ generalScale }
{
}

void CastlePrefab::Initialize(const SceneContext& sceneContext)
{
	// Light
	sceneContext.pLights->SetDirectionalLight({ 15.f, 80.f, -40.f }, { 0.2f, -0.8f, 0.f });
	//sceneContext.pLights->SetDirectionalLight({ 95.6139526f, 66.1346436f,-41.1850471f }, { -0.740129888f, -0.597205281f, 0.309117377f });
	//sceneContext.pLights->SetDirectionalLight({ 55.6139526f, 66.1346436f,-1.1850471f }, { -0.740129888f, -0.597205281f, 0.309117377f });

	// Physics
	PxMaterial* pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	// Level Creation
	// --------------
	GameObject* pLevelObject = AddChild(new GameObject());

	// Level mesh
	ModelComponent* pLevelMesh = pLevelObject->AddComponent(new ModelComponent(L"Meshes/Map/Level.ovm"));

	auto pLevelMaterials{ ContentManager::Load<std::vector<TextureData*>>(L"Textures/Map/1.0_LevelMesh.mtl") };
	BasicMaterial_Deferred* pDiffuseMaterial{ nullptr };
	for (size_t idx{}; idx < pLevelMaterials->size(); ++idx)
	{
		// Create materials
		pDiffuseMaterial = MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>();
		pDiffuseMaterial->SetDiffuseMap(pLevelMaterials->at(idx));

		// Set material
		pLevelMesh->SetMaterial(pDiffuseMaterial, static_cast<UINT8>(idx));
	}


	// Level Collision
	RigidBodyComponent* pLevelActor = pLevelObject->AddComponent(new RigidBodyComponent(true));
	PxTriangleMesh* pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Meshes/Map/Level.ovpt");
	pLevelActor->AddCollider(PxTriangleMeshGeometry(pPxTriangleMesh, PxMeshScale(m_GeneralScale)), *pDefaultMaterial);

	// Transform
	pLevelObject->GetTransform()->Scale(m_GeneralScale);

	// Skybox
	GetScene()->AddChild(new SkyBox());
}