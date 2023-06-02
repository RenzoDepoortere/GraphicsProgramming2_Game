#include "stdafx.h"
#include "Star.h"

#include "Materials/BasicMaterial_Deferred.h"

#include "Prefabs/HarryCharacter.h"
#include "Prefabs/Character.h"

#include "Scenes/HarryPotterScene.h"

Star::Star(float generalScale, HarryCharacter* pHarry)
	: m_GeneralScale{ generalScale }
	, m_pHarry{ pHarry }
{
}

void Star::Initialize(const SceneContext& /*sceneContext*/)
{
	// Mesh
	ModelComponent* pModel{ AddComponent(new ModelComponent{L"Meshes/Props/Star/Star.ovm"}) };

	BasicMaterial_Deferred* pMaterial{ MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>() };
	pMaterial->SetDiffuseMap(L"Textures/Props/Star/Star.png");
	pModel->SetMaterial(pMaterial);

	// Collision
	PxMaterial* pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	RigidBodyComponent* pActor = AddComponent(new RigidBodyComponent(true));
	PxConvexMesh* pPxConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/Props/Star/Star.ovpc");
	pActor->AddCollider(PxConvexMeshGeometry{ pPxConvexMesh, PxMeshScale{m_GeneralScale} }, *pDefaultMaterial);

	// Transform
	const XMFLOAT3 translation{ 24.7f, -8.f, -31.3f };
	GetTransform()->Translate(translation);
	GetTransform()->Scale(m_GeneralScale);

	pActor->Translate(translation);
}

void Star::Update(const SceneContext& sceneContext)
{
	HandleRotation(sceneContext);
	CheckHarry();
}

void Star::HandleRotation(const SceneContext& sceneContext)
{
	// Rotate
	const float rotationSpeed{ 60.f };

	m_TotalYaw += rotationSpeed * sceneContext.pGameTime->GetElapsed();
	GetTransform()->Rotate(0.f, m_TotalYaw, 0.f);
}
void Star::CheckHarry()
{
	// Check if Harry close enough
	const float minSqrdDistanceBetwn{ 2.5f };

	const XMFLOAT3 harryPos{ m_pHarry->GetCharacter()->GetTransform()->GetWorldPosition() };
	const XMFLOAT3 direction{ MathHelper::DirectionTo(GetTransform()->GetWorldPosition(), harryPos, false) };
	const float sqrdDistance{ XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&direction))) };

	if (sqrdDistance <= minSqrdDistanceBetwn)
	{
		// Sound

		// Particle

		// Go back to menu
		dynamic_cast<HarryPotterScene*>(GetScene())->SetMainMenu(true);
	}
}