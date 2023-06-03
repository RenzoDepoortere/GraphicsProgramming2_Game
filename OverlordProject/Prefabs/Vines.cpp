#include "stdafx.h"
#include "Vines.h"

#include "Materials/BasicMaterial_Deferred.h"
#include "Components/DestroyCastableComponent.h"

Vines::Vines(float generalScale, HarryCharacter* pHarry)
	: m_GeneralScale{ generalScale }
	, m_pHarry{ pHarry }
{
}

void Vines::Initialize(const SceneContext& /*sceneContext*/)
{
	const XMFLOAT3 translation{ 16.5f, -9.f, -79.f };
	const XMFLOAT3 rotation{ 0.f, 90.f, 0.f};

	// Mesh
	ModelComponent* pModel{ AddComponent(new ModelComponent{L"Meshes/Props/Vines/Vines.ovm"}) };
	
	BasicMaterial_Deferred* pMaterial{ MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>() };
	pMaterial->SetDiffuseMap(L"Textures/Props/Vines/Vines.png");
	pModel->SetMaterial(pMaterial);

	// Collision
	PxMaterial* pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	RigidBodyComponent* pActor = AddComponent(new RigidBodyComponent(true));
	PxConvexMesh* pPxConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/Props/Vines/Vines.ovpc");
	pActor->AddCollider(PxConvexMeshGeometry{ pPxConvexMesh, PxMeshScale{m_GeneralScale} }, *pDefaultMaterial);

	// Castable
	AddComponent(new DestroyCastableComponent(CastableComponent::Diffindo, m_pHarry));

	// Transform
	GetTransform()->Translate(translation);
	GetTransform()->Rotate(rotation);
	GetTransform()->Scale(m_GeneralScale);

	const XMFLOAT3 worldUp{ 0.f, 1.f, 0.f };
	const XMVECTOR actorRotationVector{ XMQuaternionRotationAxis(XMLoadFloat3(&worldUp), rotation.y - 10.f) };
	XMFLOAT4 actorRotation{};
	XMStoreFloat4(&actorRotation, actorRotationVector);

	pActor->Translate(translation);
	pActor->Rotate(actorRotation);
}