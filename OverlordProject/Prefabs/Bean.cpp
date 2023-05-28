#include "stdafx.h"
#include "Bean.h"

#include "Materials/DiffuseMaterial.h"

Bean::Bean(float generalScale, GameObject* pHarry, const XMFLOAT3& spawnLocation, const XMFLOAT3& forceDirection)
	: m_GeneralScale{ generalScale }
	, m_pHarry{ pHarry }
	, m_SpawnLocation{ spawnLocation }
	, m_NrBeans{ 16 }
	, m_ForceDirection{ forceDirection }
	, m_Force{ 5.f }
	, m_RotationSpeed{ 50.f }
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
	PxMaterial* pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.2f, 0.7f);

	RigidBodyComponent* pActor = AddComponent(new RigidBodyComponent(false));
	PxConvexMesh* pPxConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/Props/Beans/Bean.ovpc");
	pActor->AddCollider(PxConvexMeshGeometry{ pPxConvexMesh, PxMeshScale{m_GeneralScale} }, *pDefaultMaterial);
	
	pActor->SetConstraint(RigidBodyConstraint::RotX, false);
	pActor->SetConstraint(RigidBodyConstraint::RotY, false);
	pActor->SetConstraint(RigidBodyConstraint::RotZ, false);

	// Shoot in direction
	m_ForceDirection.y += 0.5f;
	const XMVECTOR force{ XMVectorScale(XMVector3Normalize(XMLoadFloat3(&m_ForceDirection)), m_Force) };
	XMFLOAT3 calculatedForce{};
	XMStoreFloat3(&calculatedForce, force);

	pActor->AddForce(calculatedForce, PxForceMode::eIMPULSE);
}

void Bean::Update(const SceneContext& sceneContext)
{
	if (m_IsActivated == false) return;

	// Rotate
	m_TotalYaw += m_RotationSpeed * sceneContext.pGameTime->GetElapsed();
	GetTransform()->Rotate(0.f, m_TotalYaw, 0.f);

	// Check if Harry close enough
	const float minSqrdDistanceBetwn{ 2.5f };

	const XMFLOAT3 harryPos{ m_pHarry->GetTransform()->GetWorldPosition() };
	const XMFLOAT3 direction{ MathHelper::DirectionTo(GetTransform()->GetWorldPosition(), harryPos, false) };
	const float sqrdDistance{ XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&direction))) };

	if (sqrdDistance <= minSqrdDistanceBetwn)
	{
		// Sound

		// "Delete"
		GetTransform()->Translate(XMFLOAT3{});
		m_IsActivated = false;
	}
}