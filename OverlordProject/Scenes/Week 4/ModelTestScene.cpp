#include "stdafx.h"
#include "ModelTestScene.h"
#include "Materials/DiffuseMaterial.h"
#include "Components/ModelComponent.h"

ModelTestScene::ModelTestScene() :
	GameScene(L"ModelTestScene") {}

void ModelTestScene::Initialize()
{
	// PhsyX
	// -----
	PxPhysics& pxPhsyx{ PxGetPhysics() };
	PxMaterial* pDefaultMaterial{ pxPhsyx.createMaterial(0.5f,0.5f,0.2f) };

	// Plane
	// -----
	GameSceneExt::CreatePhysXGroundPlane(*this, pDefaultMaterial);

	// Model
	// -----

	// Create chair
	m_pChair = new GameObject{};
	AddChild(m_pChair);

	// Create material
	DiffuseMaterial* pMaterial{ MaterialManager::Get()->CreateMaterial<DiffuseMaterial>() };
	pMaterial->SetDiffuseTexture(L"Textures/Chair_Dark.dds");

	// Mesh
	ModelComponent* pModel{ new ModelComponent{ L"Meshes/Chair.ovm" } };
	pModel->SetMaterial(pMaterial);
	m_pChair->AddComponent<ModelComponent>(pModel);

	// Rigidbody
	// ---------
	PxConvexMesh* pConvexMesh{ ContentManager::Load<PxConvexMesh>(L"Meshes/Chair.ovpc") };
	RigidBodyComponent* pRigidBody{ m_pChair->AddComponent<RigidBodyComponent>(new RigidBodyComponent{ false }) };
	pRigidBody->AddCollider(PxConvexMeshGeometry{ pConvexMesh }, *pDefaultMaterial);
	
	m_pChair->GetTransform()->Translate(XMFLOAT3{ 0.f,0.5f,0.f });
}

void ModelTestScene::Update()
{

}

void ModelTestScene::Draw()
{

}

void ModelTestScene::OnGUI()
{

}