#include "stdafx.h"
#include "PressurePlates.h"

#include "Prefabs/Snail.h"
#include "Prefabs/Star.h"
#include "Prefabs/CubePrefab.h"

PressurePlates::PressurePlates(float generalScale, HarryCharacter* pHarry, const std::vector<Snail*>& snails)
	: m_GeneralScale{ generalScale }
	, m_pHarry{ pHarry }
	, m_pSnails{ snails }
{
}

void PressurePlates::Initialize(const SceneContext& /*sceneContext*/)
{
	// First Collider
	// --------------

	auto pObject = AddChild(new GameObject{});
	const XMFLOAT3 boxDimension{ 3.f, 1.f, 3.f };

	// Collision
	PxMaterial* pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	auto pRigidbody = pObject->AddComponent(new RigidBodyComponent{ true });
	pRigidbody->AddCollider(PxBoxGeometry{ boxDimension.x / 2.f, boxDimension.y / 2.f, boxDimension.z / 2.f }, *pDefaultMaterial, true);

	// Transform
	XMFLOAT3 translation{ -10.4f, -9.f, -47.9f };
	pObject->GetTransform()->Translate(translation);

	// Trigger event
	pObject->SetOnTriggerCallBack([=](GameObject*, GameObject* pOther, PxTriggerAction action)
	{
		// If already activated, return
		if (m_FirstActivated) return;

		// Check if was snail
		bool wasSnail{ false };
		for (const auto& currentSnail : m_pSnails)
		{
			if (currentSnail == pOther)
			{
				wasSnail = true;
				break;
			}
		}

		// If was snail
		if (action == PxTriggerAction::ENTER && wasSnail)
		{
			// Activate plate
			m_FirstActivated = true;
		}
	});

	
	// Second Collider
	// ---------------

	pObject = AddChild(new GameObject{});

	// Collision
	pRigidbody = pObject->AddComponent(new RigidBodyComponent{ true });
	pRigidbody->AddCollider(PxBoxGeometry{ boxDimension.x / 2.f, boxDimension.y / 2.f, boxDimension.z / 2.f }, *pDefaultMaterial, true);

	// Transform
	translation = {23.5f, -9.f, -14.3f };
	pObject->GetTransform()->Translate(translation);

	// Trigger event
	pObject->SetOnTriggerCallBack([=](GameObject*, GameObject* pOther, PxTriggerAction action)
	{
		// If already activated, return
		if (m_SecondActivated) return;

		// Check if was snail
		bool wasSnail{ false };
		for (const auto& currentSnail : m_pSnails)
		{
			if (currentSnail == pOther)
			{
				wasSnail = true;
				break;
			}
		}

		// If was snail
		if (action == PxTriggerAction::ENTER && wasSnail)
		{
			// Activate plate
			m_SecondActivated = true;
		}
	});
}

void PressurePlates::Update(const SceneContext& /*sceneContext*/)
{
	// If both plates not activated, return
	if ((m_FirstActivated  && m_SecondActivated) == false) return;

	// If already spawned star, return
	if (m_StarSpawned) return;

	// Spawn star
	m_StarSpawned = true;
	GetScene()->AddChild(new Star{ m_GeneralScale, m_pHarry });

	std::cout << "star spawned" << std::endl;
}