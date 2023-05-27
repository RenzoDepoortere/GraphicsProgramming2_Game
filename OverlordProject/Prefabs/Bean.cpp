#include "stdafx.h"
#include "Bean.h"

#include "Materials/DiffuseMaterial.h"

Bean::Bean(float generalScale, const XMFLOAT3& spawnLocation, const XMFLOAT3& forceDirection)
	: m_GeneralScale{ generalScale }
	, m_SpawnLocation{ spawnLocation }
	, m_NrBeans{ 16 }
	, m_ForceDirection{ forceDirection }
	, m_Force{ 5.f }
{
}

void Bean::Initialize(const SceneContext& /*sceneContext*/)
{
	// Transform
	GetTransform()->Translate(m_SpawnLocation);
	GetTransform()->Scale(m_GeneralScale);

	// Mesh
	ModelComponent* pModel{ AddComponent(new ModelComponent{L"Meshes/Props/Beans/Bean.ovm"}) };

	// Random texture
	std::wstring baseTextureName{ L"Textures/Props/Beans/bean_" };
	const int randID{ rand() % m_NrBeans };
	baseTextureName += std::to_wstring(randID) + L".png";

	DiffuseMaterial* pMaterial{ MaterialManager::Get()->CreateMaterial<DiffuseMaterial>() };
	pMaterial->SetDiffuseTexture(baseTextureName);
	pModel->SetMaterial(pMaterial);

	// Collision
	PxMaterial* pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	RigidBodyComponent* pActor = AddComponent(new RigidBodyComponent(false));
	PxConvexMesh* pPxConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/Props/Beans/Bean.ovpc");
	pActor->AddCollider(PxConvexMeshGeometry{ pPxConvexMesh, PxMeshScale{m_GeneralScale} }, *pDefaultMaterial);

	// Shoot in direction
	const XMVECTOR force{ XMVectorScale(XMLoadFloat3(&m_ForceDirection), m_Force) };
	XMFLOAT3 calculatedForce{};
	XMStoreFloat3(&calculatedForce, force);

	pActor->AddForce(calculatedForce, PxForceMode::eIMPULSE);

	//// On collision
	//SetOnTriggerCallBack([=](GameObject*, GameObject* pOther, PxTriggerAction action)
	//{
	//	// If being destroyed, return
	//	if (m_IsBeingDestroyed) return;

	//// If was desiredObject
	//if (action == PxTriggerAction::ENTER && pOther == m_pObjectToHit->GetGameObject())
	//{
	//	// Activate object
	//	m_pObjectToHit->Activate(GetTransform()->GetPosition());

	//	// "Delete"
	//	GetTransform()->Translate(XMFLOAT3{});
	//	m_IsBeingDestroyed = true;
	//}
	//});
}