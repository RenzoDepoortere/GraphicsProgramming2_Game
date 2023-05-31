#include "stdafx.h"
#include "Trail.h"

#include "Materials/BasicMaterial_Deferred.h"

#include "Prefabs/HarryCharacter.h"
#include "Prefabs/Character.h"
#include "Prefabs/Snail.h"

Trail::Trail(HarryCharacter* pHarry, Snail* pSnail)
	: m_pHarry{ pHarry }
	, m_pSnail{ pSnail }
	, m_DamageCooldown{ 0.25f }
{
}

void Trail::Initialize(const SceneContext& /*sceneContext*/)
{
	// Mesh
	ModelComponent* pModel{ AddComponent(new ModelComponent{L"Meshes/Plane.ovm"}) };

	BasicMaterial_Deferred* pMaterial{ MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>() };
	pMaterial->SetDiffuseMap(L"Textures/Enemies/Snail/Trail.png");
	pModel->SetMaterial(pMaterial);

	// Collision
	PxMaterial* pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	RigidBodyComponent* pActor = AddComponent(new RigidBodyComponent(true));
	pActor->AddCollider(PxBoxGeometry{ 0.5f, 0.05f, 0.5f }, *pDefaultMaterial, true);

	// Transform
	m_Scale = 0.5f;
	GetTransform()->Scale(0.5f);

	// On trigger
	SetOnTriggerCallBack([=](GameObject*, GameObject* pOther, PxTriggerAction action)
	{
		// If just spawned, return
		if (0 < m_DamageCooldown) return;

		// If was Harry
		if (action == PxTriggerAction::ENTER && pOther == m_pHarry->GetCharacter())
		{
			// Deal damage
			m_pSnail->HarryHit();
		}
	});
}

void Trail::Update(const SceneContext& sceneContext)
{
	// Countdown
	if (0 < m_DamageCooldown) m_DamageCooldown -= sceneContext.pGameTime->GetElapsed();

	// Calculate new scale
	const XMFLOAT3 currentScale{ GetTransform()->GetScale() };
	const float dissapearSpeed{ m_Scale * 0.2f };
	const float newScale{ currentScale.x - dissapearSpeed * sceneContext.pGameTime->GetElapsed() };

	GetTransform()->Scale(newScale);

	// If scale is negative or zero
	if (newScale <= 0)
	{
		// Delete
		m_pSnail->RemoveTrail(this);
	}
}